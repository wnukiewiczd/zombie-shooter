#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "Bullet.h"
#include "Player.h"
#include "ClientSideCommunicationManager.h"
#include "ServerManager.h"
#include "CursorManager.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Wpisz wiecej argumentow" << std::endl;
        return -1;
    }

    const int windowWidth = 800;
    const int windowHeight = 600;

    ClientSideCommunicationManager communicationManager(argv[1], 54001, argv[2]);

    communicationManager.connectToServer();

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Shooter Game");
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
        communicationManager.receiveData(player);

        // Aktualizacja gracza
        player.update(window, deltaTime);

        // Aktualizacja celownika
        cursorManager.update(window);

        // Czyszczenie okna przed renderingiem kolejnym
        window.clear(sf::Color::Green);

        // Rysowanie pocisków obecnego gracza
        for (auto it = player.bullets.begin(); it != player.bullets.end();)
        {
            it->draw(window);
            if (it->targetHit(communicationManager))
            {
                it = player.bullets.erase(it);
            }
            else
            {
                ++it;
            }
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

        // Rysowanie informacji o zakonczeniu gry jak ktos wygral
        if (communicationManager.gameFinished)
        {
            communicationManager.drawFinishedInfo(window);
        }

        // Rysowanie celownika
        cursorManager.draw(window);

        window.display();
    }

    return 0;
}