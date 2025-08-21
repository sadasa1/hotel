#ifndef SCENE_H
#define SCENE_H

#include <SFML/Graphics.hpp>
#include <Hotel.h>

class Scene {
public:
    Scene() = default; 
    virtual ~Scene() = default; 

    virtual void enter() = 0;
    virtual void exit() = 0; 
    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(float dt) = 0; 
    virtual void draw(sf::RenderTarget& target) = 0; 
};

#endif // SCENE_H