#ifndef CLIENTSIDECOMMUNICATIONMANAGER_H
#define CLIENTSIDECOMMUNICATIONMANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <unordered_map>
#include "ServerManager.h"
#include "Player.h"

class ClientSideCommunicationManager
{
public:
    ClientSideCommunicationManager(std::string serverIp, unsigned short serverPort, std::string playerName);

    std::string playerName;
    unsigned int playerId = 0;

    void connectToServer();
    void disconnectFromServer();
    std::unordered_map<unsigned int, ServerPlayer> serverPlayerList;
    std::unordered_map<unsigned int, Player> clientPlayerList;
    void receiveData(Player player);
    void synchronizePlayerList(sf::Packet incomingPacket);
    void sendClientDataToServer(Player player);

private:
    sf::IpAddress serverIp;
    sf::UdpSocket socket;
    unsigned short serverPort;
};

#endif