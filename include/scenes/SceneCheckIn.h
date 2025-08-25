#ifndef SCENECHECKIN_H
#define SCENECHECKIN_H

#include "Scene.h"
#include <random>
#include "BadgeSprite.h"
#include "DeskDecision.h"
#include <optional>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

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

    struct Tween {
        sf::Vector2f start{};
        sf::Vector2f end{};
        float t = 0.f;
        float duration = 0.6f;
        bool active = false;
        bool starting = false;
        void reset(const sf::Vector2f& s, const sf::Vector2f& e, float d) {
            start = s;
            end = e;
            duration = d;
            t = 0.f;
            active = true;
        }
    };

private:
    sf::RenderWindow* m_win = nullptr;
    Hotel* m_hotel = nullptr;
    Claimer* m_curr = nullptr;

    std::mt19937 m_rng;

    sf::View m_leftView;
    sf::View m_rightView;

    sf::Texture m_texBgLeft;
    sf::Sprite  m_bgLeft;
    sf::Texture m_texBgRight;
    sf::Sprite  m_bgRight;

    sf::Font m_font;

    sf::Text m_guestID;
    sf::Text m_name;

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

    sf::SoundBuffer m_badgeSlideBuf;
    std::vector<sf::Sound> m_badgeSlide;

    sf::Music m_rain;
    sf::Music m_ambience;
    sf::Music m_clockTicking;
    sf::Music m_violins;

    sf::Texture m_liveTexture;
    sf::Sprite m_liveSprite;

    sf::Texture m_badgePortraitTex;
    sf::Texture m_badgeTex;

    sf::Texture m_badgeLeftTex;
    sf::Sprite m_badgeLeft;

    std::optional<BadgeSprite> m_badge;

    Tween m_tweenLeft;
    Tween m_tweenRight;

    bool m_rightBadgeInteractable = false;
    bool m_draggingRight = false;
    sf::Vector2f m_dragOffset{0.05f, 0.05f};

    bool m_approveHover = false;
    bool m_approvePressed = false;
    bool m_flagHover = false;
    bool m_flagPressed = false;

    sf::RectangleShape m_divider;
    sf::RectangleShape m_dividerL;
};

#endif
