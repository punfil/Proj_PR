#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

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

//Global variables for all threads
int player_count; //Default 0
tank_t ** tanks_in_game;

//pragma pack()

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

void* serveTheClient(void* arg){
	for_thread* my_data = (for_thread*)arg;
	printf("Accepted connection from %s\n", inet_ntoa(my_data->client->sin_addr));  
	printf("Sending configuration to client!\n");
    send_payload(*my_data->csocket, my_data->configuration, sizeof(struct configuration_t));
	close_socket(*my_data->csocket);
	player_count--;
	printf("Closing connection to client\n");
    printf("----------------------------\n");
}
int main() {
	
	player_count = 0;
	tanks_in_game = (tank_t**)malloc(MAX_PLAYERS*(sizeof(tank_t*)));
	if (tanks_in_game == NULL){
		printf("Error allocating memory. Bye!\n");
	}
	for (int i=0;i<MAX_PLAYERS;i++){
		tanks_in_game[i] = (tank_t*)malloc(MAX_PLAYERS*(sizeof(tank_t)));
		if (tanks_in_game[i] == NULL){
			printf("Error allocating memory. Bye!\n");
		}		
	}
	
	int csockets[MAX_PLAYERS];
	pthread_t cthreads[MAX_PLAYERS];
	for_thread for_threads[MAX_PLAYERS];
	struct sockaddr_in clients[MAX_PLAYERS];

	int customer_size = sizeof(struct sockaddr_in);
	printf("Hello server!\n");
	int main_socket = create_socket(PORT);


	printf("Here not hello\n");
	
	//Buffers for incoming data
	//int BUFFSIZE=512;
	//char buff[BUFFSIZE];
	struct configuration_t* temp = (struct configuration_t*)malloc(sizeof(struct configuration_t));
	temp->width = 800;
	temp->height = 600;
	temp->background_scale = 50;
	temp->player_count = 1;
	temp->player_id = 1;
	temp->tank_spawn_x = 100;
	temp->tank_spawn_y = 500;
	printf("Server started listening on port %d\n", PORT);
	int i=0;
	//NOW WORKS - THE PROBLEM IS RACE CONDITION TO VARIABLE player_count - this has to be dealt with. Temporarly solved by adding sleep(3) in main (bad...)
	while (1){
		csockets[player_count] = accept(main_socket, (struct sockaddr *)&clients[player_count], &customer_size);
		if (csockets[i] < 0){
            printf("Error: accept() failed\n");
			continue;
        }
		for_threads[player_count].player_id = player_count;
		for_threads[player_count].csocket = &csockets[player_count];
		for_threads[player_count].client = &clients[player_count];
		for_threads[player_count].configuration = temp;
		player_count++;
		printf("Players count: %d\n", player_count);
		int result = pthread_create(&cthreads[player_count-1], NULL, serveTheClient, &for_threads[player_count-1]);
		pthread_detach(cthreads[player_count-1]);
		if (result!=0){
			printf("Failed to create thread");
		}
		i++;
		sleep(3);
    	//bzero(buff, BUFFSIZE);
	}
	free(temp);
	close_socket(main_socket);
	return 0;
}