#include <iostream>
#include <thread>
#include "TCPSocket.hpp"

void printIncomingRequest(int port)
{
    TCPSocket<std::string> sock(port);
    sock.listenAndAccept();
    int res_status = 1;
    while (true)
    {
        std::tuple<std::string, int> result = sock.receive();
        std::string& res_str = std::get<0>(result);
        res_status = std::get<1>(result);
        if (!res_str.empty())
        {
            std::cout << res_str << std::endl;
        }
    }
}

int main()
{
    std::thread th(printIncomingRequest, 8081);
    th.join();
    return 0;
}
