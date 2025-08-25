#ifndef BADGESPRITE_H
#define BADGESPRITE_H

#include <SFML/Graphics.hpp>
#include "Badge.h"

struct BadgeSprite {
    static constexpr unsigned     kFontPx         = 26;
    static constexpr sf::Vector2f kPhotoTopLeft   {45.f, 147.f};
    static constexpr sf::Vector2f kGuestIdPos     {180.f, 225.f};
    static constexpr sf::Vector2f kNamePos        {180.f, 175.f};
    static constexpr sf::Vector2f kPortraitScale  {3.5f, 3.5f};
    static constexpr sf::Vector2f kBadgeScale     {0.4f, 0.4f};
    static constexpr float        kMaxNameWidth   = 185.f;

    sf::Sprite base;
    sf::Sprite portrait;
    sf::Text   guestID;
    sf::Text   name;

    sf::Vector2f pos{0.f, 0.f};

    BadgeSprite(const sf::Font& font,
                const sf::Texture& badgeTex,
                const sf::Texture& portraitTex)
    : base(badgeTex)
    , portrait(portraitTex)
    , guestID(font, "", kFontPx)
    , name(font, "", kFontPx)
    {
        auto b = base.getLocalBounds();
        base.setOrigin({ b.position.x + b.size.x * 0.5f,
                         b.position.y + b.size.y * 0.5f });
    }

    void setData(const Badge& b) {
        guestID.setCharacterSize(kFontPx);
        guestID.setString(b.guestID);
        guestID.setFillColor(sf::Color::Black);

        name.setCharacterSize(100);
        name.setString(b.name);
        name.setFillColor(sf::Color::Black);

        layout();
    }

    void setPosition(const sf::Vector2f& p) {
        pos = p;
        layout();
    }

    void draw(sf::RenderTarget& t) const {
        t.draw(base);
        t.draw(portrait);
        t.draw(guestID);
        t.draw(name);
    }

private:
    sf::Vector2f topLeft() const {
        auto bb = base.getLocalBounds();
        sf::Vector2f halfScaled{ (bb.size.x * kBadgeScale.x) * 0.5f,
                                 (bb.size.y * kBadgeScale.y) * 0.5f };
        return { pos.x - halfScaled.x, pos.y - halfScaled.y };
    }

    void fitName() {
        while (name.getLocalBounds().size.x > kMaxNameWidth && name.getCharacterSize() > 4) {
            name.setCharacterSize(name.getCharacterSize() - 1);
        }
    }

    void layout() {
        base.setScale(kBadgeScale);
        base.setPosition(pos);

        sf::Vector2f tl = topLeft();

        portrait.setScale(kPortraitScale);
        portrait.setPosition(tl + kPhotoTopLeft);

        guestID.setPosition(tl + kGuestIdPos);

        fitName();
        name.setPosition(tl + kNamePos);
    }
};

#endif // BADGESPRITE_H
