#include <iostream>
#include <thread>
#include "HTTPMessage.hpp"
#include "ClientSocket.hpp"
#include "ServerSocket.hpp"

void printIncomingRequest(int port)
{
    ClientSocket client(port);
RES_LOOP:
    client.listenAndAccept();
    int client_status = 1;
    int server_status = 1;
    while (true)
    {
        std::pair<HTTPMessage, int> client_result = client.receive();
        HTTPMessage& client_msg = client_result.first;
        client_status = client_result.second;
        if (client_status <= 0)
        {
            std::cout << "Client disconnected, resetting socket" << std::endl;
            goto RES_LOOP;
        }
        ServerSocket server(80, client_msg.host());
        server.send(client_msg);
        std::pair<HTTPMessage, int> server_result = server.receive();
        HTTPMessage& server_msg = server_result.first;
        server_status = server_result.second;
        client.send(server_msg);
    }
}

int main()
{
//    std::thread th(printIncomingRequest, 8080);
//    th.join();
    printIncomingRequest(8080);
    return 0;
}
