#include "ClientSocket.hpp"
#include "HTTPMessage.hpp"
#include "ServerSocket.hpp"
#include <cerrno>
#include <chrono>
#include <iostream>
#include <thread>
#include "ClientSocketListener.hpp"
#include <utility>
#include <string>
#include "GlobalItems.hpp"
#include <chrono>

using namespace std::chrono_literals;

using std::cout;
using std::endl;
using std::string;

constexpr uint64_t spin_wait_threshold = 500000;

void threadRunner(ClientSocket client)
{
    ServerSocket server;
    bool client_would_block, server_would_block;
    int client_status = 1, server_status = 1;
    uint64_t spin_wait_count = 0;
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
            GFD::threadedCout("Client disconnected, resetting socket");
            
            if (client_status < 0)
            {
                GFD::threadedCout(strerror(errno));
            }
            errno = 0;
            break;
        }
        errno = 0;
        // If message has content, check if server connection needs to be updated
        if (!client_msg.isEmpty())
        {
            GFD::threadedCout("Successful connection");
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
                spin_wait_count = 0;
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
                spin_wait_count = 0;
            }
        }
        spin_wait_count++;
        //Slow down polling if nothing has come through in a while to ease cpu usage
        if (spin_wait_count > spin_wait_threshold)
        {
            std::this_thread::sleep_for(1000ms);
        }
    }
    client.disconnect();
}

void runProxy(int port)
{
    ClientSocketListener listener(port);
    uint64_t spin_wait_count = 0;
    while (true)
    {
        ClientSocket sock = listener.acceptClient();
        if (sock.getFD() >= 0)
        {
            spin_wait_count = 0;
            std::thread th(threadRunner, std::move(sock));
            th.detach();
        }
        spin_wait_count++;
        //Slow down polling if nothing has come through in a while to ease cpu usage
        if (spin_wait_count > spin_wait_threshold)
        {
            std::this_thread::sleep_for(1000ms);
        }
    }
   
}

int main()
{
    runProxy(8080);
    return 0;
}
