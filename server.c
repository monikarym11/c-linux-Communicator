/// SERVER
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
#define CLIENT_LIST "./clients.txt"
pthread_mutex_t mutex, mlist, mlog;

struct client{
    int fd;
    char* name;
    int status;
};

void intHandler(){
    remove(CLIENT_LIST);
    exit(0);
}

void* handleList(void* fd){
    int ret0, *ret00, ret1, *ret11;
    ret1 = 1;
    ret11 = malloc(1);
    *ret11 = ret1;
    ret0 = 0;
    ret00 = malloc(1);
    *ret00 = ret0;
    pthread_mutex_lock(&mlist);
    int client_fd = *(int*)fd;
    char str[100];
    strcpy(str, "7");
    FILE *file = fopen(CLIENT_LIST, "r");
    if(!file){
        fclose(file);
        pthread_mutex_unlock(&mlist);
        pthread_exit((void*)ret11);
    }
    else{
        int id, status;
        char name[10];
        while(fscanf(file,"%d\t%s\t%d\n", &id, name, &status)!=EOF){
            strcat(str, ".");
            strcat(str, name);
            name[0]='\0';
        }  
        fclose(file);
        if(send(client_fd, str, strlen(str), 0) < 0){
            perror("write");
            pthread_mutex_unlock(&mlist);
            pthread_exit((void*)ret11);
        }
        else{
            printf("List sent");
        }
    }
    pthread_mutex_unlock(&mlist);
    pthread_exit((void*)ret00);
    return NULL;
}

int handleSend(char * from_name, char* to_name, char * message_to_send){
    int fd, status;
    char name[10];
    char str[100];
    FILE *file = fopen(CLIENT_LIST, "r");
    strcat(str, "4");
    strcat(str, ".");
    strcat(str, from_name);
    strcat(str, ".");
    strcat(str, message_to_send);

    while(fscanf(file,"%d\t%s\t%d\n", &fd, name, &status)!=EOF){
        if(strcmp(to_name, name) == 0){
            break;
        }
    }  
    fclose(file);
    if(status == 1){
        if(send(fd, str, strlen(str), 0) < 0){
            perror("write");
            return -1;
        }            
    }
    else{
        return -1;
    }
    return 0;
}
void* handleLog(void* log_client){
    int ret0, *ret00, ret1, *ret11, ret2, *ret22, ret4, *ret44;
    ret0 = 0;
    ret00 = malloc(1);
    *ret00 = ret0;
    ret1 = 1;
    ret11 = malloc(1);
    *ret11 = ret1;
    ret2 = 2;
    ret22 = malloc(1);
    *ret22 = ret2;
    ret4 = 4;
    ret44 = malloc(1);
    *ret44 = ret4;
    struct client *cl = (struct client*)log_client;
    int fd, status;
    char name[10];
    pthread_mutex_lock(&mlog);
    FILE *file = fopen(CLIENT_LIST, "a+");
    if(file == NULL){
        pthread_exit((void*)ret11);
    }
    while(fscanf(file,"%d\t%s\t%d\n", &fd, name, &status)!=EOF){
        if(strcmp(cl->name, name) == 0){
            fclose(file);
            pthread_mutex_unlock(&mlog);
            pthread_exit((void*)ret22);
        }
        if(cl->fd == fd)
        {
            fclose(file);
            pthread_mutex_unlock(&mlog);
            pthread_exit((void*)ret44);
        }
    }  
    fprintf(file, "%d\t%s\t%d\n", cl->fd, cl->name, 1);
    fclose(file);
    printf("User %s logged in.\n", cl->name);
    pthread_mutex_unlock(&mlog);
    pthread_exit((void*)ret00);
    return NULL;
}

