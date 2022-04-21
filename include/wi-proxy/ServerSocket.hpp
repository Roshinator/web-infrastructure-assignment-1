#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cerrno>
#include <netdb.h>

#include "HTTPMessage.hpp"

#define RECV_BUFFER_SIZE 80000

using std::string;
using std::cout;
using std::endl;

class ServerSocket
{
    const int addr_len = sizeof(struct sockaddr_in);
    uint8_t RECV_BUFFER[RECV_BUFFER_SIZE];
    
    int sockfd;
    struct sockaddr_in server_addr;
    
public:
    ServerSocket(int port, string addr);
    void send(HTTPMessage item);
    std::pair<HTTPMessage, int> receive();
    ~ServerSocket();
};

ServerSocket::ServerSocket(int port, string addr)
{
    cout << "Opening server socket for host: " << addr << endl;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    cout << "Resolving name from DNS" << endl;
    struct hostent* host = gethostbyname(addr.data());
    if (host->h_length < 0)
    {
        cout << "DNS Resolution failed" << endl;
        exit(1);
    }
    server_addr.sin_addr = *((struct in_addr*)host->h_addr_list[0]);
    cout << "Connecting to server" << endl;
    int conn = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
    if (conn < 0)
    {
        cout << "Failed to connect to server %d.\n" << server_addr.sin_addr.s_addr << endl;
        exit(1);
    }
    std::cout << "Connection established with server IP: " << inet_ntoa(server_addr.sin_addr) << " and port: " << ntohs(server_addr.sin_port) << std::endl;
}

ServerSocket::~ServerSocket()
{
    close(sockfd);
}

void ServerSocket::send(const HTTPMessage item)
{
    std::string s = item.to_string();
    ::send(sockfd, s.data(), sizeof(s.data()), 0);
}

std::pair<HTTPMessage, int> ServerSocket::receive()
{
    HTTPMessage msg;
    std::string s;
    ssize_t status;
    int body_len_read = 0;
    while ((status = recv(sockfd, RECV_BUFFER, RECV_BUFFER_SIZE, 0)) > 0)
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

