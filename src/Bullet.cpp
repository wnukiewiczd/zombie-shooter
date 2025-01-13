#include <SFML/Graphics.hpp>
#include "Bullet.h"
#include "Player.h"
#include "ClientSideCommunicationManager.h"
#include <iostream>

#include <cmath>

Bullet::Bullet(float startX, float startY, float angle)
{
    shape.setRadius(BULLET_RADIUS);
    shape.setFillColor(sf::Color::Magenta);
    shape.setOrigin(shape.getRadius(), shape.getRadius());
    shape.setPosition(startX, startY);

    float radians = angle * M_PI / 180.f;
    velocity.x = std::cos(radians) * SPEED;
    velocity.y = std::sin(radians) * SPEED;
}

void Bullet::update()
{
    shape.move(velocity);
    pos = shape.getPosition();
}

void Bullet::draw(sf::RenderWindow &window)
{
    window.draw(shape);
}

void Bullet::handleHitting(ClientSideCommunicationManager &communicationManager)
{
    for (auto &[id, pl] : communicationManager.clientPlayerList)
    {
        sf::Vector2f bulletPos = shape.getPosition();
        float bulletRadius = shape.getRadius();
        sf::Vector2f targetPos = pl.characterShape.getPosition();
        float targetRadius = pl.characterShape.getRadius();

        float distance = std::sqrt(std::pow(bulletPos.x - targetPos.x, 2) + std::pow(bulletPos.y - targetPos.y, 2));
        bool bulletHitPlayer = distance <= (bulletRadius + targetRadius);

        // std::cout << "Cout test" << std::endl;
        if (bulletHitPlayer)
        {
            communicationManager.sendHitMessageToServer(communicationManager.playerId, pl.id);
            // communicationManager.receiveData(from);
        }
    }
}