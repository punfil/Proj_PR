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
}payload_t;

enum true_or_false{
	false,
	true,
};

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
	close(sock);
}

enum true_or_false send_payload(int sock, struct payload_t* msg){
	if(write(sock, msg, sizeof(struct payload_t))<0){ //write returns number of bytes sent
		return false;
	}
	return true;
}


int main() {
	int BUFFSIZE=512;
	char buff[BUFFSIZE];
	int ssock, csock;
	int nread;
	struct sockaddr_in client;
	int customer = sizeof(client);
	ssock = create_socket(PORT);
	printf("Server started listening on port %d\n", PORT);
	csock = accept(ssock, (struct sockaddr *)&client, &customer);
	if (csock < 0)
        {
            printf("Error: accept() failed\n");
			close_socket(ssock);
			return 0;
        }

    printf("Accepted connection from %s\n", inet_ntoa(client.sin_addr));
    bzero(buff, BUFFSIZE);
    while ((nread=read(csock, buff, BUFFSIZE)) > 0)
    {
        printf("\nReceived %d bytes\n", nread);
        struct payload_t *p = (struct payload_t*) buff;
        printf("Received contents: id=%d, counter=%d, temp=%d\n",
        p->player_id, p->x_location, p->y_location);

        printf("Sending it back.. ");
        send_payload(csock, p);
    }
    printf("Closing connection to client\n");
    printf("----------------------------\n");
	close_socket(ssock);
	return 0;
}