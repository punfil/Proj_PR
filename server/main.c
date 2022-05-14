#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

//Multithreading
#include <pthread.h>
#include <semaphore.h>

//Networking
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>


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

struct singly_linked_node* global_receivings[MAX_PLAYERS];
struct singly_linked_node* global_sendings[MAX_PLAYERS];

void initialize_mutexes();
void destroy_mutexes();

int create_socket(int port);
void close_socket(int sock);

int send_payload(int sock, void* msg, uint32_t msgsize);
struct information* receive_single_information(int* sock);

void clean_up_after_disconnect(int* csocket, struct sockaddr_in* client, struct tank* tank, int player_id, int* players_ids);

void* connection_handler(void* arg);
void* player_connection_handler(void* arg);

void* sender(int* csock, struct tank* tank, struct singly_linked_node* projectiles);
struct singly_linked_node* receiver(int* csock);

void* calculate_physics(void* arg);

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

//Initializes all mutexes to NULL
void initialize_mutexes(){
	for (int i=0;i<MAX_PLAYERS;i++){
		pthread_mutex_init(&(all_mutexes[i]), NULL);
	}
	pthread_mutex_init(&players_count_mutex, NULL);
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
	printf("##DEBUG Received information size %d action=%c type_of = %c\n", nread, received->action, received->type_of);
	memcpy(returning, received, sizeof(struct information*));
	return returning;
}

//Frees memory when the client disconnects
void clean_up_after_disconnect(int* csocket, struct sockaddr_in* client, struct tank* tank, int player_id, int* players_ids){
	free(csocket);
	free(client);
	free(tank);
	set_id_available(players_ids, player_id);
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

	pthread_t physics_calculator_thread;
	struct whole_world* world = whole_world_alloc();
	whole_world_set_values(world, player_ids, tanks_in_game, projectiles_in_game);
	int result = pthread_create(&physics_calculator_thread, NULL, calculate_physics, (void*)world);
	//check if it was succesful

	int customer_size = sizeof(struct sockaddr_in);
	int main_socket = create_socket(PORT);

	int temp_socket;
	int current_player_id;
	struct sockaddr_in temp_information;
	printf("Server started listening on port %d\nInput q to stop it!\n", PORT);
	struct timeval tv;
	tv.tv_sec = 0;
    tv.tv_usec = CLIENT_MOVE_WAIT;
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
		setsockopt(*(csockets[current_player_id]), SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));

		clients[current_player_id] = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
		if (clients[current_player_id] == NULL){
			printf("Error allocating memory!\n");
			pthread_mutex_unlock(&(all_mutexes[current_player_id]));
			return NULL;
		}

		tanks_in_game[current_player_id] = (struct tank*)malloc(sizeof(struct tank));
		if (tanks_in_game[current_player_id] == NULL){
			printf("Error allocating memory!\n");
			pthread_mutex_unlock(&(all_mutexes[current_player_id]));
			return NULL;
		}
		
		//sizeof(*) or sizeof()????? memcpy because accept takes pointer, so deepcopy required
		memcpy(clients[current_player_id], &temp_information, sizeof(struct sockaddr_in));

		pthread_mutex_unlock(&(all_mutexes[current_player_id]));
		for_thread_set_values(for_threads[current_player_id], current_player_id, tanks_in_game[current_player_id], projectiles_in_game, csockets[current_player_id], clients[current_player_id], &players_count);
		
		//Create new thread to serve this client
		int result = pthread_create(&clients_threads[current_player_id], NULL, player_connection_handler, (void*)for_threads[current_player_id]);
		// if creating new_thread failed free memory and close connection
	}
	printf("Exiting the connection handler thread!\n");

	*world->running = false;
	pthread_join(physics_calculator_thread, NULL);
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
void* sender(int* csock, struct tank* tank, struct singly_linked_node* projectiles){
	struct information* information = (struct information*)malloc(sizeof(struct information));
 	if (information==NULL){
 		printf("Error allocating memory in sender!\n");
 		return NULL;
 	}
	int nwrite = 0;

	//Check if there are any global updates - global_sendings

	//Send tank
	information_set_values(information, UPDATE, TANK, tank->player_id, tank->x, tank->y, tank->tank_angle, tank->hp, tank->turret_angle);
	nwrite = send_payload(*csock, information, sizeof(struct information*));
	//Send projectiles
	//in range len(projectiles)
	return NULL;
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
		}
		singly_linked_list_add(&received_informations, received);
	}
	return received_informations;
}			

void* player_connection_handler(void* arg){
	struct for_thread* my_configuration = (struct for_thread*) arg;
	
	//Send configuration to the new client
	struct configuration* configuration_to_send = configuration_alloc();
	configuration_set_values(configuration_to_send, WINDOW_WIDTH, WINDOW_HEIGHT, BACKGROUND_SCALE, *(my_configuration->players_count), my_configuration->player_id, 400, 400, 0);

	send_payload(*(my_configuration->csocket), configuration_to_send, sizeof(struct configuration));
	printf("New client connected from %s\n", inet_ntoa(my_configuration->client->sin_addr));

	configuration_free(configuration_to_send);
	
	struct singly_linked_node* received_informations;

	while (*(my_configuration->running)){
		//Receive the information available
		//Remember to clean every information + list element
		pthread_mutex_lock(&all_mutexes[my_configuration->player_id]);
		global_receivings[my_configuration->player_id] = receiver(my_configuration->csocket);
		sender(my_configuration->csocket, my_configuration->tank, my_configuration->projectiles);
		pthread_mutex_unlock(&all_mutexes[my_configuration->player_id]);
	}
	printf("Exiting the %d player connection handler thread!\n", my_configuration->player_id);
	return 0;
}

void* calculate_physics(void* arg){
	printf("Here I will be calculating physics. For now I am only rewriting what I've got :)\n");
	struct whole_world* my_configuration = (struct whole_world*)arg;
	while (*my_configuration->running == true){

	}
	printf("Exiting the physics calculator thread!\n");
	return NULL;
}
/*
TO DO's:
a) check if client is still alive,
b) improve receiver,
c) implement sender, - check if update happened?
*/