int handleBroadcast(char * from_name, char * message_to_send){
    int fd, status;
    char name[10];
    char str[100];
    FILE *file = fopen(CLIENT_LIST, "r");
    strcat(str, "4");
    strcat(str, ".");
    strcat(str, from_name);
    strcat(str, ".");
    strcat(str, message_to_send);

    while(fscanf(file,"%d\t%s\t%d\n", &fd, name, &status)!=EOF){
        if(strcmp(from_name, name) == 0){
            continue;
        }
        if(status == 1){
            if(send(fd, str, strlen(str), 0) < 0){
                perror("write");
                return -1;
            }            
        }
        else{
            return -1;
        }
    }  
    fclose(file);    

    return 0;
}
int handleMessage(char * str, int fd){
    char * read_message;
    char dot[] = ".";
    char *Y, *Z;
    int X;
    int i = 0;
    read_message = strtok(str, dot);
    while(read_message != NULL){
        if(i==0){
            X = (int)strtol(read_message, NULL, 10);
        }
        else if(i==1){
            Y = read_message;
        }
        else if(i==2){
            Z = read_message;
        }
        read_message = strtok(NULL, dot);
        i++;
    }
    if(i==1){
        if(X == 6){
            if(!CLIENT_LIST){
                return -6;
            }
            pthread_t lthr;
            void *ltid;
            int client_fd, *new_fd;
            client_fd = fd;
            new_fd = malloc(1);
            *new_fd = client_fd;            
            if(pthread_create(&lthr, NULL, handleList, (void*)new_fd)<0){
                exit(0);
            }
            pthread_join(lthr, &ltid);
            int ret = *(int*)ltid;
            free(ltid);
            if(ret == 1){
                printf("List error\n");
                return -6;
            } 
            else{
                return 7;
            }   
            
            return 7;
        }
        else{
            return -8;
        }
    }
    else if(i == 2){
        if(X == 2){
            pthread_t lthr;
            void *logth;
            struct client *log_client;
            log_client = (struct client*)malloc(sizeof(struct client));
            log_client->fd = fd;
            log_client->name = Y;
            if(pthread_create(&lthr, NULL, handleLog, (void*)log_client)<0){
                exit(0);
            }
            pthread_join(lthr, &logth);
            int ret = *(int*)logth;
            free(logth);
            printf("%d\n", ret);
            if(ret == 1){
                return -2;
            }
            else if(ret == 2){
                return 2;
            }
            else if(ret == 4){
                return -4;
            }
            return 0;            
        }
        else{
            return -8;
        }
    }
    else if(i == 3){
        if(X == 3){
            int ffd, status;
            char name[10];
            FILE *file = fopen(CLIENT_LIST, "r");
            while(fscanf(file,"%d\t%s\t%d\n", &fd, name, &status)!=EOF){
                if(fd == ffd)
                    break;
            } 
            fclose(file);
            if(handleSend(name, Y, Z)==-1){
                return -3;
            }
            else
                return 0;

        }        
        else if(X == 5){
            if(handleBroadcast(Y, Z) == -1){
                return -5;
            }
            else{
                return 0;
            }
        }
        else{
            return -8;
        }                
    }
    else{
        return -8;
    }    
    return 0;
}

