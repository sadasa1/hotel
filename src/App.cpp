#include <SFML/Graphics.hpp>
#include "App.h"

App::App() : m_window(sf::VideoMode({1920, 1080}), "Hotel Game", sf::Style::Close) {
    m_window.setFramerateLimit(60);

    m_hotel.loadAllGuests("data/guests.tsv");
    m_hotel.selectRoundGuests(); 

    m_scene = std::make_unique<SceneCheckIn>(&m_window, &m_hotel);
    m_scene->enter();
}

void App::run() {
    sf::Clock frameClock;

    while (m_window.isOpen()) {
        while (auto event = m_window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                m_window.close();
                continue;
            }
            if (const auto* k = event->getIf<sf::Event::KeyPressed>()) {
                if (k->scancode == sf::Keyboard::Scancode::Escape) {
                    m_window.close();
                    continue;
                }
            }
            if (m_scene) m_scene->handleEvent(*event);
        }

        float dt = frameClock.restart().asSeconds();

        if (m_scene) m_scene->update(dt);

        m_window.clear();
        if (m_scene) m_scene->draw(m_window);
        m_window.display();
    }
}

