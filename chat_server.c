#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/ioctl.h>


#define MAX_MESSAGE 1000
#define MAX_REPLY 2000

void* read_buf(void*);
void* write_buf(void*);

int remaining = 0;
int _socket;

int
main(void) {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
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

    int count;
    while(1) {
        ioctl(_socket, FIONREAD, &count);
        if (count != 0) {
            printf("%d\n", count);
        }
        pthread_t listen;
        pthread_t write;
        if(pthread_create(&listen, NULL, read_buf, (void*) 0) < 0) {
            perror("Could not create listen thread");
            exit(EXIT_FAILURE);
        }
        if(pthread_create(&write, NULL, write_buf, (void*) 1) < 0) {
            perror("Could not create write thread");
            exit(EXIT_FAILURE);
        }
        if (count > 0) {
            pthread_join(write, NULL);
        } else {
            pthread_join(listen, NULL);
        }

    }

    close(_socket);
    exit(EXIT_SUCCESS);
}

void*
read_buf(void* a) {
    char reply[MAX_REPLY];
    int recieved = 0;

    if ((recieved = recv(_socket, reply, MAX_MESSAGE, 0)) < 0) {
        puts("Recv failed!");
        exit(EXIT_FAILURE);
    }

    remaining -= recieved;

    reply[recieved] = '\0';
    if (reply[recieved - 1] == '\n'){
        reply[recieved - 1] = '\0';
    }

    printf("\n> %s\n", reply);
    fflush(stdout);
    return 0;
}

void*
write_buf(void* a) {
    char message[MAX_MESSAGE];
    if (remaining < 1) {
        // fflush(stdout);
        putchar('\n');
        printf("Enter message: ");
        fflush(stdin);
        fgets(message, MAX_MESSAGE, stdin);
        // scanf("%10[0-9a-zA-Z ]", message);
        remaining = strlen(message);
        if (remaining > MAX_MESSAGE) {
            puts("!!!\nInput is larger than allowed. The server will response in chunks!\n!!!");
        }
    }

    if (send(_socket, message, strlen(message), 0) < 0) {
        puts("Send failed!");
        exit(EXIT_FAILURE);
    }
    bzero(message, 1);
    return 0;
}
