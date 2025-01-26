#ifndef CLIENTSIDECOMMUNICATIONMANAGER_H
#define CLIENTSIDECOMMUNICATIONMANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <unordered_map>
#include "ServerManager.h"

class Player;

class ClientSideCommunicationManager
{
public:
    ClientSideCommunicationManager(std::string serverIp, unsigned short serverPort, std::string playerName);

    std::string playerName;
    unsigned int playerId = 0;
    bool gameFinished = false;
    sf::Font font;
    sf::Text finishText;

    void connectToServer();
    void disconnectFromServer();
    std::unordered_map<unsigned int, ServerPlayer> serverPlayerList;
    std::unordered_map<unsigned int, Player> clientPlayerList;
    void receiveData(Player &player);
    void synchronizePlayerList(sf::Packet incomingPacket, Player &player);
    void sendClientDataToServer(Player player);
    void sendHitMessageToServer(unsigned int fromId, unsigned int toId);
    void drawFinishedInfo(sf::RenderWindow &window);

private:
    sf::IpAddress serverIp;
    sf::UdpSocket socket;
    unsigned short serverPort;
};

#endif