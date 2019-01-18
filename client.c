/// CLIENT
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <pthread.h>

#define SERVER_SOCKET_PATH "./server.sock"

pthread_mutex_t mutex;

struct client{
    int fd;
    char* name;
};

void intHandler(int fd){
    close(fd);
    exit(0);
}

int handleMessage(char *str){
    char * read_message;
    char dot[] = ".";
    char *Y, *Z;
    int X;
    int i = 0;
    read_message = strtok(str, dot);
    while(read_message != NULL){
        if(i==0){
            X = (int)strtol(read_message, NULL, 10);
            if(X==7){
                printf("\n%s\ntype:", str);
                printf("Press enter\n");
                return 0;
            }            
        }
        else if(i==1){
            Y = read_message;
            if((int)strtol(Y,NULL, 10) == 0 && X == 1){
                printf("1.000000");
                return 0;    
            }        
        }
        else if(i==2)
            Z = read_message;
        read_message = strtok(NULL, dot);
        i++;
    }
    printf("\n%s: %s\ntype:", Y, Z);        
    return 0;
}

int c=0;
void *handleServerResponse(void *fd){
    char buffer[1000], *message;
    int buf_length = 1000, bytes = 0;
    int client_fd = *(int*)fd;
    char *response;
    while(1){
        pthread_mutex_lock(&mutex);
        if((bytes = recv(client_fd, buffer, buf_length, 0)) < 0){  
            perror("recv");
        }
        else{
            buffer[bytes] = '\0';
            int errorMessage = handleMessage(buffer);    
            buffer[0] ='\0';
            pthread_mutex_unlock(&mutex);
        }        
        if(bytes == 0){
            printf("Server down :( Try again later\n");
            exit(0);
        }
       
    }
    return NULL;
}

void* handleSend(void* fd){
    char* buffer, reply[1000];
    int bufsize = 1000, bytes = 0;
    int client_fd = *(int*)fd;
    while(1){
        printf("type:");
        scanf("%s", buffer);
        if(strcmp(buffer, "q") == 0){
            printf("bye\n");
            exit(0);
        }            
        if(send(client_fd, buffer, strlen(buffer), 0) < 0)
            printf("Client: failed to send\n");
      
        if((bytes = recv(client_fd, reply, bufsize, 0)) < 0){
            perror("recv");
        }
        else if(bytes > 0){
            reply[bytes] = '\0';
            printf("Server response: %s\n", reply);
            
        }
        reply[0] ='\0';
        buffer[0] = '\0';
        
    }
    return NULL;
}

int main(int argc, char* argv){

    int client_status = 0;
    signal(SIGINT, intHandler);

    if(pthread_mutex_init(&mutex, NULL)){
		printf("mutex1 failed");
		exit(0);
	}
	
    int client_fd;
    struct sockaddr_un server_addr;

    if((client_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0){
        printf("Client: failed to create socket. Exiting...");
        exit(0);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_LOCAL;
    strncpy(server_addr.sun_path, SERVER_SOCKET_PATH, sizeof(server_addr.sun_path)-1);

    if(connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        printf("Client: Failed to connect to server. Exiting...\n");
        exit(0);
    }
    else{ 
        printf("Client: Successfully connected to server.\n");
        client_status = 1;
    }
    
    pthread_t* tid;
    int *new_fd;
    new_fd = malloc(1);
    *new_fd = client_fd;
    printf("Welcome to your new communicator.\n");
    printf("To exit type q\n");
    printf("To log in type: 2.[name]\n");
    printf("To send a message type: 3.[user name].[message]\n");
    printf("To request a list of available users type: 6.\n");
    printf("To broadcast type: 5.[your name].[message]\n");
    
    if(pthread_create(&tid[0], NULL, handleServerResponse, (void*)new_fd) < 0){
           printf("Thread failed\n");
           exit(0);
    }
    if(pthread_create(&tid[1], NULL, handleSend, (void*)new_fd) < 0){
           printf("Thread failed\n");
           exit(0);
    }
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    
    while(1){

    }
    pthread_mutex_destroy(&mutex);
    close(client_fd);
    exit(0);
    return 0;
}