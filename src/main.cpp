#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "ClientSideCommunicationManager.h"
#include "Player.h"
#include "Bullet.h"
#include "ServerManager.h"
#include "CursorManager.h"

int main()
{
    const int windowWidth = 800;
    const int windowHeight = 600;

    ClientSideCommunicationManager communicationManager("127.0.0.1", 54001, "Player1");

    communicationManager.connectToServer();

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Multiplayer Game");
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);

    Player player(communicationManager.playerId, communicationManager.playerName, windowWidth / 2.0f, windowHeight / 2.0f, 30.0f); // Twój lokalny gracz

    CursorManager cursorManager(5.f);

    sf::Clock clock;

    while (window.isOpen())
    {
        // Obsługa zdarzeń
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                communicationManager.disconnectFromServer();
                window.close();
            }
        }

        float deltaTime = clock.restart().asSeconds();

        // Wysyłanie danych gracza do serwera
        communicationManager.sendClientDataToServer(player);

        // Odbieranie aktualizacji od serwera
        communicationManager.synchronizePlayerList();

        // Aktualizacja gracza
        player.update(window, deltaTime);

        // Aktualizacja celownika
        cursorManager.update(window);

        // Czyszczenie okna przed renderingiem kolejnym
        window.clear(sf::Color::Green);

        // Rysowanie pocisków obecnego gracza
        for (auto &bullet : player.bullets)
        {
            bullet.draw(window);
        }

        // Rysowanie lokalnego gracza
        player.draw(window);

        // Rysowanie innych graczy i ich pocisków
        for (auto &[id, pl] : communicationManager.clientPlayerList)
        {
            for (auto &bullet : pl.bullets)
            {
                bullet.draw(window);
            }
            pl.draw(window);
        }

        // Rysowanie celownika
        cursorManager.draw(window);

        window.display();
    }

    return 0;
}