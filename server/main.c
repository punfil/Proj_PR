#define MIN(a,b) ((a) < (b) ? (a) : (b))

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

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

#ifndef CLIENT_PREFERENCES_H
#include "client_preferences.h"
#endif

//Global variables
pthread_mutex_t players_mutexes[MAX_PLAYERS];
pthread_mutex_t players_count_mutex;

struct singly_linked_node* global_receivings[MAX_PLAYERS];
struct singly_linked_node* global_sendings[MAX_PLAYERS];

void initialize_global_arrays();

void initialize_mutexes();
void destroy_mutexes();

int create_socket(int port);
void close_socket(int sock);

int send_payload(int sock, void* msg, uint32_t msgsize);
struct information* receive_single_information(int* sock);
struct client_preferences* receive_single_client_preference(int* sock);

void send_info_new_player_connected(int player_id, struct tank* tank, int* player_ids);
void send_info_player_disconnected(int player_id, int* player_ids);
void delete_all_projectiles_player_disconnected(int player_id, struct projectile** all_projectiles, int* player_ids);

void send_info_new_projectile(int player_id, struct projectile* projectile, int* player_ids);
void send_info_projectile_delete(struct projectile* projectile, int* player_ids, bool send_to_owner);

void send_info_player_death(int* csocket, int player_id);
void clean_up_after_disconnect(int* csocket, struct sockaddr_in* client, struct tank* tank, int player_id, int* players_ids);

void* connection_handler(void* arg);
void* player_connection_handler(void* arg);

void sender(int* csock, int player_id, struct tank** tanks_in_game, struct projectile** projectiles, int* player_ids);
struct singly_linked_node* receiver(int* csock);

int calculate_physics(struct whole_world* my_configuration, int player_id);
int check_tank_collision_with_projectile(struct tank* tank, struct projectile* projectile);

// Starts the thread that accepts clients and allows to kill the server
int main(int argc, char* argv[]){
	int map_number = DEFAULT_MAP_NUMBER;
	if (argc == 1) {
		printf("##ERROR: No map number chosen - using default - %i\n", DEFAULT_MAP_NUMBER);
	} 
	else if (argc == 2) {
		char *p;

		long conv = strtol(argv[1], &p, 10);

		if (*p != '\0' || conv < 0) {
			printf("##ERROR: Incorrect map number - using default - %i\n", DEFAULT_MAP_NUMBER);
		} 
		else {
			map_number = conv;
			printf("###INFO: Chosen map number - %d\n", map_number);
		}
	}

	pthread_t main_thread;
	struct for_connection_handler_thread* main_thread_config = for_connection_handler_thread_alloc();
	for_connection_handler_thread_set_values(main_thread_config, map_number);
	int result = pthread_create(&main_thread, NULL, connection_handler, (void*) main_thread_config);
	if (result!=0){
			printf("##ERROR: Failed to create thread\n");
	}
	while (true){
		char x;
		scanf(" %c", &x);
		if (x == SERVER_EXIT_BUTTON){
			break;
		}
	}
	printf("###INFO: Exiting the main thread!\n");
	*main_thread_config->running = false;
	pthread_join(main_thread, NULL);
	return 0;
}

//Used to set all the global variables to NULL
void initialize_global_arrays(){
	for (int i=0;i<MAX_PLAYERS;i++){
		pthread_mutex_lock(&players_mutexes[i]);
		global_receivings[i] = NULL;
		global_sendings[i] = NULL;
		pthread_mutex_unlock(&players_mutexes[i]);
	}
}

//Initializes all mutexes to NULL
void initialize_mutexes(){
	for (int i=0;i<MAX_PLAYERS;i++){
		pthread_mutex_init(&(players_mutexes[i]), NULL);
	}
	pthread_mutex_init(&players_count_mutex, NULL);
}

//Destroy the mutexes and free the resources
void destroy_mutexes(){
	for (int i=0;i<MAX_PLAYERS;i++){
		pthread_mutex_destroy(&(players_mutexes[i]));
	}
	pthread_mutex_destroy(&players_count_mutex);
}

