#ifndef SCENECHECKIN_H
#define SCENECHECKIN_H

#include "Scene.h"
#include "DeskDecision.h"
#include <optional>
#include <SFML/Graphics.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

class Hotel;
struct Claimer; 

class SceneCheckIn : public Scene {
public: 
    explicit SceneCheckIn(sf::RenderWindow* win, Hotel* h); 
    virtual ~SceneCheckIn() = default; 
    void playFromPool(std::vector<sf::Sound>& pool, float volume);

    virtual void enter();
    virtual void exit(); 
    virtual void handleEvent(const sf::Event& event);
    virtual void update(float dt); 
    virtual void draw(sf::RenderTarget& target); 

    void loadNextClaimer(); 
    void onApprove();
    void onFlag(); 

    bool contains(const sf::Shape& s, const sf::Vector2f& p);
    void styleButton(sf::RectangleShape& r, sf::Text& label, const sf::Vector2f& pos, const sf::Vector2f& size, 
                    const std::string& text, const sf::Color color);



private:
    sf::RenderWindow* m_win; 
    Hotel* m_hotel; 
    Claimer* m_curr = nullptr;

    sf::RectangleShape m_approveButton;
    sf::Text m_approveLabel;
    sf::Color m_approveColor; 
    sf::RectangleShape m_flagButton;  
    sf::Text m_flagLabel; 
    sf::Color m_flagColor; 

    sf::SoundBuffer m_clickDownBuf;
    sf::SoundBuffer m_clickUpBuf;
    std::vector<sf::Sound> m_clickDown; 
    std::vector<sf::Sound> m_clickUp;

    sf::Font m_font;

    sf::Text m_guestID;
    sf::Text m_name; 
    sf::Texture m_badgePortraitTexture; 
    sf::Sprite m_badgePortrait; 
    sf::Texture m_badgeTexture;
    sf::Sprite m_badgeSprite; 

    sf::Texture m_liveTexture;
    sf::Sprite m_liveSprite; 

    bool m_approveHover = false;
    bool m_approvePressed = false; 

    bool m_flagHover = false; 
    bool m_flagPressed = false; 
};

#endif // SCENECHECKIN_H
