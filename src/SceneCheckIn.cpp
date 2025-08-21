#include "scenes/SceneCheckIn.h"

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
    // Assign fonts after loading
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
    m_decision.reset(); 
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
    m_decision = DeskDecision::APPROVE;

    // TODO: hook into Hotel (assign room / enqueue entrance walk).
    // e.g. if you add an API later:
    // m_hotel->approveAtDesk(*m_curr);

    loadNextClaimer(); // move on to the next claimer
}

void SceneCheckIn::onFlag() {
    m_decision = DeskDecision::FLAG;

    // TODO: hook into Hotel (log identity anomaly / deny).
    // e.g.:
    // m_hotel->flagAtDesk(*m_curr);

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
        }
        else if (m_flagButton.getGlobalBounds().contains(pos)) {
            m_flagPressed = true; 
        }
    }

    if (auto* mr = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mr->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = m_win->mapPixelToCoords({mr->position.x, mr->position.y});
            if (m_approvePressed && m_approveButton.getGlobalBounds().contains(pos)) {
                onApprove();
            }
            if (m_flagPressed && m_flagButton.getGlobalBounds().contains(pos)) {
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

    target.draw(m_approveButton);
    target.draw(m_flagButton);
    target.draw(m_approveLabel);
    target.draw(m_flagLabel);
    target.draw(m_guestID);
    target.draw(m_name);
    target.draw(m_badgePortrait);
    target.draw(m_liveSprite);
}