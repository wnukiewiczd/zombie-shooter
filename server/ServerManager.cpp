#include <SFML/Network.hpp>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <mutex>
#include "ServerManager.h"
#include <json.hpp>

ServerManager::ServerManager()
{
}

void ServerManager::broadcastPlayerListToSingleClient(sf::IpAddress ip,
                                                      unsigned short port)
{
    sf::Packet packet;
    packet << static_cast<unsigned int>(players.size());
    for (const auto &[id, player] : players)
    {
        packet << id << player.name << player.x << player.y << player.angle << player.health;
        packet << static_cast<unsigned int>(player.bullets.size());
        for (const auto &bullet : player.bullets)
        {
            packet << bullet.first << bullet.second;
        }
    }

    if (socket.send(packet, ip, port) != sf::Socket::Done)
    {
        std::cerr << "Failed to send update to ip " << ip << std::endl;
    }
}

void ServerManager::broadcastPlayerListToAllClients()
{
    for (const auto &[id, player] : players)
    {
        sf::Packet packet;
        packet << static_cast<unsigned int>(players.size());
        for (const auto &[id, player] : players)
        {
            packet << id << player.name << player.x << player.y << player.angle << player.health;
            packet << static_cast<unsigned int>(player.bullets.size());
            for (const auto &bullet : player.bullets)
            {
                packet << bullet.first << bullet.second;
            }
        }

        if (socket.send(packet, playerIps[id], playerPorts[id]) != sf::Socket::Done)
        {
            std::cerr << "Failed to send update to player " << player.id << std::endl;
        }
        else
        {
            if (id == 1)
                std::cout << player.x << std::endl; // To jest BAAARDZO DOKLADNE
        }
    }
}

void ServerManager::handlePlayerJoin(sf::Packet packet, sf::IpAddress senderIp, unsigned short senderPort)
{
    std::string name;
    packet >> name;

    std::lock_guard<std::mutex> lock(playersMutex);
    ServerPlayer newPlayer(nextPlayerId++, name, 0.0f, 0.0f, 0.0f, 100);
    players[newPlayer.id] = newPlayer;
    playerIps[newPlayer.id] = senderIp;
    playerPorts[newPlayer.id] = senderPort;

    sf::Packet response;
    response << newPlayer.id;
    if (socket.send(response, senderIp, senderPort) != sf::Socket::Done)
    {
        std::cerr << "Failed to send join confirmation to " << name << std::endl;
    }

    std::cout << "Player " << name << " joined with ID " << newPlayer.id << std::endl;
}

void ServerManager::handlePlayerDisconnect(sf::Packet packet, sf::IpAddress senderIp, unsigned short senderPort)
{
    unsigned int id;
    std::string name;
    packet >> id;

    std::lock_guard<std::mutex> lock(playersMutex);
    players.erase(id);
    playerIps.erase(id);
    playerPorts.erase(id);
    std::cout << "Player \" " << name << "\" (" << id << ") disconnected." << std::endl;

    if (players.empty())
    {
        std::cout << "No players left. Server shutting down." << std::endl;
        std::exit(0);
    }
}

void ServerManager::handlePlayerUpdate(sf::Packet packet, sf::IpAddress senderIp, unsigned short senderPort)
{
    unsigned int id;
    float x, y, angle;
    int health;
    packet >> id >> x >> y >> health >> angle;

    unsigned int bulletCount;
    packet >> bulletCount;

    std::vector<std::pair<float, float>> bullets;

    for (unsigned int i = 0; i < bulletCount; i++)
    {
        std::pair<float, float> bullet;
        float bulletX, bulletY;
        packet >> bulletX >> bulletY;
        bullet = std::make_pair(bulletX, bulletY);
        bullets.push_back(bullet);
    }

    if (players.find(id) != players.end())
    {
        players[id].x = x;
        players[id].y = y;
        players[id].health = health;
        players[id].angle = angle;

        players[id].bullets.clear();
        for (const auto &bullet : bullets)
        {
            players[id].bullets.push_back(bullet);
        }
    }
    else
    {
        std::cerr << "Update received for unknown player ID: " << id << std::endl;
    }
}