#pragma once

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "HTTPMessage.hpp"

#define RECV_BUFFER_SIZE 16384

using std::cout;
using std::endl;
using std::string;

/// Wrapper for a to server HTTP TCP socket
class ServerSocket
{
    const int addr_len = sizeof(struct sockaddr_in);
    uint8_t RECV_BUFFER[RECV_BUFFER_SIZE];

    int sockfd = 0;
    struct sockaddr_in server_addr;
    bool connected = false;

  public:
    ServerSocket(){};
    bool connectTo(int port, string addr);
    void send(const HTTPMessage& item);
    bool isConnected();
    std::pair<HTTPMessage, int> receive();
    ~ServerSocket();
};

/// Set server connection
/// @param port connection port
/// @param addr address to connect to
bool ServerSocket::connectTo(int port, string addr)
{
    if (sockfd != 0)
    {
        close(sockfd);
        sockfd = 0;
    }
    cout << "Opening server socket for host: " << addr << endl;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    cout << "Resolving name from DNS" << endl;
    struct hostent* host = gethostbyname(addr.data());
    if (host == NULL || host->h_length < 0)
    {
        cout << "DNS Resolution failed, ignoring request" << endl;
        close(sockfd);
        sockfd = 0;
        return false;
    }
    server_addr.sin_addr = *((struct in_addr*)host->h_addr_list[0]);
    cout << "Connecting to server" << endl;
    int conn = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
    if (conn < 0)
    {
        cout << "Failed to connect to server\n" << inet_ntoa(server_addr.sin_addr) << endl;
        close(sockfd);
        sockfd = 0;
        return false;
    }
    // Set non-blocking on the socket
    int flags = fcntl(sockfd, F_GETFL);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    std::cout << "Connection established with server IP: " << inet_ntoa(server_addr.sin_addr)
              << " and port: " << ntohs(server_addr.sin_port) << std::endl;
    connected = true;
    return true;
}

/// Returns true if socket is connected
bool ServerSocket::isConnected()
{
    return connected;
}

ServerSocket::~ServerSocket()
{
    close(sockfd);
}

/// Send a message over the socket
/// @param item HTTP message to send
void ServerSocket::send(const HTTPMessage& item)
{
    cout << "Sending message to server" << endl;
    const std::string& s = item.to_string();
    int len;
    while (true)
    {
        errno = 0;
        len = ::send(sockfd, s.data(), s.length(), 0);
        if (len > 0 || errno != EWOULDBLOCK)
        {
            break;
        }
    }
    cout << "Sent " << len << " bytes from " << s.length() << " sized packet to server" << endl;
}

/// Receives a message and returns the message and error code from the recv call
std::pair<HTTPMessage, int> ServerSocket::receive()
{
    std::string s;
    ssize_t status;
    while ((status = recv(sockfd, RECV_BUFFER, RECV_BUFFER_SIZE, 0)) > 0)
    {
        cout << "Receiving message from server" << endl;
        s.append((char*)RECV_BUFFER, status);
        std::fill_n(RECV_BUFFER, RECV_BUFFER_SIZE, 0);
    }
    HTTPMessage msg(s);
    return std::pair<HTTPMessage, int>(msg, status);
}
