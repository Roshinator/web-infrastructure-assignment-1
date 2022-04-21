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

class TCPSocket
{
    const int addr_len = sizeof(struct sockaddr_in);
    uint8_t RECV_BUFFER[RECV_BUFFER_SIZE];
    
    int listen_sockfd;
    int client_sockfd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    
public:
    TCPSocket(int port);
    void listenAndAccept();
    void send(HTTPMessage item);
    std::pair<HTTPMessage, int> receive();
    ~TCPSocket();
};

TCPSocket::TCPSocket(int port)
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

TCPSocket::~TCPSocket()
{
    close(listen_sockfd);
    close(client_sockfd);
}

void TCPSocket::listenAndAccept()
{
    if (listen(listen_sockfd, 1) < 0)
    {
        std::cout << "Listen failed" << std::endl;
        exit(1);
    }
    client_sockfd = accept(listen_sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
    if (client_sockfd < 0)
    {
        std::cout << "Failed to accept client" << std::endl;
        return 1;
    }
    std::cout << "Connection established with client IP: " << inet_ntoa(client_addr.sin_addr) << " and port: " << ntohs(client_addr.sin_port) << std::endl;
}

void TCPSocket::send(const HTTPMessage item)
{
    std::string s = item.to_string();
    ::send(client_sockfd, s.data(), sizeof(s.data()), 0);
}

std::pair<HTTPMessage, int> TCPSocket::receive()
{
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
            break;
        }
    }
    if (status < 0)
    {
        std::cout << "Error " << errno << ": " << strerror(errno) << std::endl;
    }
    else if (status > 0)
    {
        msg.parseBody(s);
    }
    return std::pair<HTTPMessage, int>(msg, status);
}