//Creates the socket to listen
int create_socket(int port){
	int sock, err;
	struct sockaddr_in server;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("##ERROR: Socket open failed;\n");
		exit(1); //Connection failed
	}
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
	bzero((char*) &server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *)&server , sizeof(server)) < 0){
        printf("##ERROR: Socket binding failed");
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

//Allows to receive one information struct. Memory allocation happens here!
struct information* receive_single_information(int *sock){
	char buff[RECEIVER_BUFFER_SIZE];
	bzero(buff, RECEIVER_BUFFER_SIZE);
	int available;
	int nread=read(*sock, buff, RECEIVER_BUFFER_SIZE);
	if (nread <=0){
		return NULL; //Didn't receive any information
	}
	struct information* returning = information_alloc();
	if (returning == NULL){
		printf("##ERROR: Error allocating memory receiving information\n");
	}
	struct information* received = (struct information*) buff;
	memcpy(returning, received, sizeof(struct information));
	return returning;
}

//Alows to receive one client preference struct. Memory allocation happends here!
struct client_preferences* receive_single_client_preference(int* sock){
	char buff[RECEIVER_BUFFER_SIZE];
	bzero(buff, RECEIVER_BUFFER_SIZE);
	int available;
	int nread=read(*sock, buff, RECEIVER_BUFFER_SIZE);
	if (nread <=0){
		return NULL; //Didn't receive any information
	}
	struct client_preferences* returning = client_preferences_alloc();
	if (returning == NULL){
		printf("##ERROR: Error allocating memory receiving information\n");
	}
	struct client_preferences* received = (struct client_preferences*) buff;
	memcpy(returning, received, sizeof(struct client_preferences));
	return returning;
}

//Sends info to all connected players that new player has connected
void send_info_new_player_connected(int player_id, struct tank* tank, int* player_ids){
	for (int i=0;i<MAX_PLAYERS;i++){
		if (player_ids[i] == USED_ID && player_id != i){ //Send to all connected players but not to the one connecting!
			struct information* sending = information_alloc();
			information_set_values(sending, CREATE, TANK, player_id, TANK_SPAWN_POINT_X, TANK_SPAWN_POINT_Y, DEFAULT_TANK_ANGLE, FULL_HP, DEFAULT_TANK_TURRET_ANGLE);
			pthread_mutex_lock(&players_mutexes[i]);
			singly_linked_list_add(&global_sendings[i], sending);
			pthread_mutex_unlock(&players_mutexes[i]);
		}
	}
}

//Sends info to all connected players that new player has disconnected
void send_info_player_disconnected(int player_id, int* player_ids){
	for (int i=0;i<MAX_PLAYERS;i++){
		if (player_ids[i] == USED_ID && player_id != i){ //Send to all connected players but not to the one disconnecting!
			struct information* sending = information_alloc();
			information_set_values(sending, DISCONNECT, TANK, player_id, TANK_SPAWN_POINT_X, TANK_SPAWN_POINT_Y, DEFAULT_TANK_ANGLE, EMPTY_HP, DEFAULT_TANK_TURRET_ANGLE);
			pthread_mutex_lock(&players_mutexes[i]);
			singly_linked_list_add(&global_sendings[i], sending);
			pthread_mutex_unlock(&players_mutexes[i]);
		}
	}
}

//Delete all projectiles that belong to player that disconnects. Projectile values are calculated on the client side so all of them would be zombie projectiles.
void delete_all_projectiles_player_disconnected(int player_id, struct projectile** all_projectiles, int* player_ids){
	for (int i=player_id*MAX_PROJECTILES_PER_PLAYER;i<(player_id+1)*MAX_PROJECTILES_PER_PLAYER;i++){
		if (all_projectiles[i]!= NULL){
			send_info_projectile_delete(all_projectiles[i], player_ids, false);
			pthread_mutex_lock(&players_mutexes[player_id]);
			projectile_free(all_projectiles[i]);
			all_projectiles[i] = NULL;
			pthread_mutex_unlock(&players_mutexes[player_id]);
		}
	}
}

//Sends info to all connected players that a projectile has been spawned
void send_info_new_projectile(int player_id, struct projectile* projectile, int* player_ids){
	for (int i=0;i<MAX_PLAYERS;i++){
		if (player_ids[i] == USED_ID && player_id != i){ //Send to all connected players but not to the one connecting!
			struct information* sending = information_alloc();
			information_set_values(sending, CREATE, PROJECTILE, player_id, projectile->x, projectile->y, projectile->angle, (float)PROJECTILE_EXISTS, (float)projectile->id); // default angle, HP, turret_angle
			pthread_mutex_lock(&players_mutexes[i]);
			singly_linked_list_add(&global_sendings[i], sending);
			pthread_mutex_unlock(&players_mutexes[i]);
		}
	}
}

//Sends info to all connected players that a projectile has to die
void send_info_projectile_delete(struct projectile* projectile, int* player_ids, bool send_to_owner){
	for (int i=0;i<MAX_PLAYERS;i++){
		if (player_ids[i] == USED_ID && ((send_to_owner == true) || (send_to_owner == false && i != projectile->owner_id))){ 
			struct information* sending = information_alloc();
			information_set_values(sending, UPDATE, PROJECTILE, projectile->owner_id, projectile->x, projectile->y, (float)POSITION_NOT_REQUIRED, (float)PROJECTILE_NOT_EXISTS, (float)projectile->id);
			pthread_mutex_lock(&players_mutexes[i]);
			singly_linked_list_add(&global_sendings[i], sending);
			pthread_mutex_unlock(&players_mutexes[i]);
		}
	}
}

//Send info to player that he died
void send_info_player_death(int* csocket, int player_id){
	struct information* information = information_alloc();
	information_set_values(information, DIE, TANK, player_id, POSITION_NOT_REQUIRED, POSITION_NOT_REQUIRED, DEFAULT_TANK_ANGLE, EMPTY_HP, DEFAULT_TANK_TURRET_ANGLE);
	int nwrite = send_payload(*csocket, information, sizeof(struct information));
	information_free(information);
}

//Frees memory when the client disconnects
void clean_up_after_disconnect(int* csocket, struct sockaddr_in* client, struct tank* tank, int player_id, int* players_ids){
	pthread_mutex_lock(&players_count_mutex);
	set_id_available(players_ids, player_id);
	pthread_mutex_unlock(&players_count_mutex);
	free(csocket);
	free(client);
	free(tank);
}

//Accepts connections and creates new threads - one for each client
void* connection_handler(void* arg){
	struct for_connection_handler_thread* my_config = (struct for_connection_handler_thread*) arg;

	initialize_mutexes();
	initialize_global_arrays();

	//Variables for connecting and player recognition
	int players_count = 0;

	int* player_ids = generate_id_list(MAX_PLAYERS);
	if (player_ids == NULL){
		printf("##ERROR: Error allocating memory. Bye!\n");
		return NULL;
	}
	
	//Variables for tanks or projectiles
	struct tank** tanks_in_game = (struct tank**)malloc(MAX_PLAYERS*(sizeof(struct tank*)));
	if (tanks_in_game == NULL){
		printf("##ERROR: Error allocating memory. Bye!\n");
		return NULL;
	}

	struct projectile** projectiles_in_game = (struct projectile**)malloc(MAX_PROJECTILES_PER_PLAYER*MAX_PLAYERS*(sizeof(struct projectile*)));
	if (tanks_in_game == NULL){
		printf("##ERROR: Error allocating memory. Bye!\n");
		return NULL;
	}
	for (int i=0;i<MAX_PROJECTILES_PER_PLAYER*MAX_PLAYERS;i++){
		projectiles_in_game[i] = NULL;
	}

	//Variables for networking
	int** csockets = (int**)malloc(MAX_PLAYERS*sizeof(int*));
	if (csockets == NULL){
		printf("##ERROR: Error allocating memory. Bye!\n");
		return NULL;
	}

	struct sockaddr_in** clients = (struct sockaddr_in**)malloc(MAX_PLAYERS*sizeof(struct sockaddr_in*));
	if (clients == NULL){
		printf("##ERROR: Error allocating memory. Bye!\n");
		return NULL;
	}

	struct for_thread** for_threads = (struct for_thread**)malloc(MAX_PLAYERS*sizeof(struct for_thread*));
	if (for_threads == NULL){
		printf("##ERROR: Error allocating memory. Bye!\n");
		return NULL;
	}
	for (int i=0;i<MAX_PLAYERS;i++){
		for_threads[i] = for_thread_alloc();
	}

	pthread_t clients_threads[MAX_PLAYERS];

	struct whole_world* world = whole_world_alloc();
	whole_world_set_values(world, player_ids, tanks_in_game, projectiles_in_game, my_config->map_number);

	int customer_size = sizeof(struct sockaddr_in);
	int main_socket = create_socket(PORT);

	int temp_socket;
	int current_player_id;
	struct sockaddr_in temp_information;
	printf("###INFO: Server started listening on port %d\nInput q to stop it!\n", PORT);
	struct timeval tv;
	tv.tv_sec = CLIENT_MOVE_WAIT_SEC;
    tv.tv_usec = CLIENT_MOVE_WAIT_USEC;
	int one = 1;
	while (*my_config->running){
		temp_socket = accept(main_socket, (struct sockaddr*) &temp_information, &customer_size);

		//Check if connection succeeded
		if (temp_socket < 0){
			continue;
        }

		//Prepare ID for the connection. If no is available then disconnect
		pthread_mutex_lock(&players_count_mutex);
		current_player_id = return_free_id(player_ids);
		pthread_mutex_unlock(&players_count_mutex);
		
		if (current_player_id == USED_ID){
			printf("##ERROR: Currently server is full of players. Try again later\n");
			close_socket(temp_socket);
			continue;
		}
		
		clients[current_player_id] = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
		if (clients[current_player_id] == NULL){
			printf("##ERROR: Error allocating memory!\n");
			return NULL;
		}

		tanks_in_game[current_player_id] = tank_alloc();
		if (tanks_in_game[current_player_id] == NULL){
			printf("##ERROR: Error allocating memory!\n");
			return NULL;
		}

		csockets[current_player_id] = (int*)malloc(sizeof(int));
		if (csockets[current_player_id] == NULL){
			printf("##ERROR: Error allocating memory!\n");
			return NULL;
		}
		

		pthread_mutex_lock(&players_count_mutex);
		increment_players_count(&players_count);
		pthread_mutex_unlock(&players_count_mutex);

		pthread_mutex_lock(&(players_mutexes[current_player_id]));

		*csockets[current_player_id] = temp_socket;
		
		//Receive client options and preferences
		struct client_preferences* options = receive_single_client_preference(csockets[current_player_id]);
		tank_set_values(tanks_in_game[current_player_id], current_player_id, TANK_SPAWN_POINT_X, TANK_SPAWN_POINT_Y, DEFAULT_TANK_ANGLE, options->tank_max_hp, DEFAULT_TANK_TURRET_ANGLE, options->tank_version);
		printf("##DEBUG: Received tank skin %d and max HP %f\n", options->tank_max_hp, options->tank_version);
		client_preferences_free(options);
		
		memcpy(clients[current_player_id], &temp_information, sizeof(struct sockaddr_in));
		
		pthread_mutex_unlock(&(players_mutexes[current_player_id]));
		
		setsockopt(*(csockets[current_player_id]), SOL_TCP, TCP_NODELAY, &one, sizeof(one));
		setsockopt(*(csockets[current_player_id]), SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));	

		for_thread_set_values(for_threads[current_player_id], current_player_id, tanks_in_game[current_player_id], projectiles_in_game, csockets[current_player_id], clients[current_player_id], &players_count, world, player_ids);
		
		//Send information to all existing clients that new player has joined
		send_info_new_player_connected(current_player_id, tanks_in_game[current_player_id], player_ids);

		//Create new thread to serve this client
		int result = pthread_create(&clients_threads[current_player_id], NULL, player_connection_handler, (void*)for_threads[current_player_id]);
		pthread_detach(clients_threads[current_player_id]); //Automatically free resources after thread when player disconnects
	}
	printf("###INFO: Exiting the connection handler thread!\n");

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

	destroy_mutexes();

	close_socket(main_socket);

	for_connection_handler_thread_free(my_config);
	
	return NULL;	
}

