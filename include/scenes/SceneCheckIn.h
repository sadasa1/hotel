#ifndef SCENECHECKIN_H
#define SCENECHECKIN_H

#include "Scene.h"
#include <SFML/Graphics.hpp>

class SceneCheckIn : public Scene {
public: 
    explicit SceneCheckIn(sf::RenderWindow* win); 
    virtual ~SceneCheckIn() = default; 

    virtual void enter();
    virtual void exit(); 
    virtual void handleEvent(const sf::Event& event);
    virtual void update(float dt); 
    virtual void draw(sf::RenderTarget& target); 
private:
    sf::RenderWindow* m_win; 
    sf::RectangleShape m_testShape; 
};

#endif // SCENECHECKIN_H
