// demoServer.c
// Created by Ryland Sacker and Alex Johnson
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define PORT 8080
#define MAX_CLIENTS 2
void sigHandler(int);

int main() {
   
    int server_fd, client_sockets[MAX_CLIENTS], new_socket;
    struct sockaddr_in address;
    int opt = 1, addrlen = sizeof(address);
    char buffer[1024] = {0};
    
    // Initialize client sockets to 0 (empty)
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Define server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Accept two clients
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
        }
    client_sockets[0] = new_socket;
        printf("Client %d connected\n", 0 + 1);

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    client_sockets[1] = new_socket;
        printf("Client %d connected\n", 1 + 1);
   

   // Relay messages between the two clients
    while (1) {
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int valread = read(client_sockets[i], buffer, 1024);
            if (valread > 0) {
                buffer[valread] = '\0';
                printf("Client %d: %s\n", i + 1, buffer);

                // Send message to the other client
                int recipient = (i == 0) ? 1 : 0;
                send(client_sockets[recipient], buffer, strlen(buffer), 0);
		char *s = strstr(buffer, "BYE");
		if(s != NULL) {
			return 0;
		}
            }
        }
    }

    return 0;
}
void sigHandler(int sigNum)
{
  printf("Please exit from client");
} 
