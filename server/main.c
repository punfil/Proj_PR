#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

//Networking
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

//Multithreading
#include <pthread.h>
#include <semaphore.h>


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

int create_socket(int port);
void close_socket(int sock);

bool send_payload(int sock, void* msg, uint32_t msgsize);

void* connection_handler(void* arg);

void* sender(void* arg);
void* receiver(void* arg);

int main() {
	pthread_t main_thread;
	int result = pthread_create(&main_thread, NULL, connection_handler, NULL);
	//pthread_detach(main_thread);
	if (result!=0){
			printf("Failed to create thread");
	}
	while (1){
		char x;
		scanf(" %c", &x);
		if (x == 'q'){
			break;
		}
	}
	pthread_join(main_thread, NULL);
	return 0;
	
	//Buffers for incoming data
	//int BUFFSIZE=512;
	//char buff[BUFFSIZE];
	//bzero(buff, BUFFSIZE);
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

bool send_payload(int sock, void* msg, uint32_t msgsize){
	if(write(sock, msg, msgsize)<0){ //write returns number of bytes sent
		return false;
	}
	return true;
}

void* connection_handler(void* arg){
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
	struct projectile** projectiles_in_game = (struct projectile**)malloc(MAX_PROJECTILES*(sizeof(struct projectile*)));
	if (projectiles_in_game == NULL){
		printf("Error allocating memory. Bye!\n");
		return NULL;
	}

	//Variables for networking
	int** csockets = (int**)malloc(MAX_PLAYERS*sizeof(int*));
	if (csockets == NULL){
		printf("Error allocating memory. Bye!\n");
		return NULL;
	}

	struct for_thread* for_threads = (struct for_thread*)malloc(sizeof(struct for_thread));
	if (for_threads == NULL){
		printf("Error allocating memory. Bye!\n");
		return NULL;
	}

	struct sockaddr_in** clients = (struct sockaddr_in**)malloc(MAX_PLAYERS*sizeof(struct sockaddr_in*));
	if (clients == NULL){
		printf("Error allocating memory. Bye!\n");
		return NULL;
	}

	int customer_size = sizeof(struct sockaddr_in);
	int main_socket = create_socket(PORT);

	printf("Server started listening on port %d\n", PORT);

	struct configuration* configuration_to_send = configuration_alloc();
	configuration_set_values(configuration_to_send, WINDOW_WIDTH, WINDOW_HEIGHT, BACKGROUND_SCALE, 0, 0, 0, 0, 0);

	//Configure sender process
	pthread_t sender_thread;
	int result = pthread_create(&sender_thread, NULL, sender, &for_threads); //Change arguments!!!!
	pthread_detach(sender_thread);
	if (result!=0){
			printf("Failed to create sender thread!");
	}
	//Configure receiver process
	pthread_t receiver_thread;
	result = pthread_create(&receiver_thread, NULL, receiver, &for_threads); //Change arguments!!!!
	pthread_detach(receiver_thread);
	if (result!=0){
			printf("Failed to create receiver thread!");
	}
	return NULL;

	int temp_socket;
	int current_player_id;
	struct sockaddr_in temp_information;
	while (1){
		temp_socket = accept(main_socket, (struct sockaddr*) &temp_information, &customer_size);
		
		//Check if connection succeeded
		if (temp_socket < 0){
            printf("Error: accept() failed\n");
			continue;
        }

		//Prepare ID for the connection. If no is available then disconnect (!!!IN THE FUTURE: SEND SERVER IS FULL!!!)
		current_player_id = return_free_id(player_ids);
		if (current_player_id == USED_ID){
			printf("Currently server is full of players. Try again later\n");
			close_socket(temp_socket);
			continue;
		}

		//MAIN SEMAPHORE WAIT
		increment_players_count(&players_count);
		configuration_update_values(configuration_to_send, current_player_id, players_count);

		//Send configuration to the new client
		send_payload(temp_socket, configuration_to_send, sizeof(struct configuration));

		//This might be a futher function
		csockets[current_player_id] = (int*)malloc(sizeof(int));
		//IF NULL
		*csockets[current_player_id] = temp_socket;

		clients[current_player_id] = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
		//IF NULL
		//sizeof(*) or sizeof()????? memcpy because accept takes pointer, so deepcopy required
		memcpy(clients[current_player_id], &temp_information, sizeof(struct sockaddr_in));
		//MAIN SEMAPHORE POST
	}

	free(player_ids);
	free(tanks_in_game);
	free(projectiles_in_game);
	free(csockets);
	free(for_threads);
	free(clients);
	free(configuration_to_send);

	close_socket(main_socket);
	pthread_join(receiver_thread, NULL);
	pthread_join(sender_thread, NULL);
	return NULL;	
}

void* sender(void* arg){
	//struct for_thread* my_configuration = (struct for_thread*) arg;
	// while (1){
	// 	for (int i=0;i<MAX_PLAYERS;i++){
	// 		if (my_configuration->player_ids[i] == USED_ID){
	// 			printf("Hello sender!");
	// 			///Do something
	// 		}
	// 	}
	// }
	return NULL;
}

void* receiver(void* arg){
	// struct for_thread* my_configuration = (struct for_thread*) arg;
	// while (1){
	// 	for (int i=0;i<MAX_PLAYERS;i++){
	// 		if (my_configuration->player_ids[i] == USED_ID){
	// 			printf("Hello receiver!");
	// 			///Do something
	// 		}
	// 	}
	// }
	// return 0;
}

/*
TO DO's:
a) notify server that the client disconnected,
b) debug and test the code - it has been written but not tested,
c) implement sender and receiver,
d) implement semaphores for critical sections,
*/
