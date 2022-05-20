#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

//Multithreading
#include <pthread.h>
#include <semaphore.h>

//Networking
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>


#ifndef TANK_H
#include "tank.h"
#endif

#ifndef INFORMATION_H
#include "information.h"
#endif

#ifndef CONFIGURATION_H
#include "configuration.h"
#endif

#ifndef FOR_THREAD_H
#include "for_thread.h"
#endif

#ifndef PROJECTILE_H
#include "projectile.h"
#endif

#ifndef CONTANTS_H
#include "constants.h"
#endif

#ifndef OTHER_METHODS_H
#include "other_methods.h"
#endif

#ifndef SINGLY_LINKED_LIST_H
#include "singly_linked_list.h"
#endif

#ifndef WHOLE_WORLD_H
#include "whole_world.h"
#endif

pthread_mutex_t all_mutexes[MAX_PLAYERS];
pthread_mutex_t players_count_mutex;
pthread_mutex_t global_receivings_mutex;
pthread_mutex_t global_sendings_mutex;

struct singly_linked_node* global_receivings[MAX_PLAYERS];
struct singly_linked_node* global_sendings[MAX_PLAYERS];

void initialize_global_arrays();

void initialize_mutexes();
void destroy_mutexes();

int create_socket(int port);
void close_socket(int sock);

int send_payload(int sock, void* msg, uint32_t msgsize);
struct information* receive_single_information(int* sock);

void send_info_new_player_connected(int player_id, struct tank* tank, int* player_ids);
void send_info_player_disconnected(int player_id, int* player_ids);
void clean_up_after_disconnect(int* csocket, struct sockaddr_in* client, struct tank* tank, int player_id, int* players_ids);

void* connection_handler(void* arg);
void* player_connection_handler(void* arg);

void sender(int* csock, int player_id, struct tank** tanks_in_game, struct singly_linked_node* projectiles, int* player_ids);
struct singly_linked_node* receiver(int* csock);

int calculate_physics(struct whole_world* my_configuration, int player_id);

// Starts the thread that accepts clients and allows to kill the server
int main() {
	pthread_t main_thread;
	bool main_thread_running = true;
	int result = pthread_create(&main_thread, NULL, connection_handler, (void*) &main_thread_running);
	if (result!=0){
			printf("Failed to create thread\n");
	}
	while (1){
		char x;
		scanf(" %c", &x);
		if (x == 'q'){
			break;
		}
	}
	printf("Exiting the main thread!\n");
	main_thread_running = false;
	pthread_join(main_thread, NULL);
	return 0;
}

//Used to set all the global variables to NULL
void initialize_global_arrays(){
	for (int i=0;i<MAX_PLAYERS;i++){
		global_receivings[i] = NULL;
		global_sendings[i] = NULL;
	}
}

//Initializes all mutexes to NULL
void initialize_mutexes(){
	for (int i=0;i<MAX_PLAYERS;i++){
		pthread_mutex_init(&(all_mutexes[i]), NULL);
	}
	pthread_mutex_init(&players_count_mutex, NULL);
	pthread_mutex_init(&global_receivings_mutex, NULL);
	pthread_mutex_init(&global_sendings_mutex, NULL);
}

//Destroy the mutexes and free the resources
void destroy_mutexes(){
	for (int i=0;i<MAX_PLAYERS;i++){
		pthread_mutex_destroy(&(all_mutexes[i]));
	}
	pthread_mutex_destroy(&players_count_mutex);
}

//Creates the socket
int create_socket(int port){
	int sock, err;
	struct sockaddr_in server;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Socket open failed;\n");
		exit(1); //Connection failed
	}
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
	bzero((char*) &server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *)&server , sizeof(server)) < 0){
        printf("Socket binding failed");
		exit(1);
    }
    listen(sock , MAX_PLAYERS); //backlog value - how many incoming connections can queue up
	struct timeval tv;
	tv.tv_sec = NEW_CLIENT_WAIT_TIME_SEC;
    tv.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
    return sock;
}

//Closes the socket
void close_socket(int sock){
	close(sock);
}

//Sends msg of msgsize bytes via given socket and returns number of bytes sent
int send_payload(int sock, void* msg, uint32_t msgsize){
	int nwrite = write(sock, msg, msgsize);
	return nwrite;
}

