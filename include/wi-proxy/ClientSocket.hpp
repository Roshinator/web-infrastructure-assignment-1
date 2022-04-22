#pragma once

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "HTTPMessage.hpp"

using std::cout;
using std::endl;
using std::string;

/// Wrapper for a to client HTTP TCP socket
class ClientSocket
{
    static constexpr uint16_t RECV_BUFFER_SIZE = 16384;
    
    const int addr_len = sizeof(struct sockaddr_in);
    uint8_t RECV_BUFFER[RECV_BUFFER_SIZE];

    int listen_sockfd = 0;
    int client_sockfd = 0;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

  public:
    ClientSocket(int port);
    void listenAndAccept();
    void send(const HTTPMessage& item);
    std::pair<HTTPMessage, int> receive();
    ~ClientSocket();
};

/// Constructs a client socket
/// @param port port to listen on
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
    if (client_sockfd != 0)
    {
        close(client_sockfd);
    }
    if (listen_sockfd != 0)
    {
        close(listen_sockfd);
    }
}

/// Listens and accepts one client
void ClientSocket::listenAndAccept()
{
    if (client_sockfd != 0)
    {
        close(client_sockfd);
        client_sockfd = 0;
    }
    cout << "Listening for Client" << endl;
    if (listen(listen_sockfd, 1) < 0)
    {
        std::cout << "Listen failed" << std::endl;
        exit(1);
    }
    cout << "Client found, accepting connection" << endl;
    client_sockfd = accept(listen_sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
    // Set non-blocking
    int flags = fcntl(client_sockfd, F_GETFL);
    fcntl(client_sockfd, F_SETFL, flags | O_NONBLOCK);
    if (client_sockfd < 0)
    {
        std::cout << "Failed to accept client" << std::endl;
        exit(1);
    }
    std::cout << "Connection established with client IP: " << inet_ntoa(client_addr.sin_addr)
              << " and port: " << ntohs(client_addr.sin_port) << std::endl;
}

/// Sends a message to the client
/// @param item message to send
void ClientSocket::send(const HTTPMessage& item)
{
    const string& s = item.to_string();
    cout << "Sending message to client" << endl;
    int len;
    while (true)
    {
        errno = 0;
        len = ::send(client_sockfd, s.data(), s.length(), 0);
        if (len > 0 || errno != EWOULDBLOCK)
        {
            break;
        }
    }
    cout << "Sent " << len << " bytes from " << s.length() << " sized packet to client" << endl;
}

/// Receives a message and returns the message and error code from the recv call
std::pair<HTTPMessage, int> ClientSocket::receive()
{
    std::string s;
    ssize_t status;
    while ((status = recv(client_sockfd, RECV_BUFFER, RECV_BUFFER_SIZE, 0)) > 0)
    {
        cout << "Receiving mesage from client" << endl;
        s.append((char*)RECV_BUFFER, status);
        std::fill_n(RECV_BUFFER, RECV_BUFFER_SIZE, 0);
    }
    HTTPMessage msg(s);
    return std::pair<HTTPMessage, int>(msg, status);
}
