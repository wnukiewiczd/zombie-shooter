#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include "Bullet.h"
#include <vector>
#include <cmath>

class Player
{
public:
    Player() = default;
    Player(unsigned int id, std::string name, float x, float y, float radius);

    std::vector<Bullet> bullets;
    unsigned int id = 0;
    std::string name = "Player1";

    void update(sf::RenderWindow &window, float deltaTime);
    void draw(sf::RenderWindow &window);

    void setHealth(int health);
    int getHealth() const { return health; }

    // Metoda ustawiająca kąt obrotu
    void setAngle(float newAngle);

    // Metoda zwracająca kąt obrotu
    float getAngle() const
    {
        return angle;
    }

    // Dodajemy getter i setter dla pozycji gracza
    sf::Vector2f getPosition() const { return characterShape.getPosition(); }
    void setPosition(float x, float y) { characterShape.setPosition(x, y); }

    // Dodaj metodę getDirection
    sf::Vector2f getDirection() const
    {
        return sf::Vector2f(std::cos(angle * M_PI / 180.f), std::sin(angle * M_PI / 180.f));
    }

    // Funkcja dodająca pocisk do wektora pocisków
    void addBullet(float x, float y, float angle);

    float getRotation() const { return angle; }

private:
    sf::CircleShape characterShape;         // Reprezentacja gracza jako okrąg
    sf::RectangleShape gunShape;            // Reprezentacja broni
    sf::RectangleShape healthBarShape;      // Reprezentacja życia
    sf::RectangleShape healthBarFrameShape; // Reprezentacja życia obramówki
    sf::Vector2f gunOffset;                 // Pozycja broni względem gracza
    float speed = 200.0f;                   // Prędkość poruszania się gracza
    float angle = 0.0f;                     // Kąt obrotu broni
    int health = 100;                       // Domyślne zdrowie gracza
    sf::Vector2i mousePos;                  // Pozycja myszy
    sf::Clock shootCooldown;                // Czas od ostatniego strzału
    float healthBarWidth;

    // Prywatne metody pomocnicze
    void drawCharacter(sf::RenderWindow &window);
    void drawGun(sf::RenderWindow &window);
    void drawHealthBar(sf::RenderWindow &window);
    void handleMovement(float deltaTime);
    void handleRotation(sf::RenderWindow &window, float deltaTime);
    void handleShooting();
    void updateBullets(sf::RenderWindow &window);
    void dealDamage(int damage);
};

#endif
