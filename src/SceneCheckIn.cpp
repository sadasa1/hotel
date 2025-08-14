#include "scenes/SceneCheckIn.h"

SceneCheckIn::SceneCheckIn(sf::RenderWindow* win) : m_win(win) {

}

void SceneCheckIn::enter() {
    m_testShape.setFillColor(sf::Color::Cyan);
    m_testShape.setSize({100.f, 100.f});
    m_testShape.setPosition({200.f, 300.f});
}

void SceneCheckIn::exit() {
    // todo 
}

void SceneCheckIn::handleEvent(const sf::Event& event) {
    if (auto key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::A) {
            m_testShape.setFillColor(sf::Color::Green);
        }
        else if (key->code == sf::Keyboard::Key::D) {
            m_testShape.setFillColor(sf::Color::Red);
        }
        else if (key->code == sf::Keyboard::Key::Escape) {
            m_win->close(); 
        }
    }
}

void SceneCheckIn::update(float dt) {
    if (dt > 0.1f) dt = 0.1f;

    m_testShape.move({100.f * dt, 0.f});

    if (m_testShape.getPosition().x > m_win->getSize().x) {
        m_testShape.setPosition({-1 * m_testShape.getSize().x, m_testShape.getPosition().y});
    }
}

void SceneCheckIn::draw(sf::RenderTarget& target) {
    target.draw(m_testShape);
}