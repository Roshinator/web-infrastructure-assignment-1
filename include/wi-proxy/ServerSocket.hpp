#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cerrno>
#include <netdb.h>
#include <fcntl.h>

#include "HTTPMessage.hpp"

#define RECV_BUFFER_SIZE 16384

using std::string;
using std::cout;
using std::endl;

class ServerSocket
{
    const int addr_len = sizeof(struct sockaddr_in);
    uint8_t RECV_BUFFER[RECV_BUFFER_SIZE];
    
    int sockfd = 0;
    struct sockaddr_in server_addr;
    bool connected = false;
    
public:
    ServerSocket();
    void connectTo(int port, string addr);
    void send(HTTPMessage item);
    bool isConnected();
//    std::pair<HTTPMessage, int> receive();
    std::pair<HTTPMessage, int> receive();
    ~ServerSocket();
};

ServerSocket::ServerSocket()
{
    
}

void ServerSocket::connectTo(int port, string addr)
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
        cout << "Failed to connect to server\n" << inet_ntoa(server_addr.sin_addr) << endl;
        exit(1);
    }
    int flags = fcntl(sockfd, F_GETFL);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    std::cout << "Connection established with server IP: " << inet_ntoa(server_addr.sin_addr) << " and port: " << ntohs(server_addr.sin_port) << std::endl;
    connected = true;
}

bool ServerSocket::isConnected()
{
    return connected;
}

ServerSocket::~ServerSocket()
{
    close(sockfd);
}

void ServerSocket::send(const HTTPMessage item)
{
    cout << "Sending message to server" << endl;
    std::string s = item.to_string();
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

//std::pair<HTTPMessage, int> ServerSocket::receive()
//{
//    HTTPMessage msg;
//    std::string s;
//    ssize_t status;
//    int body_len_read = 0;
//    while ((status = recv(sockfd, RECV_BUFFER, RECV_BUFFER_SIZE, 0)) > 0)
//    {
//        s.append((char*)RECV_BUFFER);
//        body_len_read += msg.parse(s);
//        if (body_len_read >= msg.bodyLen())
//        {
//            break;
//        }
//    }
//    if (status < 0)
//    {
//        std::cout << "Error " << errno << ": " << strerror(errno) << std::endl;
//    }
//    else if (status > 0)
//    {
//        msg.parseBody(s);
//    }
//    return std::pair<HTTPMessage, int>(msg, status);
//}

std::pair<HTTPMessage, int> ServerSocket::receive()
{
    HTTPMessage msg;
    std::string s;
    ssize_t status;
    int body_len_read = 0;
    while ((status = recv(sockfd, RECV_BUFFER, RECV_BUFFER_SIZE, 0)) > 0)
    {
        cout << "Receiving message from server" << endl;
        s.append((char*)RECV_BUFFER, status);
        body_len_read += msg.parse(s);
        msg.parseBody(s);
        std::fill_n(RECV_BUFFER, RECV_BUFFER_SIZE, 0);
//        if (body_len_read >= msg.bodyLen())
//        {
//            cout << "Finished reading server message" << endl;
//            break;
//        }
    }
    msg.setRawText(s);
    return std::pair<HTTPMessage, int>(msg, status);
}

