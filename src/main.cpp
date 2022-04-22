#include "ClientSocket.hpp"
#include "HTTPMessage.hpp"
#include "ServerSocket.hpp"
#include <cerrno>
#include <chrono>
#include <iostream>
#include <thread>

using std::cout;
using std::endl;
using std::string;

void runProxy(int port)
{
    ClientSocket client(port);
    ServerSocket server;
    bool client_would_block;
    bool server_would_block;

    // Tag used to reset the listen loop sequence
RES_LOOP:
    client.listenAndAccept();
    int client_status = 1;
    int server_status = 1;
    while (true)
    {
        // Poll read client message
        errno = 0;
        std::pair<HTTPMessage, int> client_result = client.receive();
        HTTPMessage& client_msg = client_result.first;
        client_status = client_result.second;
        client_would_block = errno == EWOULDBLOCK;
        errno = 0;
        // If an error occurred, reset connection
        if (client_status <= 0 && !client_would_block)
        {
            std::cout << "Client disconnected, resetting socket" << std::endl;
            goto RES_LOOP;
        }
        // If message has content, check if server connection needs to be updated
        if (!client_msg.isEmpty())
        {
            cout << "Successful connection" << endl;
            if (server.connectTo(80, client_msg.host()) == false)
            {
                client.send(HTTPMessage("HTTP/1.1 400 Bad Request\r\n\r\n"));
                goto RES_LOOP;
            }
        }
        // If we have a server connection, send packet and poll receive
        if (server.isConnected())
        {
            if (!client_msg.isEmpty())
            {
                server.send(client_msg);
            }
            errno = 0;
            std::pair<HTTPMessage, int> server_result = server.receive();
            HTTPMessage& server_msg = server_result.first;
            server_status = server_result.second;
            server_would_block = errno == EWOULDBLOCK;
            errno = 0;
            if (!server_msg.isEmpty()) // Forward back to client
            {
                cout << server_msg.to_string() << endl;
                client.send(server_msg.to_string());
            }
        }
    }
}

int main()
{
    runProxy(8080);
    return 0;
}
