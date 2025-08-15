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
    sf::RenderWindow window; 
    std::unique_ptr<Scene> currScene;
    Hotel m_hotel;  
};

#endif // APP_H