#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_REPLY 2000

int
main(void) {
    int _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) {
        printf("Could not create socket!\n");
    }

    puts("Socket created!");

    struct sockaddr_in server, client;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    if (bind(_socket,(struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed. Error");
        return 1;
    }

    bzero(&server, sizeof(server));

    listen(_socket, 3);

    puts("Waiting for incoming connections...");
    int c = sizeof(struct sockaddr_in);

    int c_socket = accept(_socket, (struct sockaddr *)&client, (socklen_t*)&c);
    if (c_socket < 0) {
        perror("Accept failed");
        return 1;
    }

    puts("Connection accepted");

    int recieved;
    char incoming[MAX_REPLY];
    while ((recieved = recv(c_socket, incoming, MAX_REPLY, 0)) > 0 ) {
        incoming[recieved] = '\0';
        if (incoming[recieved - 1] == '\n'){
            incoming[recieved - 1] = '\0';
        }
        write(c_socket, incoming, strlen(incoming) + 1);
        strcpy(incoming, "");
        bzero(incoming, 100);
    }

    if (recieved == 0) {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if (recieved == -1) {
        perror("Recv failed");
    }

    return 0;
}
