#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

//Networking
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

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

//Global variables for all threads !!!!!!!MEANT TO BE NOT GLOBAL IN THE FUTURE!!!!!!!!
int player_count;
struct tank** tanks_in_game;
struct projectile** projectiles_in_game;

int create_socket(int port);
int close_socket(int sock);

bool send_payload(int sock, void* msg, uint32_t msgsize);

void* connection_handler(void* arg);

void* sender(void* arg);
void* receiver(void* arg);

void setup_globals();

int main() {
	setup_globals();
	pthread_t main_thread;
	int result = pthread_create(&main_thread, NULL, connection_handler, NULL);
	pthread_detach(main_thread);
	if (result!=0){
			printf("Failed to create thread");
	}
	while (1){
		char x;
		scanf("%c\n", &x)
		if (x == 'q'){
			break;
		}
	}
	pthread_join(main_thread);
	return 0;
	
	//Buffers for incoming data
	//int BUFFSIZE=512;
	//char buff[BUFFSIZE];
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
	//Variables for networking
	int csockets[MAX_PLAYERS];
	pthread_t cthreads[MAX_PLAYERS];
	struct for_thread for_threads[MAX_PLAYERS];
	struct sockaddr_in clients[MAX_PLAYERS];
	int customer_size = sizeof(struct sockaddr_in);
	int main_socket = create_socket(PORT);
	printf("Server started listening on port %d\n", PORT);

	struct configuration* configuration_to_send = configuration_alloc();
	configuration_set_values(configuration_to_send, 800, 600, 50, 1, 0, 100, 500);
	
	//Configure sender process
	pthread_t sender_thread;
	int result = pthread_create(&sender_thread, NULL, sender, NULL); //Change arguments!!!!
	pthread_detach(sender_thread);
	if (result!=0){
			printf("Failed to create sender thread!");
	}
	//Configure receiver process
	pthread_t receiver_thread;
	int result = pthread_create(&receiver_thread, NULL, receiver, NULL); //Change arguments!!!!
	pthread_detach(receiver_thread);
	if (result!=0){
			printf("Failed to create receiver thread!");
	}

	while (1){
		csockets[player_count] = accept(main_socket, (struct sockaddr *)&clients[player_count], &customer_size);
		if (csockets[player_count] < 0){
            printf("Error: accept() failed\n");
			continue;
        }
		send_payload(csockets[player_count], configuration_to_send, sizeof())
    	//bzero(buff, BUFFSIZE);
	}
	free(temp);
	close_socket(main_socket);
	return 0;




	configuration_free(configuration_to_send);
}

void setup_globals(){
	//Config tank lists
	player_count = 0;
	tanks_in_game = (struct tank_t**)malloc(MAX_PLAYERS*(sizeof(struct tank*)));
	if (tanks_in_game == NULL){
		printf("Error allocating memory. Bye!\n");
	}
	projectiles_in_game = (struct projectile**)malloc(MAX_PROJECTILES*(sizeof(struct projectile*)));
	
}