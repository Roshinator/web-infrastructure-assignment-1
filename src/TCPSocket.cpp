#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include "TCPSocket.hpp"

using namespace std;

template<typename T>
TCPSocket<T>::TCPSocket(int port)
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        cout << "Failed to bind socket" << endl;
        exit(1);
    }
}

template<typename T>
TCPSocket<T>::~TCPSocket()
{
    close(sockfd);
}

template<typename T>
void TCPSocket<T>::listen_and_accept()
{
    if (listen(sockfd, 1) < 0)
    {
        cout << "Listen failed" << endl;
        exit(1);
    }
    int client_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
    if (client_sockfd < 0)
    {
        cout << "Failed to accept client" << endl;
        return 1;
    }
    cout << "Connection established with client IP: " << inet_ntoa(client_addr.sin_addr) << " and port: " << ntohs(client_addr.sin_port) << endl;
}

template <typename T>
void TCPSocket<T>::send(const T item)
{
    string s = item.to_string();
    write(sockfd, s.data(), sizeof(s.data()));
}

template <typename T>
T TCPSocket<T>::receive()
{
    string s;
    size_t readlen;
    while ((readlen = read(sockfd, RECV_BUFFER, 1024)) > 0)
    {
        RECV_BUFFER[readlen] = '\0';
        s.append(RECV_BUFFER);
    }
    return T(s);
}
