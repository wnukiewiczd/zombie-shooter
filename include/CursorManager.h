#ifndef CURSORMANAGER_H
#define CURSORMANAGER_H

#include <SFML/Graphics.hpp>

class CursorManager {
public:
    CursorManager(float radius);
    void draw(sf::RenderWindow& window);
    void update(sf::RenderWindow& window);
    sf::Vector2i mousePos;

private:
    sf::CircleShape shape;
};

#endif
