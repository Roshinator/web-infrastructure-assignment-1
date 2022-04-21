#include <iostream>
#include <thread>
#include "HTTPMessage.hpp"
#include "TCPSocket.hpp"

void printIncomingRequest(int port)
{
    TCPSocket sock(port);
RES_LOOP:
    sock.listenAndAccept();
    int res_status = 1;
    while (true)
    {
        std::pair<HTTPMessage, int> result = sock.receive();
        HTTPMessage& res_msg = result.first;
        res_status = result.second;
        if (res_status <= 0)
        {
            std::cout << "Client disconnected, resetting socket" << std::endl;
            goto RES_LOOP;
        }
        HTTPMessage response;
        sock.send(response);
    }
}

int main()
{
//    std::thread th(printIncomingRequest, 8080);
//    th.join();
    printIncomingRequest(8080);
    return 0;
}
