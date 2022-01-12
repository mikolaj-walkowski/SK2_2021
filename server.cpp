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
#define SERVER_PORT 1234
#define QUEUE_SIZE 5

#define MAX_EVENTS 10

struct epoll_event ev, events[MAX_EVENTS];
int nfds, epollfd;

enum client_states
{   };
class Room;

class Client
{
public:
    int fd;
    client_states cs;
    std::string ip;
    Room* currRoom;
    Client(int _fd, std::string _ip):fd(_fd), ip(_ip){
        currRoom=NULL;
    }

};

class Room
{
    std::vector<Client *> room_clients;
    std::vector<std::pair<std::string, std::string>> chatLog;
    public:
    struct Client *owner;
    void addClient(Client *cl)
    {
        room_clients.push_back(cl);
    }
    void removeClient(Client* client){
        for (auto itr = room_clients.begin(); itr != room_clients.end(); itr++){
            if((*itr) == client){
                room_clients.erase(itr);
            }
        }
    }
    void broadcastMsg(Client *cl, std::string msg)
    {
        chatLog.push_back(std::make_pair(cl->ip, msg));
        for (auto itr : room_clients)
        {
            if (cl != itr)
            {
                int size = cl->ip.length() + msg.length() + 6;
                char *buff2 = new char[size];
                sprintf(buff2, "msg %s: %s", cl->ip, msg);
                if (write(itr->fd, buff2, size) == -1)
                {
                    perror("Couldnt write to socket");
                }
                delete [] buff2;
            }
        }
    }
    void kickClient(std::string ip)
    {
        for (auto itr = room_clients.begin(); itr != room_clients.end(); itr++)
            if ((*itr)->ip == ip)
            {
                char *buff = "kick";
                if(write((*itr)->fd, buff, 4) == -1)
                {
                    perror("Couldnt write to socket");
                }
                room_clients.erase(itr);
            }
    }
    void sendChatLog(Client *cl)
    {
        for (auto itr : chatLog)
        {
            int size = itr.first.length() + itr.second.length() + 6;
            char *buff2 = new char[size];
            sprintf(buff2, "msg %s: %s", itr.first , itr.second);
            if (write(cl->fd, buff2, size) == -1)
            {
                perror("Couldnt write to socket");
            }
            delete [] buff2;
        }
    }
    Room(Client *ow) : owner(ow)
    {
        
    }
};

std::map<int, Client *> clients;
std::vector<Room*> rooms;

//struktura zawierająca dane, które zostaną przekazane do wątku
void handleClient(Client* client){

}

int main(int argc, char *argv[])
{
    int server_socket_descriptor;
    int connection_socket_descriptor;
    int bind_result;
    int listen_result;
    char reuse_addr_val = 1;
    struct sockaddr_in server_address;

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

    //TODO stworzyć instancję epoll przez epoll_create1(0);
    int epoll_fd = epoll_create1(MAX_EVENTS);
    if (epoll_fd == -1)
    {
        fprintf(stderr, "Blad epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = server_socket_descriptor, server_socket_descriptor;


    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket_descriptor, &ev) == -1)
    {
        fprintf(stderr, "Blad epollll_ ctl ADD dla server socket");
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
            if (events[i].data.fd == server_socket_descriptor)
            {
                struct sockaddr_in client_sock;
                socklen_t slen = sizeof(client_sock);
                connection_socket_descriptor = accept(server_socket_descriptor, (struct sockaddr *)&client_sock, &slen);
                fcntl(connection_socket_descriptor, F_SETFL, O_NONBLOCK, 1);

                if (connection_socket_descriptor == -1)
                {
                    fprintf(stderr, "Blad polaczenia");
                    exit(EXIT_FAILURE);
                }
                ev.events = EPOLLIN | EPOLLHUP;
                ev.data.fd = connection_socket_descriptor;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection_socket_descriptor, &ev) == -1)
                {
                    fprintf(stderr, "Blad dodania klienata do kolejki epoll");
                    exit(EXIT_FAILURE);
                }
                struct Client *dummy = new Client(connection_socket_descriptor,inet_ntoa(client_sock.sin_addr));
                clients.insert(std::make_pair(connection_socket_descriptor, dummy));
            }
            else
            {
                if(events[i].events & EPOLLHUP){
                    auto client = clients[events[i].data.fd];
                    clients.erase(events[i].data.fd);
                    if(client->currRoom!=NULL){
                        client->currRoom->removeClient(client);
                    }
                    epoll_ctl(epoll_fd,EPOLL_CTL_DEL,client->fd,NULL);
                    close(client->fd);
                    delete client;
                }else handleClient(clients[events[i].data.fd]);
            }
        }
    }

    close(server_socket_descriptor);
    return (0);
}
