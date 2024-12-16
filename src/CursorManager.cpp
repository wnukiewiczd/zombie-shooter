#include "CursorManager.h"

CursorManager::CursorManager(float radius) {
    shape.setRadius(radius);
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineColor(sf::Color::Blue);
    shape.setOutlineThickness(2.f);
    shape.setOrigin(radius, radius);
    shape.setPosition(0, 0);

}

void CursorManager::update(sf::RenderWindow& window) {
    this->mousePos = sf::Mouse::getPosition(window);
    shape.setPosition(mousePos.x, mousePos.y);
}

void CursorManager::draw(sf::RenderWindow& window) {
    window.draw(shape);
}
