#ifndef SERVER_H
#define SERVER_H

#include <SFML/Network.hpp>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <mutex>
#include <thread>
#include <utility>

struct ServerPlayer
{
    unsigned int id;
    std::string name;
    float x, y, angle;
    int health;
    std::vector<std::pair<float, float>> bullets;

    ServerPlayer() : id(0), name(""), x(0.0f), y(0.0f), health(100), angle(0.0f) {}

    ServerPlayer(unsigned int id, const std::string &name, float x, float y, float angle, int health)
        : id(id), name(name), x(x), y(y), angle(angle), health(health) {}
};

class ServerManager
{

public:
    ServerManager() {};
    std::unordered_map<unsigned int, ServerPlayer> players;
    std::unordered_map<unsigned int, sf::IpAddress> playerIps;
    std::unordered_map<unsigned int, unsigned short> playerPorts;
    std::mutex playersMutex;
    unsigned int nextPlayerId = 1;

    sf::UdpSocket socket;

    void broadcastPlayerListToSingleClient(sf::IpAddress ip,
                                           unsigned short port);
    void handlePlayerJoin(sf::Packet packet, sf::IpAddress senderIp, unsigned short senderPort);
    void handlePlayerUpdate(sf::Packet packet, sf::IpAddress senderIp, unsigned short senderPort);
    void handlePlayerHit(sf::Packet packet, sf::IpAddress senderIp, unsigned short senderPort);
    void handlePlayerDisconnect(sf::Packet packet, sf::IpAddress senderIp, unsigned short senderPort);

private:
    sf::IpAddress serverIp;
    unsigned short serverPort;
};

#endif