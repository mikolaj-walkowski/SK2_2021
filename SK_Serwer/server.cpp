#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <string>
#include <map>
#include "Client.h"
#include "enums.h"
#include "Room.h"
#include "Protocol.h"

#define SERVER_PORT 4200
#define QUEUE_SIZE 5

#define MAX_EVENTS 10

struct epoll_event ev, events[MAX_EVENTS];
int nfds, epollfd;

std::map<int, Client *> clients;
std::vector<Room *> rooms;

void hc_create(Client *client, char *buff)
{
    //TODO ew dodać guardy na cstring lenng
    printf("\tClient %s requests: create %s, ",client->nick,buff+7);
    char *room_name = createBuffer(NULL,1, buff+7);
    sprintf(room_name,"%s",buff + 7);

    for (auto itr : rooms)
    {
        if (strcmp(itr->name,room_name)==0)
        {
            printf("Failure\n");
            if (write(client->fd, "failed create", 14) == -1)
            {
                perror("Couldnt write to socket");
            }
            free(room_name);
            return;
        }
    }
    printf("Success\n");
    rooms.push_back(new Room(client, room_name));
}

void hc_join(Client *client, char *buff)
{
    printf("\tClient %s requests: join %s, ",client->nick,buff+5);
    for (auto itr : rooms)
    {
        if (strcmp(itr->name,buff+5)==0)
        {
            printf("Success\n");
            if (write(client->fd, "success", 8) == -1)
            {
                perror("Couldnt write to socket");
            }
            itr->addClient(client);
            client->currRoom = itr;
            return;
        }
    }

    printf("Failure\n");
    if (write(client->fd, "failed join", 12) == -1)
    {
        perror("Couldnt write to socket");
    }
}

char* readUntil(int fd, char stop)
{
    char* handle = (char *)malloc(sizeof(char) * 100);
    int sizeLimit = 100, i = -1;
    do{
        ++i;
        if (!i < sizeLimit)
        {
            sizeLimit += 100;
            handle = (char *)realloc(handle, sizeof(char) * sizeLimit);
        }
        read(fd, handle + i, 1);
        //printf("%c(%d)",handle[i],handle[i]);
    }while (handle[i] != stop);
    //printf("\n");
    //printf("%d, %s\n",handle,handle);
    return handle;
}

void hc_kick(Client *client, char *buff)
{
    printf("\tClient %s requests: kick %s, ",client->nick,buff+5);
    if (client->currRoom->owner == client->nick)
    {
        if (client->currRoom->kickClient(buff+5) == 0)
        {
            printf("Success\n");
            if (write(client->fd, "success kick", 13) == -1)
            {
                perror("Couldnt write to socket");
            }
        }
        else
        {
            printf("Failure\n");
            if (write(client->fd, "failed kick", 12) == -1)
            {
                perror("Couldnt write to socket");
            }
        }
        free(buff+5);
    }
}

void hc_msg(Client *client, char *buff)
{
    client->currRoom->broadcastMsg(client->nick, buff+4);
}

void hc_leave(Client* client){
    printf("\tClient %s requests: leave, ",client->nick);

    if (client->currRoom != NULL)
        {
            client->currRoom->removeClient(client);
            client->currRoom = NULL;
    }
}


