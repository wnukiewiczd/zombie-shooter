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

void ServerManager::broadcastPlayerListToSingleClient(unsigned int playerId)
{
    std::lock_guard<std::mutex> lock(playersMutex);

    sf::Packet packet;
    std::ostringstream jsonStream;
    jsonStream << "{";
    for (const auto &[id, player] : players)
    {
        // Tutaj powinnismy przeslac pozycje x, pozycje y, tablice pociskow, zycie, kat obrotu
        jsonStream << "\"" << id << "\": {\"name\": \"" << player.name << "\", \"x\": " << player.x << ", \"y\": " << player.y << ", \"health\": " << player.health << "},";
    }
    std::string jsonString = jsonStream.str();
    if (!players.empty())
    {
        jsonString.pop_back(); // Remove trailing comma
    }
    jsonString += "}";

    packet << jsonString;
    if (socket.send(packet, playerIps[playerId], playerPorts[playerId]) != sf::Socket::Done)
    {
        std::cerr << "Failed to send playerlist to player " << playerId << std::endl;
    }
}

void ServerManager::broadcastPlayerListToAllClients()
{
    std::lock_guard<std::mutex> lock(playersMutex);

    sf::Packet packet;
    std::ostringstream jsonStream;
    jsonStream << "{";
    for (const auto &[id, player] : players)
    {
        using json = nlohmann::json;
        json playerJson;
        playerJson["name"] = player.name;
        playerJson["x"] = player.x;
        playerJson["y"] = player.y;
        playerJson["angle"] = player.angle;
        playerJson["health"] = player.health;

        json bulletsArray = json::array();
        for (const auto &bullet : player.bullets)
        {
            json bulletJson;
            bulletJson["x"] = bullet.first;
            bulletJson["y"] = bullet.second;
            bulletsArray.push_back(bulletJson);
        }

        playerJson["bullets"] = bulletsArray;

        jsonStream << "\"" << id << "\": " << playerJson.dump() << ",";

        // jsonStream << "\"" << id << "\": {\"name\": \"" << player.name << "\", \"x\": " << player.x << ", \"y\": " << player.y << ", \"health\": " << player.health << "},";
    }
    std::string jsonString = jsonStream.str();
    if (!players.empty())
    {
        jsonString.pop_back(); // Remove trailing comma
    }
    jsonString += "}";

    packet << jsonString;
    for (const auto &[id, player] : players)
    {
        if (socket.send(packet, playerIps[id], playerPorts[id]) != sf::Socket::Done)
        {
            std::cerr << "Failed to send update to player " << player.id << std::endl;
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
    packet >> id;

    std::lock_guard<std::mutex> lock(playersMutex);
    players.erase(id);
    playerIps.erase(id);
    playerPorts.erase(id);
    std::cout << "Player with ID " << id << " disconnected." << std::endl;

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

    std::lock_guard<std::mutex> lock(playersMutex);
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