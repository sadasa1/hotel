#include <SFML/Graphics.hpp>
#include "App.h"

App::App() : window(sf::VideoMode({1280, 720}), "Hotel Surveillance", sf::Style::Close) {
    window.setFramerateLimit(60);

    m_hotel.loadAllGuests("data/guests.tsv");
    m_hotel.selectRoundGuests(); 

    currScene = std::make_unique<SceneCheckIn>(&window, &m_hotel);
    currScene->enter();

}

void App::run() {
    sf::Clock frameClock;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                continue;
            }
            if (const auto* k = event->getIf<sf::Event::KeyPressed>()) {
                if (k->scancode == sf::Keyboard::Scancode::Escape) {
                    window.close();
                    continue;
                }
            }
            if (currScene) currScene->handleEvent(*event);
        }

        float dt = frameClock.restart().asSeconds();

        if (currScene) currScene->update(dt);

        window.clear();
        if (currScene) currScene->draw(window);
        window.display();
    }
}