void *handleClient(void *fd){
    pthread_mutex_lock(&mutex);
    char buffer[1000], *message;
    int buf_length = 1000, bytes = 0;
    int client_fd = *(int*)fd;
    char *response;
    message = "Now type";
    printf("Client %d\n", client_fd);
  
    while((bytes = recv(client_fd, buffer, buf_length, 0)) > 0){  
        buffer[bytes] = '\0';
        printf("\nBytes received: %d, Message received: %s\n", bytes, buffer);
        int errorMessage = handleMessage(buffer, client_fd);
        if(errorMessage == -2){
            response = "1.2.Could not log in\n";
        }
        else if(errorMessage == 2){
            response = "1.2.Could not log in, name already exists.\n";
        }
        else if(errorMessage == -4){
            response = "1.2.Could not log in. You are already logged in.\n";
        }
        else if(errorMessage == -3){
            response = "1.3.Could not send message\n";
        }
        else if(errorMessage == -5){
            response = "1.5.Broadcast failed\n";
        }
        else if(errorMessage == -6){
            response = "1.6.List is empty :(\n";
        }
        else if(errorMessage == -8){
            response = "1.8.Wrong option\n";
        }
        else if(errorMessage == 0){
            response = "1.0";            
        }
        if(errorMessage != 7){
            if(send(client_fd, response, strlen(response), 0) < 0)
                perror("write");
        }
      
        buffer[0] ='\0';
        
        pthread_mutex_unlock(&mutex);
    }
    if(bytes == 0){
        printf("Client %d disconnected\n", client_fd);
        FILE *file = fopen(CLIENT_LIST, "r");
        FILE *temp = fopen("temp.txt", "w");
        int fd, status;
        char name[10];
        if(!temp){
            printf("log out error");
            fclose(file);
            return NULL;
        }
        char str[100];
        
        if(!file){
            printf("log out error");
            fclose(file);
            return NULL;
        }
        else{
            while(!feof(file)){
                strcpy(str, "\0");                
                if(!feof(file)){
                    fscanf(file,"%d\t%s\t%d\n", &fd, name, &status);
                    if(fd == client_fd)
                        continue;
                    else{
                        fgets(str, 100, file);
                        fprintf(temp, "%s", str);
                    }
                }
            }
        fclose(file);
        fclose(temp);
        remove(CLIENT_LIST);
        rename("temp.txt", CLIENT_LIST);
        }
    }
    else if(bytes == -1){
        printf("Read failed\n");
        perror("error");
    }  
    
}

int main(){
    
    signal(SIGINT, intHandler);
    if(SERVER_SOCKET_PATH)
        remove(SERVER_SOCKET_PATH);
    if(CLIENT_LIST)
        remove(CLIENT_LIST);
    FILE *file = fopen(CLIENT_LIST, "a");
    if(!file){
        exit(0);
    }
    fclose(file);

    int server_ready = 0;

    //sockets
    struct sockaddr_un server_addr, client_addr;
    int server_fd, client_fd, *new_fd;
    socklen_t addrsize = sizeof(struct sockaddr_un);

    //socket, bind
    if((server_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0){
        printf("Server: Failed to create server socket. Exiting...\n");
        exit(0);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_LOCAL;
    strncpy(server_addr.sun_path, SERVER_SOCKET_PATH, sizeof(server_addr.sun_path)-1);

    if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) < 0){
        printf("Server: Failed to bind to a socket. Exiting...\n");
        exit(0);
    }

    //Start listening
    if(listen(server_fd, 10) < 0){
        printf("Server: Failed to listen. Exiting...");
        exit(0);
    }

    //epoll
    int epoll_fd;
    struct epoll_event s_event, *c_events;

    if((epoll_fd = epoll_create1(0)) < 0 ){
        printf("Server: Failed to create epoll, Exiting...");
        exit(0);
    }

    s_event.events = EPOLLIN;
    s_event.data.fd = server_fd;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, s_event.data.fd, &s_event) < 0){
        printf("Server: Operation epoll_ctl failed. Exiting...\n");
        exit(0);
    }
    else
        server_ready = 1;

    //Handle messages
    
    printf("Server status: %d\n", server_ready);    

    pthread_t *tid;
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mlist, NULL);
    pthread_mutex_init(&mlog, NULL);
    while((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrsize)) >= 0){  
        new_fd = malloc(1);     
        *new_fd = client_fd;
       if(pthread_create(&tid[client_fd], NULL, handleClient, (void*)new_fd) < 0){
           printf("Thread failed\n");
           exit(0);
       }
       else   
        printf("Thread %d created\n", client_fd);
        
    }
    if(client_fd < 0){
            printf("Client denied\n");
    }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mlist);
    pthread_mutex_destroy(&mlog);
    pthread_exit(NULL);
    
    return 0;
}