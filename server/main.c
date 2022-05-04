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

pthread_mutex_t all_mutexes[MAX_PLAYERS];
void initialize_mutexes();

int create_socket(int port);
void close_socket(int sock);

int send_payload(int sock, void* msg, uint32_t msgsize);
struct information* receive_single_information(int* sock);

void clean_up_after_disconnect(int* csocket, struct sockaddr_in* client, struct tank* tank, int player_id, int* players_ids);

void* connection_handler(void* arg);

void* sender(void* arg);
void* receiver(void* arg);

int main() {
	pthread_t main_thread;
	bool main_thread_running = true;
	int result = pthread_create(&main_thread, NULL, connection_handler, (void*) &main_thread_running);
	pthread_detach(main_thread); //This shouldn't be here, it doesn't free resources.
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
	main_thread_running = false;
	pthread_join(main_thread, NULL);
	return 0;
}

void initialize_mutexes(){
	for (int i=0;i<MAX_PLAYERS;i++){
		pthread_mutex_init(&(all_mutexes[i]), NULL);
	}
}

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
    return sock;
}

void close_socket(int sock){
	close(sock);
}

int send_payload(int sock, void* msg, uint32_t msgsize){
	int nwrite = write(sock, msg, msgsize);
	return nwrite;
}

//Remember to use free!!!!!
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

void clean_up_after_disconnect(int* csocket, struct sockaddr_in* client, struct tank* tank, int player_id, int* players_ids){
	free(csocket);
	free(client);
	free(tank);
	set_id_available(players_ids, player_id);
}

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

	struct for_thread* for_threads = for_thread_alloc();
	if (for_threads == NULL){
		printf("Error allocating memory. Bye!\n");
		return NULL;
	}
	for_thread_set_values(for_threads, player_ids, tanks_in_game, projectiles_in_game, csockets, clients, &players_count);

	int customer_size = sizeof(struct sockaddr_in);
	int main_socket = create_socket(PORT);

	struct configuration* configuration_to_send = configuration_alloc();
	configuration_set_values(configuration_to_send, WINDOW_WIDTH, WINDOW_HEIGHT, BACKGROUND_SCALE, 0, 0, 400, 400, 0);

	//Configure sender process
	pthread_t sender_thread;
	int result = pthread_create(&sender_thread, NULL, sender, for_threads); //Change arguments!!!!
	if (result!=0){
			printf("Failed to create sender thread!");
	}
	pthread_detach(sender_thread);

	//Configure receiver process
	pthread_t receiver_thread;
	result = pthread_create(&receiver_thread, NULL, receiver, for_threads); //Change arguments!!!!
	if (result!=0){
			printf("Failed to create receiver thread!");
	}
	pthread_detach(receiver_thread);

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
            printf("Error: accept() failed\n");
			continue;
        }

		printf("New client connected from %s\n", inet_ntoa(temp_information.sin_addr));

		//Prepare ID for the connection. If no is available then disconnect (!!!IN THE FUTURE: SEND SERVER IS FULL!!!)
		current_player_id = return_free_id(player_ids);
		if (current_player_id == USED_ID){
			printf("Currently server is full of players. Try again later\n");
			close_socket(temp_socket);
			continue;
		}
		//MAIN SEMAPHORE WAIT
		pthread_mutex_lock(&(all_mutexes[current_player_id]));
		increment_players_count(&players_count);
		configuration_update_values(configuration_to_send, current_player_id, players_count);

		//Send configuration to the new client
		send_payload(temp_socket, configuration_to_send, sizeof(struct configuration));

		//This might be a futher function
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
		//MAIN SEMAPHORE POST
	}
	//Disconnect all clients here!!! Remember to comment detach in main
	//pthread_join(receiver_thread, NULL);
	pthread_join(sender_thread, NULL);

	free(player_ids);
	free(tanks_in_game);
	free(projectiles_in_game);
	free(csockets);
	free(for_threads);
	free(clients);
	free(configuration_to_send);

	close_socket(main_socket);
	
	return NULL;	
}

void* sender(void* arg){
	struct for_thread* my_configuration = (struct for_thread*) arg;
	struct information* information = (struct information*)malloc(sizeof(struct information));
	int nwrite = 0;
	if (information==NULL){
		printf("Error allocating memory in sender!\n");
		return NULL;
	}
	while (my_configuration->running){
		for (int i=0;i<MAX_PLAYERS;i++){
			if (my_configuration->player_ids[i] == USED_ID){
				pthread_mutex_lock(&(all_mutexes[i]));
				information->action = UPDATE;
				information->player_id = i;
				information->type_of = TANK;
				information->x_location = my_configuration->tanks_in_game[i]->x;
				information->y_location = my_configuration->tanks_in_game[i]->y;
				information->tank_angle = my_configuration->tanks_in_game[i]->tank_angle;
				information->hp = my_configuration->tanks_in_game[i]->hp;
				information->turret_angle = my_configuration->tanks_in_game[i]->turret_angle;
				nwrite = send_payload(*(my_configuration->csockets[i]), (void*)information, sizeof(struct information));
				printf("##DEBUG Sent information size %d bytes to player %d action=%c type_of=%c\n", nwrite, i, information->action, information->type_of);
				pthread_mutex_unlock(&(all_mutexes[i]));
			}
		}
	}
	printf("Exiting sender\n");
	free(information);
	return NULL;
}

void* receiver(void* arg){
	struct for_thread* my_configuration = (struct for_thread*) arg;
	while (my_configuration->running){
		for (int i=0;i<MAX_PLAYERS;i++){
			if (my_configuration->player_ids[i] == USED_ID){
				//Check connection alive. If not free the memory.
				//  if (){
				//  	clean_up_after_disconnect(my_configuration->csockets[i], my_configuration->clients[i], my_configuration->tanks_in_game[i], i, my_configuration->player_ids);
				// }

				//Receive information. If the function returned NULL -> allocation error or no information to be received.
				pthread_mutex_lock(&(all_mutexes[i]));
				struct information* received = receive_single_information(my_configuration->csockets[i]);
				if (received == NULL){ //No information has been received
					pthread_mutex_unlock(&(all_mutexes[i]));
					continue;
				}
				if (received->action == CREATE){
					//Create a projectile in the system. When projectile crashes or it's lifetime ends call free()!
					struct projectile* new_projectile = projectile_alloc();
					if (new_projectile == NULL){
						printf("Error allocating memory...");
						continue;
					}
					projectile_set_values(new_projectile, i, received->x_location, received->y_location, received->tank_angle, received->hp);
					
					//Semaphore WAIT!
					singly_linked_list_add(my_configuration->projectiles_in_game, new_projectile);
					//Semaphore POST
				}
				else if (received->action == UPDATE){
					//Update tank position
					//Semaphore WAIT
					tank_update(my_configuration->tanks_in_game[i], received->x_location, received->y_location, received->tank_angle, received->hp, received->turret_angle);
					//Semaphore POST
				}
				else if (received->action == DISCONNECT){
					clean_up_after_disconnect(my_configuration->csockets[i], my_configuration->clients[i], my_configuration->tanks_in_game[i], i, my_configuration->player_ids);
					decrement_players_count(my_configuration->players_count);
					printf("Player %d disconnected!\n", i);
					//Send to other players that the player has disconnected!
				}
				else{
					printf("Received wrong command!:%c\n", received->action);
				}
				free(received);
				pthread_mutex_unlock(&(all_mutexes[i]));
			}
		}
	}
	printf("Exiting receiver\n");
	return NULL;
}

/*
TO DO's:
a) check if client is still alive,
b) improve receiver,
c) implement sender, - check if update happened?
*/
