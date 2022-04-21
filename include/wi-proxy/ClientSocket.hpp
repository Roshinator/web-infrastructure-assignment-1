#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cerrno>

#include "HTTPMessage.hpp"

#define RECV_BUFFER_SIZE 80000

using std::string;
using std::cout;
using std::endl;

class ClientSocket
{
    const int addr_len = sizeof(struct sockaddr_in);
    uint8_t RECV_BUFFER[RECV_BUFFER_SIZE];
    
    int listen_sockfd;
    int client_sockfd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    
public:
    ClientSocket(int port);
    void listenAndAccept();
    void send(HTTPMessage item);
    std::pair<HTTPMessage, int> receive();
    ~ClientSocket();
};

ClientSocket::ClientSocket(int port)
{
    listen_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int result = bind(listen_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (result < 0)
    {
        std::cout << "Failed to bind socket" << std::endl;
        exit(1);
    }
}

ClientSocket::~ClientSocket()
{
    close(listen_sockfd);
    close(client_sockfd);
}

void ClientSocket::listenAndAccept()
{
    cout << "Listening for Client" << endl;
    if (listen(listen_sockfd, 1) < 0)
    {
        std::cout << "Listen failed" << std::endl;
        exit(1);
    }
    cout << "Client found, accepting connection" << endl;
    client_sockfd = accept(listen_sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
    if (client_sockfd < 0)
    {
        std::cout << "Failed to accept client" << std::endl;
        exit(1);
    }
    std::cout << "Connection established with client IP: " << inet_ntoa(client_addr.sin_addr) << " and port: " << ntohs(client_addr.sin_port) << std::endl;
}

void ClientSocket::send(const HTTPMessage item)
{
    cout << "Sending message to client" << endl;
    std::string s = item.to_string();
    ::send(client_sockfd, s.data(), sizeof(s.data()), 0);
}

std::pair<HTTPMessage, int> ClientSocket::receive()
{
    cout << "Receiving mesage from client" << endl;
    HTTPMessage msg;
    std::string s;
    ssize_t status;
    int body_len_read = 0;
    while ((status = recv(client_sockfd, RECV_BUFFER, RECV_BUFFER_SIZE, 0)) > 0)
    {
        s.append((char*)RECV_BUFFER);
        body_len_read += msg.parse(s);
        if (body_len_read >= msg.bodyLen())
        {
            cout << "Finished reading client message" << endl;
            break;
        }
    }
    if (status < 0)
    {
        std::cout << "Error " << errno << ": " << strerror(errno) << std::endl;
    }
    else if (status > 0)
    {
        cout << "Parsing message body" << endl;
        msg.parseBody(s);
    }
    else
    {
        cout << "Client disconnected, transmission ended" << endl;
    }
    return std::pair<HTTPMessage, int>(msg, status);
}

