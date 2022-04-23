#include "ClientSocket.hpp"
#include "HTTPMessage.hpp"
#include "ServerSocket.hpp"
#include <cerrno>
#include <chrono>
#include <iostream>
#include <thread>
#include "ClientSocketListener.hpp"
#include <utility>

using std::cout;
using std::endl;
using std::string;

void threadRunner(ClientSocket client)
{
    ServerSocket server;
    bool client_would_block, server_would_block;
    int client_status = 1, server_status = 1;
    while (true)
    {
        // Poll read client message
        errno = 0;
        std::pair<HTTPMessage, int> client_result = client.receive();
        HTTPMessage& client_msg = client_result.first;
        client_status = client_result.second;
        client_would_block = errno == EWOULDBLOCK;
        // If an error occurred, reset connection
        if (client_status <= 0 && !client_would_block)
        {
            std::cout << "Client disconnected, resetting socket" << std::endl;
            if (client_status < 0)
                std::cout << strerror(errno) << std::endl;
            errno = 0;
            break;
        }
        errno = 0;
        // If message has content, check if server connection needs to be updated
        if (!client_msg.isEmpty())
        {
            cout << "Successful connection" << endl;
            if (server.connectTo(80, client_msg.host()) == false)
            {
                client.send(HTTPMessage("HTTP/1.1 400 Bad Request\r\n\r\n"));
                break;
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
//                cout << server_msg.to_string() << endl;
                client.send(server_msg.to_string());
            }
        }
    }
    client.disconnect();
}

void runProxy(int port)
{
    ClientSocketListener listener(port);
    while (true)
    {
        ClientSocket sock = listener.listenAndAccept();
        if (sock.getFD() >= 0)
        {
            std::thread th(threadRunner, std::move(sock));
            th.detach();
        }
    }
   
}

int main()
{
    runProxy(8080);
    return 0;
}