//Sends information about everything :)
void sender(int* csock, int player_id, struct tank** tanks_in_game, struct projectile** projectiles, int* player_ids){
	struct information* information = information_alloc();
 	if (information==NULL){
 		printf("##ERROR: Error allocating memory in sender!\n");
 		return;
 	}
	int nwrite = 0;
	
	//Send all tanks
	for (int i=0;i<MAX_PLAYERS;i++){
		if (player_ids[i] == USED_ID){
			information_set_values(information, UPDATE, TANK, tanks_in_game[i]->player_id, tanks_in_game[i]->x, tanks_in_game[i]->y, tanks_in_game[i]->tank_angle, tanks_in_game[i]->hp, tanks_in_game[i]->turret_angle);
			nwrite = send_payload(*csock, information, sizeof(struct information));		
		}
	}

	//Check if there are any global updates - global_sendings
	pthread_mutex_lock(&players_mutexes[player_id]);
	struct singly_linked_node* iterator = global_sendings[player_id];
	global_sendings[player_id] = NULL;
	pthread_mutex_unlock(&players_mutexes[player_id]);
	struct singly_linked_node* free_helper = NULL;
	while (iterator != NULL){
		nwrite = send_payload(*csock, (void *)iterator->data, sizeof(struct information));
		free_helper = iterator;
		iterator = iterator->next;
		information_free((struct information*)free_helper->data);
		free(free_helper);
	}

	//Send all projectiles
	for (int i=0;i<MAX_PROJECTILES_PER_PLAYER*MAX_PLAYERS;i++){
		if (projectiles[i]!=NULL){
			pthread_mutex_lock(&players_mutexes[i/MAX_PROJECTILES_PER_PLAYER]);
			information_set_values(information, UPDATE, PROJECTILE, projectiles[i]->owner_id, projectiles[i]->x, projectiles[i]->y, projectiles[i]->angle, (float)PROJECTILE_EXISTS, (float)projectiles[i]->id);
			pthread_mutex_unlock(&players_mutexes[i/MAX_PROJECTILES_PER_PLAYER]);
			nwrite = send_payload(*csock, information, sizeof(struct information));
		}
	}
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
	configuration_set_values(configuration_to_send, WINDOW_WIDTH, WINDOW_HEIGHT, BACKGROUND_SCALE, *(my_configuration->players_count), my_configuration->player_id, TANK_SPAWN_POINT_X, TANK_SPAWN_POINT_Y, my_configuration->whole_world->map_number);

	tank_set_values(my_configuration->tank, my_configuration->player_id, TANK_SPAWN_POINT_X, TANK_SPAWN_POINT_Y, NO_ROTATION, FULL_HP, NO_ROTATION, DEFAULT_TANK_SKIN);

	send_payload(*(my_configuration->csocket), configuration_to_send, sizeof(struct configuration));
	printf("###INFO: New client player ID:%d connected from %s\n", my_configuration->player_id, inet_ntoa(my_configuration->client->sin_addr));

	configuration_free(configuration_to_send);
	int waits_since_last_update = 0;
	int player_state = OK;
	while (*(my_configuration->running) && player_state == OK){
		//Receive the information available
		//Remember to clean every information + list element
		//No need to use mutex - only this player can access global_receiving[player_id]
		global_receivings[my_configuration->player_id] = receiver(my_configuration->csocket);
		//If received nothing then repeat
		if (global_receivings[my_configuration->player_id] == NULL){
			waits_since_last_update++;
			if (waits_since_last_update>CLIENT_NO_RESPONSE_ITERATION){ //Servers waits for CLIENT_NO_RESPONSE_ITERATION*(CLIENT_MOVE_WAIT_USEC+CLIENT_MOVE_WAIT_SEC)
				player_state = CONNECTION_LOST;
				*(my_configuration->running) = false;
			}
			continue;
		}
		waits_since_last_update = 0;
		player_state = calculate_physics(my_configuration->whole_world, my_configuration->player_id);
		if (player_state == DISCONNECTED || player_state == DEAD){
			//Player sent info that he wants to disconnect or died.
			*(my_configuration->running) = false;
			break;
		}
		sender(my_configuration->csocket, my_configuration->player_id, my_configuration->whole_world->tanks, my_configuration->whole_world->projectiles, my_configuration->whole_world->player_ids);
	}
	if (player_state == DISCONNECTED){
		printf("###INFO: Client player ID:%d connected from %s disconnected\n", my_configuration->player_id, inet_ntoa(my_configuration->client->sin_addr));
	}
	else if (player_state == DEAD){
		printf("###INFO: Client player ID:%d connected from %s has been reported dead and has been disconnected\n", my_configuration->player_id, inet_ntoa(my_configuration->client->sin_addr));
		send_info_player_death(my_configuration->csocket, my_configuration->player_id);
	}
	else if(player_state==CONNECTION_LOST){
		printf("###ERROR: Client player ID: %d connected from %s has lost connection with server\n", my_configuration->player_id, inet_ntoa(my_configuration->client->sin_addr));
	}
	pthread_mutex_lock(&players_count_mutex);
	decrement_players_count(my_configuration->players_count);
	pthread_mutex_unlock(&players_count_mutex);
	delete_all_projectiles_player_disconnected(my_configuration->player_id, my_configuration->whole_world->projectiles, my_configuration->player_ids);
	send_info_player_disconnected(my_configuration->player_id, my_configuration->player_ids);
	clean_up_after_disconnect(my_configuration->csocket, my_configuration->client, my_configuration->tank, my_configuration->player_id, my_configuration->player_ids);
	pthread_mutex_lock(&players_mutexes[my_configuration->player_id]);
	singly_linked_list_clear(&global_receivings[my_configuration->player_id]);
	singly_linked_list_clear(&global_sendings[my_configuration->player_id]);
	pthread_mutex_unlock(&players_mutexes[my_configuration->player_id]);
	return NULL;
}

