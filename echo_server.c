#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_REPLY 2000
#define MAX_CLIENT 25

void* connection_handler(void*);
void remove_client(int);

pthread_mutex_t lock;
int* clients[MAX_CLIENT];
int client_count = 0;

int
main(void) {
    int _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) {
        printf("Could not create socket!\n");
        exit(EXIT_FAILURE);
    }

    puts("Socket created!");

    struct sockaddr_in server, client;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&server,sizeof(int));

    if (bind(_socket,(struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed. Error");
        exit(EXIT_FAILURE);
    }

    bzero(&server, sizeof(server));

    listen(_socket, 3);

    puts("Waiting for incoming connections...");

    int c = sizeof(struct sockaddr_in);
    int* new_sock;
    int c_socket;


    if (pthread_mutex_init(&lock, NULL) != 0) {
        perror("Mutex init failed");
        exit(EXIT_FAILURE);
    }

    while ((c_socket = accept(_socket, (struct sockaddr *)&client, (socklen_t*)&c)) > 0) {
        if (c_socket < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        puts("Connection accepted!");

        pthread_t socket_thread;
        new_sock = malloc(1);
        *new_sock = c_socket;

        // store client
        clients[client_count] = new_sock;
        ++client_count;

        if(pthread_create(&socket_thread, NULL, connection_handler, (void*) new_sock) < 0) {
            perror("Could not create thread");
            exit(EXIT_FAILURE);
        }

        puts("Handler assigned");
    }

    if (c_socket < 0) {
        perror("Connection failed");
        return 1;
    }
    pthread_mutex_destroy(&lock);
    exit(EXIT_SUCCESS);
}

void*
connection_handler(void *socket_id) {
    int c_socket = *(int*)socket_id;
    int recieved;
    char incoming[MAX_REPLY];
    char response[MAX_REPLY + 15];
    // pthread_mutex_lock(&lock);
    while ((recieved = recv(c_socket, incoming, MAX_REPLY, 0)) > 0 ) {
        incoming[recieved] = '\0';
        if (incoming[recieved - 1] == '\n'){
            incoming[recieved - 1] = '\0';
        }
        // pthread_mutex_unlock(&lock);
        if (recieved == 0) {
            puts("Client disconnected");
            remove_client(c_socket);
            close(c_socket);
            fflush(stdout);
        }

        else if (recieved == -1) {
            perror("Recv failed");
        }

        sprintf(response, "#%d Says: ", c_socket);
        strcat(response, incoming);
        printf("%s\n", response);
        for (size_t i = 0; i < client_count; i++) {
            printf("index: %zu\n", i);
            if (*clients[i] != c_socket){
                printf("serving %d\n", *clients[i]);
                write(*clients[i], response, strlen(response) + 1);
                printf("Wrote to socket: %d\n", *clients[i]);
            }
        }
        strcpy(incoming, "");
        bzero(incoming, 100);
    }

    strcpy(response, "");
    bzero(response, 100);

    free(socket_id);
    return 0;
}

void
remove_client(int value) {
    int i = 0;
    while (i < client_count && *clients[i] != value) {
        ++i;
    }
    if (i == client_count) {
        puts("Can't remove client cleanly.");
        exit(EXIT_FAILURE);
    } else {
        puts("Removing client...");
        *clients[i] = 0;
        --client_count;
        return;
    }

}
