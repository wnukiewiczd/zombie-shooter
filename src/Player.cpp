#include "Bullet.h"
#include "Player.h"
#include <cmath>
#include <SFML/Network.hpp>

Player::Player(unsigned int id, std::string name, float x, float y, float radius)
{
    characterShape.setRadius(radius);
    characterShape.setFillColor(sf::Color::Red);
    characterShape.setOrigin(radius, radius);
    characterShape.setPosition(x, y);

    gunShape.setSize(sf::Vector2f(radius / 3, radius * 1.5f));
    gunShape.setFillColor(sf::Color::Black);
    gunShape.setPosition(x + radius - gunShape.getSize().x, y + gunShape.getSize().y);

    gunOffset = sf::Vector2f(characterShape.getRadius() - gunShape.getSize().x, characterShape.getRadius() - gunShape.getSize().x);

    speed = 200.0f;

    this->id = id;
    this->name = name;
}

void Player::update(sf::RenderWindow &window, float deltaTime)
{
    mousePos = sf::Mouse::getPosition(window);
    handleMovement(deltaTime);
    handleRotation(window, deltaTime);
    handleShooting();
    updateBullets(window);
}

void Player::updateBullets(sf::RenderWindow &window)
{
    for (auto &bullet : bullets)
    {
        bullet.update();
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [&](const Bullet &bullet)
                                 {
                                     sf::Vector2f pos = bullet.pos;
                                     return pos.x < 0 || pos.x > window.getSize().x || pos.y < 0 || pos.y > window.getSize().y;
                                 }),
                  bullets.end());
}

void Player::handleMovement(float deltaTime)
{
    sf::Vector2f movement(0.0f, 0.0f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        movement.y -= speed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        movement.y += speed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        movement.x -= speed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        movement.x += speed * deltaTime;
    }

    characterShape.move(movement);
    gunShape.move(movement);
}

void Player::handleRotation(sf::RenderWindow &window, float deltaTime)
{
    sf::Vector2f characterPosition = characterShape.getPosition();

    float deltaX = mousePos.x - characterPosition.x;
    float deltaY = mousePos.y - characterPosition.y;
    angle = std::atan2(deltaY, deltaX) * 180 / M_PI - 90.f; // Convert from radians to degrees
}

void Player::handleShooting()
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        if (shootCooldown.getElapsedTime().asMilliseconds() > 100)
        { // Shooting cooldown (200ms)
            // Wyliczenie pozycji wylotu pocisku
            sf::Vector2f gunTip = gunShape.getPosition() +
                                  sf::Vector2f(std::cos((gunShape.getRotation() + 90.f) * M_PI / 180.f) * gunShape.getSize().y,
                                               std::sin((gunShape.getRotation() + 90.f) * M_PI / 180.f) * gunShape.getSize().y) +
                                  sf::Vector2f(std::cos(gunShape.getRotation() * M_PI / 180.f) * gunShape.getSize().x / 2,
                                               std::sin(gunShape.getRotation() * M_PI / 180.f) * gunShape.getSize().x / 2);

            // Dodanie nowego pocisku
            addBullet(gunTip.x, gunTip.y, angle + 90.f);
            shootCooldown.restart();
        }
    }
}

void Player::draw(sf::RenderWindow &window)
{
    characterShape.setRotation(angle);
    gunShape.setRotation(angle);

    gunShape.setPosition(
        characterShape.getPosition().x + this->gunOffset.x * std::cos(angle * M_PI / 180.f),
        characterShape.getPosition().y + this->gunOffset.y * std::sin(angle * M_PI / 180.f));

    drawCharacter(window);
    drawGun(window);
}

void Player::drawCharacter(sf::RenderWindow &window)
{
    window.draw(characterShape);
}

void Player::drawGun(sf::RenderWindow &window)
{
    window.draw(gunShape);
}

// Funkcja dodająca pocisk do tabeli
void Player::addBullet(float x, float y, float angle)
{
    bullets.emplace_back(x, y, angle);
}

void Player::setHealth(int health)
{
    this->health = health;
}

void Player::setAngle(float newAngle)
{
    this->angle = newAngle;
}