void handleClient(Client *client)
{
    char *buff = readUntil(client->fd,'\0');

    printf("Incoming msg %s\n",buff);
    protocol_keywords key = resolve_pkw(buff);
    switch (key)
    {
    case kick:
    {
        hc_kick(client, buff);
        break;
    }
    case join:
    {
        hc_join(client, buff);
        break;
    }
    case leave:
    {
        hc_leave(client);
        break;
    }
    case create:
    {
        hc_create(client, buff);
        break;
    }
    case msg:
    {
        hc_msg(client, buff);
        break;
    }
    default:
    {
        printf("Bad request\n");
        break;
    }
    }
    free(buff);
    printf("\n");
}
void guard(int i, std::string s)
{
    if (i < 0)
    {
    }
}
int main(int argc, char *argv[])
{
    int server_socket_descriptor;
    int connection_socket_descriptor;
    int bind_result;
    int listen_result;
    char reuse_addr_val = 1;
    struct sockaddr_in server_address;
    int maxid = 1;

    //inicjalizacja gniazda serwera

    memset(&server_address, 0, sizeof(struct sockaddr));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(SERVER_PORT);

    server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_descriptor < 0)
    {
        fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda..\n", argv[0]);
        exit(1);
    }

    setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse_addr_val, sizeof(reuse_addr_val));

    bind_result = bind(server_socket_descriptor, (struct sockaddr *)&server_address, sizeof(struct sockaddr));
    if (bind_result < 0)
    {
        fprintf(stderr, "%s: Błąd przy próbie dowiązania adresu IP i numeru portu do gniazda.\n", argv[0]);
        exit(1);
    }

    listen_result = listen(server_socket_descriptor, QUEUE_SIZE);
    if (listen_result < 0)
    {
        fprintf(stderr, "%s: Błąd przy próbie ustawienia wielkości kolejki.\n", argv[0]);
        exit(1);
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        fprintf(stderr, "Blad epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = server_socket_descriptor;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket_descriptor, &ev) == -1)
    {
        fprintf(stderr, "Blad epoll_ctl ADD dla server socket");
    }
    ev.data.fd = 0;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &ev) == -1)
    {
        fprintf(stderr, "Blad epoll_ctl ADD dla klawiatury");
    }
    int create_result = 0;
    for (;;)
    {
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            fprintf(stderr, "Błąd epoll wait");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < nfds; ++i)
        {
            if (events[i].data.fd == 0)
            {
                //FIXME sockety nie zamykają się automatycznie client side
                char buff[10];
                read(0, buff, 10);
                for (auto itr = clients.begin(); itr != clients.end(); itr++)
                {
                    write(itr->first, "terminate", 10);
                    shutdown(itr->first, SHUT_RDWR);
                    close(itr->first);
                    printf("Closed socket %d\n", itr->first);
                }
                close(server_socket_descriptor);
                return (0);
            }
            if (events[i].data.fd == server_socket_descriptor)
            {
                struct sockaddr_in client_sock;
                socklen_t slen = sizeof(client_sock);
                connection_socket_descriptor = accept(server_socket_descriptor, (struct sockaddr *)&client_sock, &slen);
                fcntl(connection_socket_descriptor, F_SETFL, O_NONBLOCK);

                if (connection_socket_descriptor == -1)
                {
                    fprintf(stderr, "Blad polaczenia");
                    exit(EXIT_FAILURE);
                }
                ev.events = EPOLLIN | EPOLLRDHUP;
                ev.data.fd = connection_socket_descriptor;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection_socket_descriptor, &ev) == -1)
                {
                    fprintf(stderr, "Blad dodania klienata do kolejki epoll");
                    exit(EXIT_FAILURE);
                }
                char* nick = (char*)malloc(sizeof(char)*10);
                sprintf(nick,"User%d",maxid++);
                printf("Added Client: \n\tfd: %d\n\tNick: %s\n",connection_socket_descriptor,nick);
                struct Client *dummy = new Client(connection_socket_descriptor,nick);
                clients.insert(std::make_pair(connection_socket_descriptor, dummy));
                continue;
            }
            if (events[i].events & EPOLLRDHUP)
            {
                auto client = clients[events[i].data.fd];
                if (client->currRoom != NULL)
                {
                    client->currRoom->removeClient(client);
                }
                clients.erase(events[i].data.fd);
                close(client->fd);
                printf("Deleted client ip: %s\n", client->nick);
                delete client;
            }
            else
                try
                {
                    handleClient(clients.at(events[i].data.fd));
                }
                catch (const std::exception &e)
                {
                }
        }
    }
    return (0);
}
