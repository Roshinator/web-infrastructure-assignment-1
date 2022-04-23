#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include "ClientSocket.hpp"
#include <string>

#include "GlobalItems.hpp"

using std::string;
using std::cout;
using std::endl;

class ClientSocketListener
{
    const int addr_len = sizeof(struct sockaddr_in);
    
    int listen_sockfd = -1;
    struct sockaddr_in server_addr;
    
public:
    ClientSocketListener(int port);
    ClientSocket acceptClient();
    ~ClientSocketListener();
};

ClientSocketListener::ClientSocketListener(int port)
{
    listen_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int result = bind(listen_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    int flags = fcntl(listen_sockfd, F_GETFL);
    fcntl(listen_sockfd, F_SETFL, flags | O_NONBLOCK);
    if (result < 0)
    {
        GFD::threadedCout("Failed to bind client listener socket");
        exit(1);
    }
    GFD::threadedCout("Bound listener, open for connections");
    //    cout << "Listening for Client" << endl;
    if (listen(listen_sockfd, 10) < 0)
    {
        GFD::threadedCout("Listen failed");
        exit(1);
    }
}

ClientSocketListener::~ClientSocketListener()
{
    if (listen_sockfd != 0)
    {
        GFD::fdMutex.lock();
        close(listen_sockfd);
        GFD::fdMutex.unlock();
    }
}

ClientSocket ClientSocketListener::acceptClient()
{
//    cout << "Client found, accepting connection" << endl;
    struct sockaddr_in client_addr;
    GFD::fdMutex.lock();
    int client_sockfd = accept(listen_sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
//    std::cout << "FROM: " << listen_sockfd << " ESTABLISHED CONNECTION WITH FD: " << client_sockfd << std::endl;
    GFD::fdMutex.unlock();
    int flags = fcntl(client_sockfd, F_GETFL);
    fcntl(client_sockfd, F_SETFL, flags | O_NONBLOCK);
//    if (client_sockfd < 0)
//    {
//        std::cout << "Failed to accept client" << std::endl;
//        exit(1);
//    }
    if (client_sockfd > 0)
    {
        GFD::threadedCout("Connection established with client IP: ", inet_ntoa(client_addr.sin_addr), " and port: ", ntohs(client_addr.sin_port));
    }
    return ClientSocket(client_addr, client_sockfd);
}
