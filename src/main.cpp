#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "Bullet.h"
#include "CursorManager.h"

int main() {
    const int windowWidth = 800;
    const int windowHeight = 600;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Zombie Shooter Game");
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);

    Player player(windowWidth / 2.0f, windowHeight / 2.0f, 30.0f);

    CursorManager cursorManager(5.f);

    std::vector<Bullet> bullets;

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();

        // Update player
        player.update(window, deltaTime, bullets);
        
        // Update custom cursor
        cursorManager.update(window);

        // Update bullets
        for (auto& bullet : bullets) {
            bullet.update();
        }
        // Delete bullets which are outside the window
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [&](const Bullet& bullet) {
                sf::Vector2f pos = bullet.pos;
                return pos.x < 0 || pos.x > window.getSize().x || pos.y < 0 || pos.y > window.getSize().y;
            }),
            bullets.end());

        // Fill the window with color before drawing anything
        window.clear(sf::Color::Green);
        
        for (auto& bullet : bullets) {
            bullet.draw(window);
        }

        player.draw(window);
        
        cursorManager.draw(window);
        
        window.display();
    }

    return 0;
}
