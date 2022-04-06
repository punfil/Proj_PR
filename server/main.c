//RUN ON LINUX!

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PORT 2137
#define MAX_PLAYERS 4

//#pragma pack(1)
typedef struct tank_t {
	uint32_t player_id;
	uint32_t x_location;
	uint32_t y_location;
}tank_t;

typedef struct for_thread {
	uint32_t player_id;
	int csocket;
	struct sockaddr_in client;
}for_thread;

enum true_or_false{
	false,
	true,
};

typedef struct configuration_t{
	uint32_t width;
	uint32_t height;
	uint32_t background_scale;
	uint32_t player_count;
	uint32_t tank_spawn_x;
	uint32_t tank_spawn_y;
}configuration_t;

int player_count; //Default 0
tank_t ** tanks_in_game;
//pragma pack()

int create_socket(int port){
	int sock, err;
	struct sockaddr_in server;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		exit(1); //Connection failed
	}
	bzero((char*) &server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        exit(1);
    }
    listen(sock , MAX_PLAYERS); //backlog value - how many incoming connections can queue up
    return sock;
}

void close_socket(int sock){
	close(sock);
}

enum true_or_false send_payload(int sock, void* msg, uint32_t msgsize){
	if(write(sock, msg, msgsize)<0){ //write returns number of bytes sent
		return false;
	}
	return true;
}

void serveTheClient(){

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
	int main_socket = create_socket(PORT);
	//Buffers for incoming data
	//int BUFFSIZE=512;
	//char buff[BUFFSIZE];
	struct configuration_t* temp = (struct configuration_t*)malloc(sizeof(struct configuration_t));
	temp->width = 800;
	temp->height = 600;
	temp->background_scale = 50;
	temp->player_count = 1;
	temp->tank_spawn_x = 100;
	temp->tank_spawn_y = 500;
	printf("Server started listening on port %d\n", PORT);
	while (1){
		csockets[player_count] = accept(main_socket, (struct sockaddr *)&clients[player_count], &customer_size);
		player_count++;
		if (csockets[player_count-1] < 0){
            printf("Error: accept() failed\n");
			player_count--;
			continue;
        }
		printf("Accepted connection from %s\n", inet_ntoa(clients[player_count-1].sin_addr));  
    	//bzero(buff, BUFFSIZE);
			
		printf("Sending configuration to client!\n");
    	send_payload(csockets[player_count-1], temp, sizeof(struct configuration_t));
    	printf("Closing connection to client\n");
    	printf("----------------------------\n");
		close_socket(csockets[player_count-1]);
		player_count--;
	}
	free(temp);
	close_socket(main_socket);
	return 0;
}