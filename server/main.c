//RUN ON LINUX!

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PORT 2137

//#pragma pack(1)
typedef struct payload_t {
	uint32_t player_id;
	uint32_t x_location;
	uint32_t y_location;
};
enum true_or_false{
	false,
	true,
}

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
    listen(sock , 32); //backlog value - how many incoming connections can queue up
    return sock;
}

void close_socket(int sock){
	close(socket);
}

void send_payload(int sock, payload_t* msg){
	if(write(sock, msg, sizeof(payload_t))<0){ //write returns number of bytes sent
		return false;
	}
	return true;
}


int main() {

	return 0;
}