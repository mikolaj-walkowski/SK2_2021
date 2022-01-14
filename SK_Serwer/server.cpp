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
#define SERVER_PORT 4200
#define QUEUE_SIZE 5

#define MAX_EVENTS 10

struct epoll_event ev, events[MAX_EVENTS];
int nfds, epollfd;

enum client_states
{
    recMsg,
    newChain
};
enum protocol_keywords
{
    bad,
    msg_size,
    msg,
    kick,
    join,
    leave,
    create
};
std::map<std::string, protocol_keywords> enum_resolver = {{"msg_size", msg_size}, {"msg", msg}, {"kick", kick}, {"join", join}, {"leave", leave}, {"create", create}};

protocol_keywords resolve_pkw(std::string s)
{
    try
    {
        return enum_resolver.at(s);
    }
    catch (const std::exception &e)
    {
        return bad;
    }
}
class Room;

class Client
{
public:
    int fd;
    client_states cs;
    std::string ip;
    int msg_size = 0;
    Room *currRoom;
    Client(int _fd, std::string _ip) : fd(_fd), ip(_ip)
    {
        currRoom = NULL;
        cs = newChain;
    }
};

class Room
{
    std::vector<Client *> room_clients;
    std::vector<std::pair<std::string, std::string>> chatLog;

public:
    std::string name;
    std::string owner;

    void roomMngMsg(std::string msg)
    {
        chatLog.push_back(make_pair("serwer", msg));
        for (auto itr : room_clients)
        {
            std::string cmsg = "Serwer message: " + msg;
            if (write(itr->fd, cmsg.c_str(), cmsg.length()) == -1)
            {
                perror("Couldnt write to socket");
            }
        }
    }

    void addClient(Client *cl)
    {
        room_clients.push_back(cl);
        roomMngMsg(cl->ip + " joined\n");
        printf(("Room " + name + " current size: %d\n").c_str(), room_clients.size());
    }
    void removeClient(Client *client)
    {
        for (auto itr = room_clients.begin(); itr != room_clients.end(); itr++)
        {
            if ((*itr) == client)
            {
                room_clients.erase(itr);
                roomMngMsg((*itr)->ip + " left\n");
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
                std::string cmsg = "msg " + cl->ip + ": " + msg;
                if (write(itr->fd, cmsg.c_str(), cmsg.length()) == -1)
                {
                    perror("Couldnt write to socket");
                }
            }
        }
    }
    int kickClient(std::string ip)
    {
        for (auto itr = room_clients.begin(); itr != room_clients.end(); itr++)
            if ((*itr)->ip == ip)
            {
                if (write((*itr)->fd, "kick", 5) == -1)
                {
                    perror("Couldnt write to socket");
                    return 1;
                }
                room_clients.erase(itr);
                roomMngMsg("Kicked " + (*itr)->ip + "\n");
                return 0;
            }
        return 1;
    }
    void sendChatLog(Client *cl)
    {
        for (auto itr : chatLog)
        {
            std::string cmsg = "msg " + itr.first + ": " + itr.second;
            if (write(cl->fd, cmsg.c_str(), cmsg.length()) == -1)
            {
                perror("Couldnt write to socket");
            }
        }
    }
    Room(Client *ow, std::string _name) : owner(ow->ip), name(_name)
    {
        addClient(ow);
        ow->currRoom = this;
    }
};

std::map<int, Client *> clients;
std::vector<Room *> rooms;

void hc_create(Client *client, char *buff)
{
    std::string room_name(buff + 7, strcspn(buff + 7, "\n"));
    for (auto itr : rooms)
    {
        if (itr->name == room_name)
        {
            if (write(client->fd, "failed create", 14) == -1)
            {
                perror("Couldnt write to socket");
            }
            return;
        }
    }

    rooms.push_back(new Room(client, room_name));
    if (write(client->fd, "success", 8) == -1)
    {
        perror("Couldnt write to socket");
    }
}

void hc_join(Client *client, char *buff)
{
    std::string room_name(buff + 5, strcspn(buff + 5, "\n"));
    printf(room_name.c_str());
    printf("\n");

    for (auto itr : rooms)
    {
        if (itr->name == room_name)
        {
            if (write(client->fd, "success", 8) == -1)
            {
                perror("Couldnt write to socket");
            }
            itr->addClient(client);
            client->currRoom = itr;
            itr->sendChatLog(client);

            return;
        }
    }

    if (write(client->fd, "failed join", 12) == -1)
    {
        perror("Couldnt write to socket");
    }
}

void hc_recMsg(Client *client)
{
    char *buff = new char[client->msg_size];
    read(client->fd, buff, client->msg_size);
    std::string token(buff, strcspn(buff, " "));
    if (token == "msg" && client->currRoom != NULL)
    {
        client->currRoom->broadcastMsg(client, std::string(buff + 5));
    }
    client->cs = newChain;
    client->msg_size = 0;
}

void hc_msg_size(Client *client, char *buff)
{
    printf("Was i even here");
    //std::string number(buff + 9, strcspn(buff, "\n"));
    //client->msg_size = stoi(number);
    //printf("Msg_size %d", client->msg_size);
    //client->cs = recMsg;
}

void hc_newChain(Client *client)
{
    char buff[100];
    read(client->fd, buff, 100);
    std::string token(buff, strcspn(buff, " "));
    printf("Token pos:%d data: ", strcspn(buff, " "));
    printf(token.c_str());
    printf("\n");
    printf("PKW %d \n", resolve_pkw(token));
    switch (resolve_pkw(token))
    {
    case kick:
        if (client->currRoom->owner == client->ip)
        {
            if (client->currRoom->kickClient(std::string(buff + 5)) == 0)
            {
                if (write(client->fd, "success kick", 13) == -1)
                {
                    perror("Couldnt write to socket");
                }
            }
            else
            {
                if (write(client->fd, "failed kick", 12) == -1)
                {
                    perror("Couldnt write to socket");
                }
            }
        }
        break;
    case join:
        hc_join(client, buff);
        break;
    case leave:
        if (client->currRoom != NULL)
        {
            client->currRoom->removeClient(client);
            client->currRoom = NULL;
        }
        break;
    case create:
        hc_create(client, buff);
        break;
    case msg_size:
        break;
        // printf("WTf\n");
        // //hc_msg_size(client, buff);
        // break;
    default:
        printf("Bad request\n");
        break;
    }
    printf("Did i leave the switch");
}

void handleClient(Client *client)
{
    switch (client->cs)
    {
    case newChain:
        hc_newChain(client);
        break;
    case recMsg:
        hc_recMsg(client);
        break;
    default:
        break;
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
                char buff[10];
                read(0, buff, 10);
                goto end;
                continue;
            }
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
                struct Client *dummy = new Client(connection_socket_descriptor, inet_ntoa(client_sock.sin_addr));
                clients.insert(std::make_pair(connection_socket_descriptor, dummy));
                continue;
            }

            if (events[i].events & EPOLLHUP)
            {
                printf("Deleteed?\n");
                auto client = clients[events[i].data.fd];
                clients.erase(events[i].data.fd);
                if (client->currRoom != NULL)
                {
                    client->currRoom->removeClient(client);
                }
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL);
                close(client->fd);
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
end:
    for (auto itr = clients.begin(); itr != clients.end(); itr++)
    {
        close(itr->second->fd);
    }
    close(server_socket_descriptor);
    return (0);
}