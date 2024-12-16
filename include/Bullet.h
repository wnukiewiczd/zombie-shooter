#ifndef BULLET_H
#define BULLET_H

#include <SFML/Graphics.hpp>

class Bullet {
public:
    Bullet(float startX, float startY, float angle);
    void update();
    void draw(sf::RenderWindow& window);

    sf::Vector2f pos;

private:
    sf::CircleShape shape;
    sf::Vector2f velocity;

    float SPEED = 20.f;
    float BULLET_RADIUS = 5.f;
};

#endif