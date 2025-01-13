#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include "ClientSideCommunicationManager.h"
#include "Player.h"
#include "Bullet.h"
#include <json.hpp>
#include <cmath>

ClientSideCommunicationManager::ClientSideCommunicationManager(std::string serverIp, unsigned short serverPort, std::string playerName)
{
    this->serverIp = serverIp;
    this->serverPort = serverPort;
    socket.setBlocking(false);

    this->playerName = playerName;
}

void ClientSideCommunicationManager::connectToServer()
{
    // Wyślij dane startowe
    std::cout << "Sending join request to server..." << std::endl;
    sf::Packet packet;
    packet << "playerJoin" << playerName;
    if (socket.send(packet, serverIp, serverPort) != sf::Socket::Done)
    {
        std::cerr << "Failed to send initial packet to server." << std::endl;
        std::exit(0);
    }
    else
    {
        std::cout << "Join request sent." << std::endl;
    }

    // Odbierz odpowiedź od serwera
    sf::Packet responsePacket;
    sf::IpAddress senderIp;
    unsigned short senderPort;
    sf::sleep(sf::milliseconds(100)); // Dodajemy opóźnienie

    sf::Socket::Status status = socket.receive(responsePacket, senderIp, senderPort);
    if (status == sf::Socket::Done)
    {
        responsePacket >> playerId;
        std::cout << "Connected to server. Assigned ID: " << playerId << std::endl;
    }
    else
    {
        std::cerr << "Failed to receive response from server. Status: " << status << std::endl;
        std::exit(0);
    }
}

void ClientSideCommunicationManager::disconnectFromServer()
{
    sf::Packet packet;
    packet << "playerDisconnect" << playerId << playerName;
    if (socket.send(packet, serverIp, serverPort) != sf::Socket::Done)
    {
        std::cerr << "Failed to send disconnect packet to server." << std::endl;
        std::exit(0);
    }
}

void ClientSideCommunicationManager::receiveData(Player &player)
{
    sf::Packet incomingPacket;
    sf::IpAddress senderIp;
    unsigned short senderPort;
    if (socket.receive(incomingPacket, senderIp, senderPort) == sf::Socket::Done)
    {
        std::string message;
        incomingPacket >> message;
        if (message == "playerList")
        {
            this->synchronizePlayerList(incomingPacket, player);
        }
    }
}

void ClientSideCommunicationManager::synchronizePlayerList(sf::Packet incomingPacket, Player &player)
{
    unsigned int playersCount;
    incomingPacket >> playersCount;

    std::vector<unsigned int> usedIds;

    for (int i = 0; i < playersCount; i++)
    {
        unsigned int id, bulletCount;
        std::string name;
        float x, y, angle;
        int health;

        incomingPacket >> id >> name >> x >> y >> angle >> health;

        usedIds.push_back(id);

        std::vector<std::pair<float, float>> bullets;
        incomingPacket >> bulletCount;
        for (int j = 0; j < bulletCount; j++)
        {
            float bulletX, bulletY;
            incomingPacket >> bulletX >> bulletY;

            std::pair<float, float> bullet;
            bullet = std::make_pair(bulletX, bulletY);
            bullets.push_back(bullet);
        }

        if (id != playerId)
        {
            auto it = clientPlayerList.find(id);
            if (it != clientPlayerList.end())
            {
                // Update existing player record
                it->second.setPosition(x, y);
                if (health == 0)
                {
                    it->second.kill();
                }
                else
                {
                    it->second.setHealth(health);
                }
                it->second.setAngle(angle);

                it->second.bullets.clear(); // Clear bullets to update
                for (const auto &bullet : bullets)
                {
                    it->second.addBullet(bullet.first, bullet.second, 0.0f); // Default angle if not available
                }
            }
            else
            {
                // Add a new player to the client list
                Player newPlayer(
                    id,
                    name,
                    x,
                    y,
                    30.0f);
                newPlayer.setHealth(health);
                newPlayer.setAngle(angle);
                for (const auto &bullet : bullets)
                {
                    newPlayer.addBullet(bullet.first, bullet.second, 0.0f); // Default angle if not available
                }
                clientPlayerList.emplace(id, newPlayer);
            }
        }
        else
        {

            // incomingPacket >> idFrom >> damage;
            if (health == 0)
            {
                player.kill();
            }
            else
            {
                player.setHealth(health);
            }
        }
    }

    // Usun graczy ktorych juz nie ma na serwerze
    // std::cout << "ClientPlayerList: " << clientPlayerList.size() << " usedIds: " << usedIds.size() << std::endl;
    if (clientPlayerList.size() > usedIds.size() - 1)
    {
        std::cout << "Jest roznica" << std::endl;
        for (auto it = clientPlayerList.begin(); it != clientPlayerList.end();)
        {
            if (std::find(usedIds.begin(), usedIds.end(), it->first) == usedIds.end())
            {
                std::cout << "Usuniety gracz" << std::endl;
                it = clientPlayerList.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}

void ClientSideCommunicationManager::sendClientDataToServer(Player player)
{
    sf::Packet updatePacket;
    updatePacket << "playerUpdate" << player.id << player.getPosition().x << player.getPosition().y << player.getHealth() << player.getAngle();

    updatePacket << static_cast<unsigned int>(player.bullets.size());
    for (const auto &bullet : player.bullets)
    {
        updatePacket << bullet.pos.x << bullet.pos.y;
    }

    if (socket.send(updatePacket, serverIp, serverPort) != sf::Socket::Done)
    {
        std::cerr << "Failed to send player update." << std::endl;
    }
}

void ClientSideCommunicationManager::sendHitMessageToServer(unsigned int fromId, unsigned int toId)
{
    sf::Packet hitPacket;
    int damage = 5;
    hitPacket << "playerHit" << fromId << toId << damage;

    if (socket.send(hitPacket, serverIp, serverPort) != sf::Socket::Done)
    {
        std::cerr << "Failed to send player update." << std::endl;
    }
}