//Remember to use free!!!!! Allows to receive one information struct. Memory allocation happens here!
struct information* receive_single_information(int *sock){
	char buff[RECEIVER_BUFFER_SIZE];
	bzero(buff, RECEIVER_BUFFER_SIZE);
	int available;
	int nread=read(*sock, buff, RECEIVER_BUFFER_SIZE); //Debug to check if it doesn't wait forever
	if (nread <=0){
		return NULL; //Didn't receive any information
	}
	struct information* returning = (struct information*)malloc(sizeof(struct information));
	if (returning == NULL){
		printf("Error allocating memory receiving information\n");
	}
	struct information* received = (struct information*) buff;
	memcpy(returning, received, sizeof(struct information));
	//information_set_values(returning, received->action, received->type_of, received->player_id, received->x_location, received->y_location, received->tank_angle, received->hp, received->turret_angle);
	return returning;
}

//Sends info to all connected players that new player has connected
void send_info_new_player_connected(int player_id, struct tank* tank, int* player_ids){
	for (int i=0;i<MAX_PLAYERS;i++){
		if (player_ids[i] == USED_ID && player_id != i){ //Send to all connected players but not to the one connecting!
			printf("Trying to send info to player %d\n", i);
			struct information* sending = information_alloc();
			information_set_values(sending, CREATE, TANK, player_id, TANK_SPAWN_POINT_X, TANK_SPAWN_POINT_Y, 0.0, 10.0, 0.0); // default angle, HP, turret_angle
			//Semaphore
			singly_linked_list_add(&global_sendings[i], sending);
			//Semaphore
		}
	}
}

//Sends info to all connected players that new player has disconnected
void send_info_player_disconnected(int player_id, int* player_ids){
	for (int i=0;i<MAX_PLAYERS;i++){
		if (player_ids[i] == USED_ID && player_id != i){ //Send to all connected players but not to the one disconnecting!
			struct information* sending = information_alloc();
			information_set_values(sending, DISCONNECT, TANK, player_id, 0, 0, 0.0, 0.0, 0.0);
			//Semaphore
			singly_linked_list_add(&global_sendings[i], sending);
			//Semaphore
		}
	}
}

//Frees memory when the client disconnects
void clean_up_after_disconnect(int* csocket, struct sockaddr_in* client, struct tank* tank, int player_id, int* players_ids){
	set_id_available(players_ids, player_id);
	free(csocket);
	free(client);
	free(tank);
}

