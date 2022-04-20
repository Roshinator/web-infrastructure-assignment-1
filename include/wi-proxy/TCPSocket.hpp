#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>

template <typename T>
class TCPSocket
{
    const int addr_len = sizeof(struct sockaddr_in);
    uint8_t RECV_BUFFER[1024];
    
    int sockfd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    
public:
    TCPSocket(int port);
    void listen_and_accept();
    void send(T item);
    T receive();
    
    ~TCPSocket();
        
};
