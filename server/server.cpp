#include <SFML/Network.hpp>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <mutex>
#include "ServerManager.h"

ServerManager serverManager;

int main()
{
    if (serverManager.socket.bind(54001) != sf::Socket::Done)
    {
        std::cerr << "Failed to bind server socket." << std::endl;
        return -1;
    }
    serverManager.socket.setBlocking(false);
    std::cout << "Server is running on port 54001" << std::endl;

    while (true)
    {
        sf::Packet packet;
        sf::IpAddress senderIp;
        unsigned short senderPort;

        if (serverManager.socket.receive(packet, senderIp, senderPort) == sf::Socket::Done)
        {
            std::string message;
            packet >> message;

            if (message == "playerJoin")
            {
                serverManager.handlePlayerJoin(packet, senderIp, senderPort);
            }
            else if (message == "playerUpdate")
            {
                serverManager.handlePlayerUpdate(packet, senderIp, senderPort);
            }
            else if (message == "playerDisconnect")
            {
                serverManager.handlePlayerDisconnect(packet, senderIp, senderPort);
            }
        }
        serverManager.broadcastPlayerListToAllClients();
    }

    return 0;
}