//Accepts connections and creates new threads - one for each client
void* connection_handler(void* arg){
	bool* running = (bool*) arg;
	//Variables for connecting and player recognition
	int players_count = 0;

	int* player_ids = generate_id_list(MAX_PLAYERS);
	if (player_ids == NULL){
		printf("Error allocating memory. Bye!\n");
		return NULL;
	}
	
	//Variables for tanks or projectiles
	struct tank** tanks_in_game = (struct tank**)malloc(MAX_PLAYERS*(sizeof(struct tank*)));
	if (tanks_in_game == NULL){
		printf("Error allocating memory. Bye!\n");
		return NULL;
	}

	struct singly_linked_node* projectiles_in_game = NULL;

	//Variables for networking
	int** csockets = (int**)malloc(MAX_PLAYERS*sizeof(int*));
	if (csockets == NULL){
		printf("Error allocating memory. Bye!\n");
		return NULL;
	}

	struct sockaddr_in** clients = (struct sockaddr_in**)malloc(MAX_PLAYERS*sizeof(struct sockaddr_in*));
	if (clients == NULL){
		printf("Error allocating memory. Bye!\n");
		return NULL;
	}

	struct for_thread** for_threads = (struct for_thread**)malloc(MAX_PLAYERS*sizeof(struct for_thread*));
	if (for_threads == NULL){
		printf("Error allocating memory. Bye!\n");
		return NULL;
	}
	for (int i=0;i<MAX_PLAYERS;i++){
		for_threads[i] = for_thread_alloc();
	}

	pthread_t clients_threads[MAX_PLAYERS];

	struct whole_world* world = whole_world_alloc();
	whole_world_set_values(world, player_ids, tanks_in_game, projectiles_in_game);

	int customer_size = sizeof(struct sockaddr_in);
	int main_socket = create_socket(PORT);

	int temp_socket;
	int current_player_id;
	struct sockaddr_in temp_information;
	printf("Server started listening on port %d\nInput q to stop it!\n", PORT);
	struct timeval tv;
	tv.tv_sec = 0;
    tv.tv_usec = CLIENT_MOVE_WAIT;
	int one = 1;
	while (*running){
		temp_socket = accept(main_socket, (struct sockaddr*) &temp_information, &customer_size);

		//Check if connection succeeded
		if (temp_socket < 0){
            printf("Information: accept() failed\n");
			continue;
        }

		//Prepare ID for the connection. If no is available then disconnect (!!!IN THE FUTURE: SEND SERVER IS FULL!!!)
		current_player_id = return_free_id(player_ids);
		if (current_player_id == USED_ID){
			printf("Currently server is full of players. Try again later\n");
			close_socket(temp_socket);
			continue;
		}
		
		//Now prepare variables for the new client
		pthread_mutex_lock(&(all_mutexes[current_player_id]));
		increment_players_count(&players_count);

		//This might be a further function
		csockets[current_player_id] = (int*)malloc(sizeof(int));
		if (csockets[current_player_id] == NULL){
			printf("Error allocating memory!\n");
			pthread_mutex_unlock(&(all_mutexes[current_player_id]));
			return NULL;
		}
		*csockets[current_player_id] = temp_socket;
		setsockopt(*(csockets[current_player_id]), SOL_TCP, TCP_NODELAY, &one, sizeof(one));
		setsockopt(*(csockets[current_player_id]), SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));

		clients[current_player_id] = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
		if (clients[current_player_id] == NULL){
			printf("Error allocating memory!\n");
			pthread_mutex_unlock(&(all_mutexes[current_player_id]));
			return NULL;
		}

		tanks_in_game[current_player_id] = tank_alloc();
		if (tanks_in_game[current_player_id] == NULL){
			printf("Error allocating memory!\n");
			pthread_mutex_unlock(&(all_mutexes[current_player_id]));
			return NULL;
		}
		
		//sizeof(*) or sizeof()????? memcpy because accept takes pointer, so deepcopy required
		memcpy(clients[current_player_id], &temp_information, sizeof(struct sockaddr_in));

		pthread_mutex_unlock(&(all_mutexes[current_player_id]));
		for_thread_set_values(for_threads[current_player_id], current_player_id, tanks_in_game[current_player_id], projectiles_in_game, csockets[current_player_id], clients[current_player_id], &players_count, world, player_ids);
		
		//Send information to all existing clients that new player as joined
		send_info_new_player_connected(current_player_id, tanks_in_game[current_player_id], player_ids);

		//Create new thread to serve this client
		int result = pthread_create(&clients_threads[current_player_id], NULL, player_connection_handler, (void*)for_threads[current_player_id]);
		pthread_detach(clients_threads[current_player_id]); //Automatically free resources when player disconnects
	}
	printf("Exiting the connection handler thread!\n");

	whole_world_free(world);
	//Destroying clients
	for (int i=0;i<MAX_PLAYERS;i++){
		*for_threads[i]->running = false;
		if (player_ids[i] == USED_ID){
			pthread_join(clients_threads[i], NULL);
		}
	}
	free(player_ids);
	free(tanks_in_game);
	free(projectiles_in_game);
	free(csockets);
	for (int i=0;i<MAX_PLAYERS;i++){
		for_thread_free(for_threads[i]);
	}
	free(for_threads);
	free(clients);

	close_socket(main_socket);
	
	return NULL;	
}

//Sends information about everything :)
void sender(int* csock, int player_id, struct tank** tanks_in_game, struct singly_linked_node* projectiles, int* player_ids){
	struct information* information = information_alloc();
 	if (information==NULL){
 		printf("Error allocating memory in sender!\n");
 		return;
 	}
	int nwrite = 0;

	//Check if there are any global updates - global_sendings
	struct singly_linked_node* iterator = global_sendings[player_id];
	global_sendings[player_id] = NULL;
	struct singly_linked_node* free_helper = NULL;
	while (iterator != NULL){
		nwrite = send_payload(*csock, (void *)iterator->data, sizeof(struct information));
		free_helper = iterator;
		iterator = iterator->next;
		information_free((struct information*)free_helper->data);
		free(free_helper);
	}

	//Send all tanks
	for (int i=0;i<MAX_PLAYERS;i++){
		if (player_ids[i] == USED_ID){
			information_set_values(information, UPDATE, TANK, tanks_in_game[i]->player_id, tanks_in_game[i]->x, tanks_in_game[i]->y, tanks_in_game[i]->tank_angle, tanks_in_game[i]->hp, tanks_in_game[i]->turret_angle);
			nwrite = send_payload(*csock, information, sizeof(struct information));		
		}
	}
	
	//Send projectiles
	//in range len(projectiles)
	information_free(information);
	return;
}

//Returns a list of informations received
struct singly_linked_node* receiver(int* csock){
	bool finished = false;
	struct singly_linked_node* received_informations = NULL;
	while (finished == false){
		//Check connection alive. If not free the memory.
		struct information* received = receive_single_information(csock);
		if (received == NULL){ //No information has been received
			finished = true;
			break;
		}
		singly_linked_list_add(&received_informations, received);
	}
	return received_informations;
}			

