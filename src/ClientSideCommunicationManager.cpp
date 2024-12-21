#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include "ClientSideCommunicationManager.h"
#include "Player.h"
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

void ClientSideCommunicationManager::synchronizeServerPlayerList()
{
    sf::Packet incomingPacket;
    sf::IpAddress senderIp;
    unsigned short senderPort;

    if (socket.receive(incomingPacket, senderIp, senderPort) == sf::Socket::Done)
    {
        std::string jsonString;
        incomingPacket >> jsonString;

        try
        {
            // Parsowanie JSON
            auto jsonData = nlohmann::json::parse(jsonString);
            for (auto &[idString, data] : jsonData.items())
            {
                unsigned int id = static_cast<unsigned int>(std::stoi(idString));
                auto it = serverPlayerList.find(id);
                if (it != serverPlayerList.end())
                {
                    it->second.x = data["x"];
                    it->second.y = data["y"];
                    it->second.angle = data["angle"];
                    it->second.health = data["health"];

                    // Mozliwe ze update pociskow wymagac bedzie optymalizacji
                    it->second.bullets.clear();
                    for (const auto &bullet : data["bullets"])
                    {
                        it->second.bullets.emplace_back(bullet["x"], bullet["y"]);
                    }
                }
                else
                {
                    ServerPlayer newPlayer(
                        id,
                        data["name"],
                        data["x"],
                        data["y"],
                        data["angle"],
                        data["health"]);
                    for (const auto &bullet : data["bullets"])
                    {
                        newPlayer.bullets.emplace_back(bullet["x"], bullet["y"]);
                    }
                    serverPlayerList.emplace(id, newPlayer);
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
        }
    }
}

void ClientSideCommunicationManager::synchronizeClientPlayerList()
{
    for (const auto &[id, serverPlayer] : serverPlayerList)
    {
        if (id == playerId)
        {
            // Skip the player's own data
            continue;
        }

        auto it = clientPlayerList.find(id);
        if (it != clientPlayerList.end())
        {
            // Update existing player record
            it->second.setPosition(serverPlayer.x, serverPlayer.y);
            it->second.setHealth(serverPlayer.health);
            it->second.setAngle(serverPlayer.angle);

            it->second.bullets.clear(); // Clear bullets to update
            for (const auto &bullet : serverPlayer.bullets)
            {
                it->second.addBullet(bullet.first, bullet.second, 0.0f); // Default angle if not available
            }
        }
        else
        {
            // Add a new player to the client list
            Player newPlayer(
                id,
                serverPlayer.name,
                serverPlayer.x,
                serverPlayer.y,
                30.0f);
            newPlayer.setHealth(serverPlayer.health);
            newPlayer.setAngle(serverPlayer.angle);
            for (const auto &bullet : serverPlayer.bullets)
            {
                newPlayer.addBullet(bullet.first, bullet.second, 0.0f); // Default angle if not available
            }
            clientPlayerList.emplace(id, newPlayer);
        }
    }

    for (const auto &[id, clientPlayer] : clientPlayerList)
    {
        auto it = serverPlayerList.find(id);
        if (it == serverPlayerList.end())
        {
            clientPlayerList.erase(id);
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
