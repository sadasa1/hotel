#include "scenes/SceneCheckIn.h"
#include "Hotel.h"

static sf::Vector2f deskPos{40.f, 40.f};
static float lineHeight = 26.f;

static sf::Vector2f approvePos{1040.f, 400.f};
static sf::Vector2f flagPos{1040.f, 520.f};
static sf::Vector2f buttonSize{200.f, 100.f};

static sf::Vector2f badgePos{420.f, 60.f};
static sf::Vector2f BadgePortraitPos{440.f, 80.f};
static sf::Vector2f LivePos{720.f, 60.f};

SceneCheckIn::SceneCheckIn(sf::RenderWindow* win, Hotel* h)
: m_win(win)
, m_hotel(h)
, m_approveLabel(m_font, "APPROVE", 100)
, m_flagLabel(m_font, "FLAG", 100)
, m_guestID(m_font, "GUEST ID:", 20)
, m_name(m_font, "NAME:", 20)
, m_badgePortrait(m_badgePortraitTexture) 
, m_badgeSprite(m_badgeTexture)
, m_liveSprite(m_liveTexture)
, m_approveColor(40, 180, 40)
, m_flagColor(200, 40, 40)
{
    (void)m_font.openFromFile("assets/fonts/BMmini.TTF");

    m_approveLabel.setFont(m_font);
    m_flagLabel.setFont(m_font);
    m_guestID.setFont(m_font);
    m_name.setFont(m_font);

    m_approveLabel.setCharacterSize(20);
    m_flagLabel.setCharacterSize(20);
    m_guestID.setCharacterSize(20);
    m_name.setCharacterSize(20);

    styleButton(m_approveButton, m_approveLabel, approvePos, buttonSize, "APPROVE", m_approveColor);
    styleButton(m_flagButton,    m_flagLabel,    flagPos, buttonSize, "FLAG",    m_flagColor);

    (void)m_clickDownBuf.loadFromFile("assets/sfx/mouseClickDown.wav");
    (void)m_clickUpBuf.loadFromFile("assets/sfx/mouseClickUp.wav");

    m_clickDown.reserve(5);
    m_clickUp.reserve(5);

    for (int i = 0; i < 5; i++) {
        m_clickDown.emplace_back(m_clickDownBuf);
        m_clickUp.emplace_back(m_clickUpBuf);
        m_clickDown.back().setRelativeToListener(true);
        m_clickUp.back().setRelativeToListener(true);
    }
}

void SceneCheckIn::playFromPool(std::vector<sf::Sound>& pool, float volume) {
    for (auto& s : pool) {
        if (s.getStatus() != sf::Sound::Status::Playing) {
            s.setVolume(volume);
            s.play();
            return;
        }
    }
}

void SceneCheckIn::enter() {
    m_approveHover   = false;
    m_flagHover      = false;
    m_approvePressed = false;
    m_flagPressed    = false;

    loadNextClaimer();
}

void SceneCheckIn::exit() {
    ;
}

bool SceneCheckIn::contains(const sf::Shape& s, const sf::Vector2f& p) {
    return s.getGlobalBounds().contains(p);
}

void SceneCheckIn::styleButton(sf::RectangleShape& r, sf::Text& label, const sf::Vector2f& pos, const sf::Vector2f& size, const std::string& text, const sf::Color color) {
    r.setSize(size);
    r.setPosition(pos);
    r.setFillColor(color);
    r.setOutlineThickness(2.f);
    r.setOutlineColor(sf::Color::Black);

    label.setFont(m_font);
    label.setString(text);
    label.setFillColor(sf::Color::White);

    sf::FloatRect bounds = label.getLocalBounds(); 
    float localX = pos.x + (size.x-bounds.size.x) * 0.5f - bounds.position.x;
    float localY = pos.y + (size.y - bounds.size.y) * 0.5f - bounds.position.y - 4.f + 4;
    label.setPosition({localX, localY});
}

void SceneCheckIn::loadNextClaimer() {
    m_curr = m_hotel->loadClaimer();

    if (!m_curr) {
        return; 
    }

    m_guestID.setString("Guest ID: " + m_curr->badge.guestID);
    m_name.setString("Name: " + m_curr->badge.name);

    (void)m_badgePortraitTexture.loadFromFile("assets/portraits/" + m_curr->badge.portraitID + ".png");
    m_badgePortrait.setTexture(m_badgePortraitTexture, true);

    (void)m_liveTexture.loadFromFile("assets/portraits/" + m_curr->live.observedPortraitID + ".png");
    m_liveSprite.setTexture(m_liveTexture, true);
}

void SceneCheckIn::onApprove() {
    if (!m_curr) { return; }
    auto res = m_hotel->resolveDecision(DeskDecision::APPROVE);
    std::cout << res.correct << " + " << res.reason << std::endl; 

    loadNextClaimer();
}

