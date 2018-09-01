#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_MESSAGE 1000
#define MAX_REPLY 2000


int
main(void) {
    int _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) {
        puts("Could not create socket");
    }
    puts("Socket created");

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    if (connect(_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connection failed. Maybe you forgot to open the server?\nError");
        return 1;
    }
    puts("Connected succesfully!");
    char message[MAX_MESSAGE], reply[MAX_REPLY];
    int remaining = 0;
    while(1) {
        if (remaining < 1) {
            printf("Enter message: ");
            scanf("%s", message);
            remaining = strlen(message);
            if (remaining > MAX_MESSAGE) {
                puts("!!!\nInput is larger than allowed. The server will response in chunks!\n!!!");
            }
        }
        if (send(_socket, message, strlen(message), 0) < 0) {
            puts("Send failed!");
            return 1;
        }

        int recieved = 0;
        if ((recieved = recv(_socket, reply, MAX_MESSAGE, 0)) < 0) {
            puts("Recv failed!");
            break;
        }
        remaining -= recieved;

        reply[recieved] = '\0';
        if (reply[recieved - 1] == '\n'){
            reply[recieved - 1] = '\0';
        }

        printf("Server reply: %s\n", reply);
        bzero(message, 1);
    }

    close(_socket);
    return 0;
}