//Communicates with particular player
void* player_connection_handler(void* arg){
	struct for_thread* my_configuration = (struct for_thread*) arg;
	
	//Send configuration to the new client
	struct configuration* configuration_to_send = configuration_alloc();
	configuration_set_values(configuration_to_send, WINDOW_WIDTH, WINDOW_HEIGHT, BACKGROUND_SCALE, *(my_configuration->players_count), my_configuration->player_id, TANK_SPAWN_POINT_X, TANK_SPAWN_POINT_Y, DEFAULT_MAP_NUMBER);

	tank_set_values(my_configuration->tank, my_configuration->player_id, TANK_SPAWN_POINT_X, TANK_SPAWN_POINT_Y, NO_ROTATION, FULL_HP, NO_ROTATION, DEFAULT_TANK_SKIN);

	send_payload(*(my_configuration->csocket), configuration_to_send, sizeof(struct configuration));
	printf("New client connected from %s\n", inet_ntoa(my_configuration->client->sin_addr));

	configuration_free(configuration_to_send);
	
	struct singly_linked_node* received_informations;

	float time_start;
	float sleep_time_minus;
	float sleep_time;
	while (*(my_configuration->running)){
		//Receive the information available
		//Remember to clean every information + list element
		//pthread_mutex_lock(&all_mutexes[my_configuration->player_id]);
		time_start = (float)time(NULL);
		global_receivings[my_configuration->player_id] = receiver(my_configuration->csocket);
		//printf("###INFO: Finished receiving info's from player: %d\n", my_configuration->player_id);
		if (calculate_physics(my_configuration->whole_world, my_configuration->player_id) == DISCONNECTED){
			//Player sent info that wants to disconnect
			*(my_configuration->running) = false;
			break;
		}
		//printf("##INFO: Finished calculating physics for player %d\n", my_configuration->player_id);
		sender(my_configuration->csocket, my_configuration->player_id, my_configuration->whole_world->tanks, my_configuration->whole_world->projectiles, my_configuration->whole_world->player_ids);
		//printf("##INFO: Finished sending information to player: %d\n", my_configuration->player_id);
		//pthread_mutex_unlock(&all_mutexes[my_configuration->player_id]);
		sleep_time_minus = time_start-(float)time(NULL);
		sleep_time = 1/COMMUNICATION_INTERVAL - sleep_time_minus;
		usleep(sleep_time*1000000); //Seconds to microseconds conversion
	}
	printf("Client connected from %s disconnected\n", inet_ntoa(my_configuration->client->sin_addr));
	//mutex!
	decrement_players_count(my_configuration->players_count);
	clean_up_after_disconnect(my_configuration->csocket, my_configuration->client, my_configuration->tank, my_configuration->player_id, my_configuration->player_ids);
	send_info_player_disconnected(my_configuration->player_id, my_configuration->player_ids);
	printf("Exiting the %d player connection handler thread!\n", my_configuration->player_id);

	return NULL;
}

//Checks the worlds' physics
int calculate_physics(struct whole_world* my_configuration, int player_id){
	struct singly_linked_node* iterator = global_receivings[player_id];
	struct singly_linked_node* free_help = NULL;
	global_receivings[player_id] = NULL;
	int return_value = 0;
	while (iterator!=NULL){
		struct information* data = (struct information*)(iterator->data);
		//Create
		if (data->action == CREATE){
			if(data->type_of == PROJECTILE){
				//Create projectile
			}
			else if (data->type_of == TANK){
				//Create tank - this never happens as tanks are created when client connects
			}
			else{
				printf("###ERROR: Unknown target of command CREATE!\n");
			}
		}
		else if (data->action == UPDATE){
			if (data->type_of == TANK){
				//Update tank
				tank_set_values(my_configuration->tanks[player_id], data->player_id, data->x_location, data->y_location, data->tank_angle, data->hp, data->turret_angle, DEFAULT_TANK_SKIN);
			}
			else if (data->type_of == PROJECTILE){
				//Update projectile - projectile ID required?
			}
			else{
				printf("###ERROR: Unknown target of command UPDATE!\n");
			}
		}	
		else if (data->action == DISCONNECT){
			//Sets the value and the player_connection_handler will do the rest :)
			return_value = DISCONNECTED;
		}
		else{
			printf("###ERROR: Unknown command received!\n");
		}
		free_help = iterator;
		iterator = iterator->next;
		information_free(free_help->data);
		free(free_help);
	}
	//Other calculations?
	return return_value;
}
/*
TO DO's:
a) check if client is still alive,
b) improve receiver,
c) implement sender, - check if update happened?
*/
