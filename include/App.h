#ifndef APP_H
#define APP_H

#include <SFML/Graphics.hpp>
#include <memory>
#include "Scene.h"
#include "Hotel.h"
#include "scenes/SceneCheckIn.h"

class App {
public:
    App();
    void run();
private:
    sf::RenderWindow m_window;
    std::unique_ptr<Scene> m_scene;
    Hotel m_hotel;
};

#endif