//Checks the world's physics
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
				struct projectile* projectile = projectile_alloc();
				projectile_set_values(projectile, (int)data->turret_angle, data->player_id, data->x_location, data->y_location, data->tank_angle, data->hp);
				pthread_mutex_lock(&players_mutexes[data->player_id]);
				my_configuration->projectiles[(int)data->turret_angle] = projectile;
				pthread_mutex_unlock(&players_mutexes[data->player_id]);
				send_info_new_projectile(data->player_id, projectile, my_configuration->player_ids);
			}
			//Create tank - this never happens as tanks are created when client connects
			else{
				printf("##ERROR: Unknown target of command CREATE!\n");
			}
		}
		else if (data->action == UPDATE){
			if (data->type_of == TANK){
				//Update tank
				pthread_mutex_lock(&players_mutexes[player_id]);
				tank_set_values(my_configuration->tanks[player_id], data->player_id, data->x_location, data->y_location, data->tank_angle, MIN(data->hp, my_configuration->tanks[player_id]->hp), data->turret_angle, DEFAULT_TANK_SKIN);
				pthread_mutex_unlock(&players_mutexes[player_id]);
				if (data->hp<=EMPTY_HP){
					return_value = DEAD; //I am dead. Disconnect the player. Player connection handler will do the rest.
				}
			}
			else if (data->type_of == PROJECTILE){
				if (data->hp == PROJECTILE_NOT_EXISTS){ //Received that projectile should be removed from the board
					struct projectile* this_projectile = get_projectile_with_id(my_configuration->projectiles, (int)data->turret_angle);
					send_info_projectile_delete(this_projectile, my_configuration->player_ids, true);
					pthread_mutex_lock(&players_mutexes[player_id]);
					remove_projectile_from_list(my_configuration->projectiles, (int)data->turret_angle);
					pthread_mutex_unlock(&players_mutexes[player_id]);
				}
				else{ //Projectile is still alive
					update_projectile_values(my_configuration->projectiles, (int)data->turret_angle, data->x_location, data->y_location);
					//Check collision of this projectile with other tanks
					struct projectile* this_projectile = get_projectile_with_id(my_configuration->projectiles, (int)data->turret_angle);
					bool exit = false;
					for (int i=0;i<MAX_PLAYERS && exit == false;i++){
						if (this_projectile == NULL){
							exit = true;
							break;
						}
						if (my_configuration->player_ids[i] == USED_ID){
							int this_projectile_collision = check_tank_collision_with_projectile(my_configuration->tanks[i], this_projectile);
							if (this_projectile_collision == DEAD){
								int this_projectile_owner_id = this_projectile->owner_id;
								pthread_mutex_lock(&players_mutexes[i]);
								my_configuration->tanks[i]->hp -= TANK_PROJECTILE_COLLISION_DAMAGE;
								pthread_mutex_unlock(&players_mutexes[i]);

								send_info_projectile_delete(this_projectile, my_configuration->player_ids, true);

								pthread_mutex_lock(&players_mutexes[i]);
								remove_projectile_from_list(my_configuration->projectiles, this_projectile->id); //This is bad, MUST be replaced!
								pthread_mutex_unlock(&players_mutexes[i]);
								exit = true;
							}
						}
					}
				}
			}
			else{
				printf("##ERROR: Unknown target of command UPDATE!\n");
			}
		}	
		else if (data->action == DISCONNECT){
			//Sets the value and the player_connection_handler will do the rest :)
			return_value = DISCONNECTED;
		}
		else{
			printf("##ERROR: Unknown command received!\n");
		}
		free_help = iterator;
		iterator = iterator->next;
		information_free(free_help->data);
		free(free_help);
	}
	return return_value;
}

//Checks if projectile collides with tank. If so returns DEAD (projectile) else OK
int check_tank_collision_with_projectile(struct tank* tank, struct projectile* projectile){
	//We treat tank collision surface as an circle
	if (tank->player_id != projectile->owner_id && ((projectile->x - tank->x)*(projectile->x - tank->x) + (projectile->y - tank->y)*(projectile->y - tank->y) < (TANK_COLLISION_R*TANK_COLLISION_R))){
		return DEAD;
	}
	else{
		return OK;
	}
}
