#include <SFML/Graphics.hpp>
#include "Bullet.h"

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

void Bullet::handleHitting(ClientSideCommunicationManager communicationManager)
{
}