void SceneCheckIn::onFlag() {
    if (!m_curr) {return; }
    auto res = m_hotel->resolveDecision(DeskDecision::FLAG);
    std::cout << res.correct << " + " << res.reason << std::endl; 

    loadNextClaimer();
}


void SceneCheckIn::handleEvent(const sf::Event& event) {
    if (auto* mm = event.getIf<sf::Event::MouseMoved>()) {
        const sf::Vector2f pos = m_win->mapPixelToCoords({mm->position.x, mm->position.y});

        if (m_approveButton.getGlobalBounds().contains(pos)) {
            m_approveHover = true; 
        } 
        else {
            m_approveHover = false; 
        }

        if (m_flagButton.getGlobalBounds().contains(pos)) {
            m_flagHover = true; 
        }
        else {
            m_flagHover = false; 
        }
    }

    if (auto* mc = event.getIf<sf::Event::MouseButtonPressed>()) {
        const sf::Vector2f pos = m_win->mapPixelToCoords({mc->position.x, mc->position.y});
        if (mc->button != sf::Mouse::Button::Left) {
            return; 
        }
        if (m_approveButton.getGlobalBounds().contains(pos)) {
            m_approvePressed = true; 
            playFromPool(m_clickDown, 80.f);
        }
        else if (m_flagButton.getGlobalBounds().contains(pos)) {
            m_flagPressed = true; 
            playFromPool(m_clickDown, 80.f);
        }
    }

    if (auto* mr = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mr->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = m_win->mapPixelToCoords({mr->position.x, mr->position.y});
            if (m_approvePressed && m_approveButton.getGlobalBounds().contains(pos)) {
                playFromPool(m_clickUp, 100.f);
                onApprove();
            }
            if (m_flagPressed && m_flagButton.getGlobalBounds().contains(pos)) {
                playFromPool(m_clickUp, 100.f);
                onFlag(); 
            }

            m_approvePressed = false; 
            m_flagPressed = false; 
        }
    }  

    if (event.is<sf::Event::MouseLeft>() || event.is<sf::Event::FocusLost>()) {
        m_approveHover  = false;
        m_flagHover     = false;
        m_approvePressed = false;
        m_flagPressed    = false;
    }

}

void SceneCheckIn::update(float dt) {
    if (dt > 0.1f) dt = 0.1f;
}

void SceneCheckIn::draw(sf::RenderTarget& target) {
    sf::Color approveHoverC(20, 120, 20);
    sf::Color approvePressC(10, 80, 10);  
    sf::Color flagHoverC(140, 20, 20);
    sf::Color flagPressC(100, 10, 10);

    if (m_approvePressed) {
        m_approveButton.setFillColor(approvePressC);
    }
    else if (m_approveHover) {
        m_approveButton.setFillColor(approveHoverC);
    }
    else {
        m_approveButton.setFillColor(m_approveColor);
    }

    if (m_flagPressed) {
        m_flagButton.setFillColor(flagPressC);
    }
    else if (m_flagHover) {
        m_flagButton.setFillColor(flagHoverC);
    }
    else {
        m_flagButton.setFillColor(m_flagColor);
    }

    // Badge portrait
    sf::FloatRect bb = m_badgePortrait.getLocalBounds();

    m_badgePortrait.setOrigin(
        sf::Vector2f(bb.position.x + bb.size.x / 2.f,
                    bb.position.y + bb.size.y / 2.f)
    );

    float bw = 128.f;
    float bh = 128.f;
    float bScaleX = bw / bb.size.x;
    float bScaleY = bh / bb.size.y;
    float bScale = std::min(bScaleX, bScaleY);
    m_badgePortrait.setScale({bScale, bScale});

    m_badgePortrait.setPosition(sf::Vector2f(300.f, 400.f));
    target.draw(m_badgePortrait);

    // Live portrait
    sf::FloatRect lb = m_liveSprite.getLocalBounds();

    m_liveSprite.setOrigin(
        sf::Vector2f(lb.position.x + lb.size.x / 2.f,
                    lb.position.y + lb.size.y / 2.f)
    );

    float lw = 128.f;
    float lh = 128.f;
    float lScaleX = lw / lb.size.x;
    float lScaleY = lh / lb.size.y;
    float lScale = std::min(lScaleX, lScaleY);
    m_liveSprite.setScale({lScale, lScale});

    m_liveSprite.setPosition(sf::Vector2f(800.f, 400.f));
    target.draw(m_liveSprite);


    target.draw(m_approveButton);
    target.draw(m_flagButton);
    target.draw(m_approveLabel);
    target.draw(m_flagLabel);
    target.draw(m_guestID);
    target.draw(m_name);
    target.draw(m_badgePortrait);
    target.draw(m_liveSprite);
    
}


