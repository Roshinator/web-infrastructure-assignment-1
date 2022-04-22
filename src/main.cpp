#include <iostream>
#include <thread>
#include "HTTPMessage.hpp"
#include "ClientSocket.hpp"
#include "ServerSocket.hpp"
#include <cerrno>

using std::string;
using std::cout;
using std::endl;

void printIncomingRequest(int port)
{
    ClientSocket client(port);
    ServerSocket server;
    bool client_would_block;
    bool server_would_block;
RES_LOOP:
    client.listenAndAccept();
    int client_status = 1;
    int server_status = 1;
    while (true)
    {
        errno = 0;
        std::pair<HTTPMessage, int> client_result = client.receive();
        HTTPMessage& client_msg = client_result.first;
        client_status = client_result.second;
        client_would_block = errno == EWOULDBLOCK;
        errno = 0;
        if (client_status <= 0 && !client_would_block)
        {
            std::cout << "Client disconnected, resetting socket" << std::endl;
            goto RES_LOOP;
        }
        if (!client_msg.isEmpty())
        {
            cout << "Successful connection" << endl;
            server.connectTo(80, client_msg.host());
        }
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
            if (!server_msg.isEmpty())
            {
                cout << server_msg.getRawText() << endl;
                client.send(server_msg.getRawText());
            }
        }
    }
}

int main()
{
//    std::thread th(printIncomingRequest, 8080);
//    th.join();
    printIncomingRequest(8080);
    return 0;
}
