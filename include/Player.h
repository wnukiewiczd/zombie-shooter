#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include "Bullet.h"

class Player {
public:
    Player(float x, float y, float radius);
    void update(sf::RenderWindow& window, float deltaTime, std::vector<Bullet>& bullets);
    void draw(sf::RenderWindow& window);

private:
    sf::CircleShape characterShape;
    sf::RectangleShape gunShape;
    
    float speed;
    sf::Vector2f gunOffset;
    sf::Vector2i mousePos;
    float angle;
    sf::Clock shootCooldown;

    void drawCharacter(sf::RenderWindow& window);
    void drawGun(sf::RenderWindow& window);
    void handleMovement(float deltaTime);
    void handleRotation(sf::RenderWindow& window, float deltaTime);
    void handleShooting(std::vector<Bullet>& bullets);
};

#endif
