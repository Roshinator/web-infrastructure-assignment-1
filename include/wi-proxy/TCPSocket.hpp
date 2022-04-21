#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cerrno>

#define RECV_BUFFER_SIZE 1

template <typename T>
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
    void send(T item);
    std::tuple<T, int> receive();
    ~TCPSocket();
};

template<typename T>
TCPSocket<T>::TCPSocket(int port)
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

template<typename T>
TCPSocket<T>::~TCPSocket()
{
    close(listen_sockfd);
    close(client_sockfd);
}

template<typename T>
void TCPSocket<T>::listenAndAccept()
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

template <typename T>
void TCPSocket<T>::send(const T item)
{
    std::string s;
    s << item;
    send(client_sockfd, s.data(), sizeof(s.data()), 0);
}

template <typename T>
std::tuple<T, int> TCPSocket<T>::receive()
{
    std::string s;
    ssize_t status;
    while ((status = recv(client_sockfd, RECV_BUFFER, RECV_BUFFER_SIZE, 0)) > 0)
    {
        s.append((char*)RECV_BUFFER);
        if (std::find(RECV_BUFFER, RECV_BUFFER + RECV_BUFFER_SIZE, 0) < RECV_BUFFER + RECV_BUFFER_SIZE) //If the string is finished, break receiving
        {
            break;
        }
    }
    if (status < 0)
    {
        std::cout << "Error " << errno << ": " << strerror(errno) << std::endl;
    }
    return std::tuple<T, int>(s, status);
}

