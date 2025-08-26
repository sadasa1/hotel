#include "scenes/SceneCheckIn.h"
#include "Hotel.h"

namespace {

constexpr float kWinW = 1920.f;
constexpr float kWinH = 1080.f;

constexpr float kDeskW = kWinW * 0.35f;
constexpr float kPad   = 16.f;
constexpr float kDeskX = kWinW - kDeskW;
constexpr float kDeskH = kWinH;

constexpr float kBadgeY      = 80.f;
constexpr float kPortraitBox = 160.f;

constexpr float kLiveX = kPad;
constexpr float kLiveY = kPad;
constexpr float kLiveW = (kWinW - kDeskW) - kPad * 2.f;
constexpr float kLiveH = kWinH - kPad * 2.f;
constexpr float kLiveHeadBox = 380.f;

constexpr float kCounterTopY = 447.f;

const sf::Vector2f kLivePos{ kLiveX + kLiveW * 0.5f, kCounterTopY };

constexpr float kBadgeSlideDuration = 0.65f;
constexpr float kSlideYOffsetLeft   = 450.f;
const sf::Vector2f kBadgePos       { kDeskX + kDeskW * 0.5f, kBadgeY + kPortraitBox + 300.f };
const sf::Vector2f kRightBadgeStart{ kBadgePos.x, -200.f };
const sf::Vector2f kRightBadgeEnd  { kBadgePos   };
const sf::Vector2f kLeftBadgeStart { kLivePos.x, kLivePos.y };
const sf::Vector2f kLeftBadgeEnd   { kLivePos.x, kLivePos.y + kSlideYOffsetLeft };

constexpr float kFeedbackDuration = 2.f;
const sf::Vector2f kFeedbackSize{ kWinW * 0.50f, 56.f };
const sf::Vector2f kFeedbackPos { (kWinW - kFeedbackSize.x) * 0.5f, (kWinH - kFeedbackSize.y) * 0.5f};
const sf::Color kFeedbackBgC    ( 20, 20, 24, 220);
const sf::Color kFeedbackOutline( 80, 80, 90, 255);
const sf::Color kFeedbackBad    (220, 70, 70, 255);

const sf::FloatRect kRosterRect({ 24.f, 24.f}, {520.f, 240.f});

constexpr int   kUIFontSize    = 20;
constexpr float kButtonOutline = 5.f;
constexpr int   kSfxPool       = 5;

const sf::Vector2f kButtonSize{200.f, 100.f};
const sf::Vector2f kApprovePos{1040.f, 800.f};
const sf::Vector2f kFlagPos   {1040.f, 920.f};

const sf::Color kApproveBaseC ( 40,180, 40);
const sf::Color kApproveHoverC( 20,120, 20);
const sf::Color kApprovePressC( 10, 80, 10);
const sf::Color kFlagBaseC    (200, 40, 40);
const sf::Color kFlagHoverC   (140, 20, 20);
const sf::Color kFlagPressC   (100, 10, 10);

constexpr float kButtonVol   = 80.f;
constexpr float kRainVol     = 80.f;
constexpr float kAmbienceVol = 10.f;
constexpr float kClockVol    = 60.f;
constexpr float kViolinsVol  = 100.f;
constexpr float kSlideVol    = 100.f;
constexpr float kWrongVol    = 100.f;

const sf::Vector2f kBadgeLeftScale{0.8f, 0.8f};

constexpr float kBadgeProbeXRatio = 0.85f;

void setButtonVisual(sf::RectangleShape& btn, const sf::Color& base, const sf::Color& hover, const sf::Color& press, bool isHover, bool isPressed) {
    if (isPressed) { btn.setFillColor(press); return; }
    if (isHover) { btn.setFillColor(hover); return; }
    btn.setFillColor(base);
}

}

SceneCheckIn::SceneCheckIn(sf::RenderWindow* win, Hotel* h)
: m_win(win)
, m_hotel(h)
, m_curr(nullptr)
, m_rng(std::random_device{}())
, m_leftView()
, m_rightView()
, m_texBgLeft()
, m_bgLeft(m_texBgLeft)
, m_texBgRight()
, m_bgRight(m_texBgRight)
, m_font()
, m_guestID(m_font, "GUEST ID:", kUIFontSize)
, m_name(m_font, "NAME:", kUIFontSize)
, m_approveButton()
, m_approveLabel(m_font, "APPROVE", kUIFontSize)
, m_approveColor(kApproveBaseC)
, m_flagButton()
, m_flagLabel(m_font, "FLAG", kUIFontSize)
, m_flagColor(kFlagBaseC)
, m_clickDownBuf()
, m_clickUpBuf()
, m_clickDown()
, m_clickUp()
, m_badgeSlideBuf()
, m_badgeSlide()
, m_wrongBuf()
, m_wrong()
, m_rain("assets/sfx/rain.wav")
, m_ambience("assets/sfx/creepyambience.wav")
, m_clockTicking("assets/sfx/clockticking.wav")
, m_violins("assets/sfx/cut_scaryviolins.wav")
, m_liveTexture()
, m_liveSprite(m_liveTexture)
, m_badgePortraitTex()
, m_badgeTex()
, m_badgeLeftTex()
, m_badgeLeft(m_badgeTex)
, m_badge(std::nullopt)
, m_tweenLeft()
, m_tweenRight()
, m_rightBadgeInteractable(false)
, m_draggingRight(false)
, m_dragOffset(0.05f, 0.05f)
, m_approveHover(false)
, m_approvePressed(false)
, m_flagHover(false)
, m_flagPressed(false)
, m_divider()
, m_dividerL()
, m_showRoster(false)
, m_rosterScroll(0.f)
, m_rosterDirty(false)
, m_rosterRect()
, m_rosterBg()
, m_rosterHeader(m_font, "GUESTS EXPECTED TODAY:", 20)
, m_rosterRows()
, m_rosterRT()
, m_rosterSprite(m_rosterRT.getTexture())
, m_feedbackText(m_font, "", kUIFontSize)
{
    m_rain.setVolume(kRainVol); m_rain.setLooping(true); m_rain.play();
    m_clockTicking.setVolume(kClockVol); m_clockTicking.setLooping(true); m_clockTicking.play();
    m_ambience.setVolume(kAmbienceVol); m_ambience.setLooping(true); m_ambience.play();
    m_violins.setVolume(kViolinsVol); m_violins.setLooping(false);

    (void)m_font.openFromFile("assets/fonts/BMmini.TTF");

    styleButton(m_approveButton, m_approveLabel, kApprovePos, kButtonSize, "APPROVE", m_approveColor);
    styleButton(m_flagButton,    m_flagLabel,    kFlagPos,    kButtonSize, "FLAG",    m_flagColor);

    (void)m_clickDownBuf.loadFromFile("assets/sfx/mouseClickDown.wav");
    (void)m_clickUpBuf.loadFromFile("assets/sfx/mouseClickUp.wav");
    (void)m_wrongBuf.loadFromFile("assets/sfx/wrong.mp3");
    (void)m_badgeSlideBuf.loadFromFile("assets/sfx/badgeslide.wav");
    (void)m_texBgLeft.loadFromFile("assets/textures/m_texBgLeft.png");
    (void)m_texBgRight.loadFromFile("assets/textures/m_texDesk2.png");

    m_bgLeft.setTexture(m_texBgLeft, true);
    m_bgRight.setTexture(m_texBgRight, true);

    sf::FloatRect leftBox({0.f, 0.f}, {kWinW - kDeskW, kWinH});
    sf::FloatRect rightBox({kDeskX, 0.f}, {kDeskW, kWinH});

    m_leftView = sf::View(leftBox);
    m_rightView = sf::View(rightBox);
    m_leftView.setViewport(sf::FloatRect({0.f, 0.f}, {(kWinW - kDeskW) / kWinW, 1.f}));
    fitSprite(m_bgLeft, leftBox);
    m_rightView.setViewport(sf::FloatRect({(kWinW - kDeskW) / kWinW, 0.f}, {(kDeskW / kWinW), 1.f}));
    fitSprite(m_bgRight, rightBox);

    m_clickDown.reserve(kSfxPool);
    m_clickUp.reserve(kSfxPool);
    m_badgeSlide.reserve(kSfxPool);
    m_wrong.reserve(kSfxPool);
    for (int i = 0; i < kSfxPool; i++) {
        m_clickDown.emplace_back(m_clickDownBuf);
        m_clickUp.emplace_back(m_clickUpBuf);
        m_badgeSlide.emplace_back(m_badgeSlideBuf);
        m_wrong.emplace_back(m_wrongBuf);
        m_clickDown.back().setRelativeToListener(true);
        m_clickUp.back().setRelativeToListener(true);
        m_badgeSlide.back().setRelativeToListener(true);
        m_wrong.back().setRelativeToListener(true);
    }

    m_divider.setSize({2.f, kWinH});
    m_divider.setFillColor(sf::Color(30, 28, 22, 255));
    m_dividerL.setSize({1.f, kWinH});
    m_dividerL.setFillColor(sf::Color(255, 245, 220, 40));
    float dividerX = std::floor(kDeskX);
    m_divider.setPosition({ dividerX - 2.f, 0.f });
    m_dividerL.setPosition({ dividerX, 0.f });

    (void)m_badgeTex.loadFromFile("assets/textures/m_badgeTexRM.png");
    (void)m_badgeLeftTex.loadFromFile("assets/textures/[v1]m_badgeTexLeft.png");
    m_badgeLeft.setTexture(m_badgeLeftTex, true);
    m_badgeLeft.setScale(kBadgeLeftScale);

    m_rosterRect = kRosterRect;
    m_rosterBg.setPosition({m_rosterRect.position.x, m_rosterRect.position.y});
    m_rosterBg.setSize({m_rosterRect.size.x, m_rosterRect.size.y});
    m_rosterBg.setFillColor(sf::Color(20,20,24,220));
    m_rosterBg.setOutlineThickness(2.f);
    m_rosterBg.setOutlineColor(sf::Color(80,80,90));
    m_rosterHeader.setFillColor(sf::Color::White);
    m_rosterHeader.setPosition({m_rosterRect.position.x + 12.f, m_rosterRect.position.y + 10.f});
    m_rosterRT = sf::RenderTexture({(unsigned)m_rosterRect.size.x, (unsigned)m_rosterRect.size.y});
    m_rosterSprite.setPosition({m_rosterRect.position.x, m_rosterRect.position.y});

    m_feedbackBg.setSize(kFeedbackSize);
    m_feedbackBg.setPosition(kFeedbackPos);
    m_feedbackBg.setFillColor(kFeedbackBgC);
    m_feedbackBg.setOutlineThickness(2.f);
    m_feedbackBg.setOutlineColor(kFeedbackOutline);
    m_feedbackText.setFillColor(sf::Color::White);
}

void SceneCheckIn::enter() {
    m_approveHover = false;
    m_flagHover = false;
    m_approvePressed = false;
    m_flagPressed = false;
    m_rosterRows.clear();
    m_rosterRows.push_back(m_rosterHeader);
    auto shuffled = m_hotel->getRoundGuests();
    std::shuffle(shuffled.begin(), shuffled.end(), m_rng);
    for (auto& g : shuffled) {
        sf::Text t(m_font, "", 18);
        t.setFillColor(sf::Color(220,220,230));
        t.setString(g.name + "   (" + g.guestID + ")");
        t.setPosition({12.f, 0.f});
        m_rosterRows.push_back(std::move(t));
    }
    m_rosterScroll = 0.f;
    m_rosterDirty = true;
    loadNextClaimer();
}

void SceneCheckIn::exit() {}

void SceneCheckIn::loadNextClaimer() {
    m_curr = m_hotel->loadClaimer();
    m_rightBadgeInteractable = false;
    m_draggingRight = false;
    playFromPool(m_badgeSlide, kSlideVol);
    if (!m_curr) return;

    m_guestID.setString("Guest ID: " + m_curr->badge.guestID);
    m_name.setString("Name: " + m_curr->badge.name);

    (void)m_badgePortraitTex.loadFromFile("assets/portraits/" + m_curr->badge.portraitID + ".png");
    (void)m_liveTexture.loadFromFile("assets/portraits/" + m_curr->live.observedPortraitID + ".png");

    m_liveSprite.setTexture(m_liveTexture, true);
    m_badge.emplace(m_font, m_badgeTex, m_badgePortraitTex);
    m_badge->setData(m_curr->badge);
    m_badge->setPosition(kRightBadgeStart);

    auto b = m_badgeLeft.getLocalBounds();
    m_badgeLeft.setOrigin({ b.position.x + b.size.x * 0.5f, b.position.y + b.size.y * 0.5f });
    m_badgeLeft.setPosition(kLeftBadgeStart);

    m_tweenRight.reset(kRightBadgeStart, kRightBadgeEnd, kBadgeSlideDuration);
    m_tweenLeft.reset(kLeftBadgeStart, kLeftBadgeEnd, kBadgeSlideDuration);

    std::uniform_int_distribution<int> dist(0, 99);
    if (dist(m_rng) < 8) m_violins.play();
}

void SceneCheckIn::onApprove() {
    if (!m_curr) return;
    auto res = m_hotel->resolveDecision(DeskDecision::APPROVE);
    std::cout << res.correct << " + " << res.reason << std::endl;

    if (!res.correct) {
        playFromPool(m_wrong, kWrongVol);
        showFeedback(res.reason);
    }

    loadNextClaimer();
}

void SceneCheckIn::onFlag() {
    if (!m_curr) return;
    auto res = m_hotel->resolveDecision(DeskDecision::FLAG);
    std::cout << res.correct << " + " << res.reason << std::endl;

    if (!res.correct) {
        playFromPool(m_wrong, kWrongVol);
        showFeedback(res.reason);
    }

    loadNextClaimer();
}

void SceneCheckIn::handleEvent(const sf::Event& event) {
    if (auto* mm = event.getIf<sf::Event::MouseMoved>()) {
        const sf::Vector2f pos = m_win->mapPixelToCoords({ mm->position.x, mm->position.y });
        const sf::Vector2f posRight = m_win->mapPixelToCoords({ mm->position.x, mm->position.y }, m_rightView);
        m_approveHover = m_approveButton.getGlobalBounds().contains(pos);
        m_flagHover = m_flagButton.getGlobalBounds().contains(pos);
        if (m_badge && m_draggingRight) m_badge->setPosition(posRight + m_dragOffset);
        if (m_badge) {
            auto b = m_badge->base.getLocalBounds();
            sf::Vector2f localPt(b.position.x + b.size.x * kBadgeProbeXRatio, b.position.y + b.size.y * 0.5f);
            sf::Vector2f worldPt = m_badge->base.getTransform().transformPoint(localPt);
            float x85 = worldPt.x;
            if (x85 < kDeskX && m_draggingRight) {
                m_rightBadgeInteractable = false;
                m_tweenRight.reset({ m_badge->base.getPosition() }, kRightBadgeEnd, kBadgeSlideDuration);
            }
        }
    }

    if (auto* mc = event.getIf<sf::Event::MouseButtonPressed>()) {
        const sf::Vector2f pos = m_win->mapPixelToCoords({ mc->position.x, mc->position.y });
        const sf::Vector2f posRight = m_win->mapPixelToCoords({ mc->position.x, mc->position.y }, m_rightView);
        if (mc->button != sf::Mouse::Button::Left) return;
        if (m_badge && m_badge->base.getGlobalBounds().contains(posRight)) {
            m_draggingRight = true;
            m_dragOffset = m_badge->base.getPosition() - posRight;
        }
        if (m_approveButton.getGlobalBounds().contains(pos)) {
            m_approvePressed = true; playFromPool(m_clickDown, kButtonVol);
        } else if (m_flagButton.getGlobalBounds().contains(pos)) {
            m_flagPressed = true; playFromPool(m_clickDown, kButtonVol);
        }
    }

    if (auto* mr = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mr->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = m_win->mapPixelToCoords({ mr->position.x, mr->position.y });
            if (m_approvePressed && m_approveButton.getGlobalBounds().contains(pos)) { playFromPool(m_clickUp, 100.f); onApprove(); }
            if (m_flagPressed && m_flagButton.getGlobalBounds().contains(pos))     { playFromPool(m_clickUp, 100.f); onFlag(); }
            if (m_draggingRight) m_draggingRight = false;
            m_approvePressed = false;
            m_flagPressed = false;
        }
    }

    if (auto* tab = event.getIf<sf::Event::KeyPressed>()) {
        if (tab->scancode == sf::Keyboard::Scancode::Tab) m_showRoster = !m_showRoster;
    }

    if (m_showRoster) {
        if (auto* scroll = event.getIf<sf::Event::MouseWheelScrolled>()) {
            if (scroll->wheel == sf::Mouse::Wheel::Vertical) {
                m_rosterScroll -= scroll->delta * 24.f;
                m_rosterDirty = true;
            }
        }
    }

    if (event.is<sf::Event::MouseLeft>() || event.is<sf::Event::FocusLost>()) {
        m_approveHover = false;
        m_flagHover = false;
        m_approvePressed = false;
        m_flagPressed = false;
        if (m_draggingRight) {
            m_draggingRight = false;
            if (m_badge) m_tweenRight.reset({ m_badge->base.getPosition() }, kRightBadgeEnd, kBadgeSlideDuration);
        }
    }
}

void SceneCheckIn::update(float dt) {
    if (dt > 0.1f) dt = 0.1f;

    if (m_feedbackVisible) {
        m_feedbackTimer -= dt;
        if (m_feedbackTimer <= 0.f) m_feedbackVisible = false;
    }

    if (m_tweenLeft.active) {
        m_tweenLeft.t += dt / m_tweenLeft.duration;
        if (m_tweenLeft.t >= 1.f) { m_tweenLeft.t = 1.f; m_tweenLeft.active = false; }
        float k = easeOutCubic(m_tweenLeft.t);
        m_badgeLeft.setPosition(lerp(m_tweenLeft.start, m_tweenLeft.end, k));
    }

    if (m_tweenRight.active && !m_draggingRight && m_badge) {
        m_tweenRight.t += dt / m_tweenRight.duration;
        if (m_tweenRight.t >= 1.f) { m_tweenRight.t = 1.f; m_tweenRight.active = false; }
        float k = easeOutCubic(m_tweenRight.t);
        m_badge->setPosition(lerp(m_tweenRight.start, m_tweenRight.end, k));
    }

    if (!m_tweenRight.active) m_rightBadgeInteractable = true;

    setButtonVisual(m_approveButton, m_approveColor, kApproveHoverC, kApprovePressC, m_approveHover, m_approvePressed);
    setButtonVisual(m_flagButton,    m_flagColor,    kFlagHoverC,    kFlagPressC,    m_flagHover,    m_flagPressed);
    centerScaleSpriteToBox(m_liveSprite, kLiveHeadBox);
    m_liveSprite.setPosition(kLivePos);

    if (m_showRoster && m_rosterDirty) rebuildRosterRT();
}

void SceneCheckIn::draw(sf::RenderTarget& target) {
    auto defaultView = target.getView();

    target.setView(m_leftView);
    target.draw(m_bgLeft);
    if (m_curr) {
        target.draw(m_liveSprite);
        target.draw(m_badgeLeft);
    }

    target.setView(m_rightView);
    target.draw(m_bgRight);
    if (m_curr && m_badge) m_badge->draw(target);

    target.setView(defaultView);
    target.draw(m_approveButton);
    target.draw(m_flagButton);
    target.draw(m_approveLabel);
    target.draw(m_flagLabel);
    target.draw(m_divider);
    target.draw(m_dividerL);

    if (m_feedbackVisible) {
        target.draw(m_feedbackBg);
        target.draw(m_feedbackText);
    }

    if (!m_showRoster) return;
    target.draw(m_rosterBg);
    target.draw(m_rosterSprite);
}

bool SceneCheckIn::contains(const sf::Shape& s, const sf::Vector2f& p) {
    return s.getGlobalBounds().contains(p);
}

void SceneCheckIn::styleButton(sf::RectangleShape& r, sf::Text& label, const sf::Vector2f& pos, const sf::Vector2f& size, const std::string& text, const sf::Color color) {
    r.setSize(size);
    r.setPosition(pos);
    r.setFillColor(color);
    r.setOutlineThickness(kButtonOutline);
    r.setOutlineColor(sf::Color::Black);
    label.setFont(m_font);
    label.setString(text);
    label.setFillColor(sf::Color::White);
    sf::FloatRect bounds = label.getLocalBounds();
    float localX = pos.x + (size.x - bounds.size.x) * 0.5f - bounds.position.x;
    float localY = pos.y + (size.y - bounds.size.y) * 0.5f - bounds.position.y;
    label.setPosition({ localX, localY });
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

void SceneCheckIn::showFeedback(const std::string& msg) {
    m_feedbackText.setString(msg);
    m_feedbackText.setFillColor(kFeedbackBad);
    sf::FloatRect tb = m_feedbackText.getLocalBounds();
    float x = kFeedbackPos.x + (kFeedbackSize.x - tb.size.x) * 0.5f - tb.position.x;
    float y = kFeedbackPos.y + (kFeedbackSize.y - tb.size.y) * 0.5f - tb.position.y;
    m_feedbackText.setPosition({x, y});
    m_feedbackTimer = kFeedbackDuration;
    m_feedbackVisible = true;
}

void SceneCheckIn::rebuildRosterRT() {
    const float rowH  = 24.f;
    const float padY  = 8.f;
    const float padX  = 12.f;
    const float visibleH = m_rosterRect.size.y - 2.f * padY;
    const float contentH = static_cast<float>(m_rosterRows.size()) * rowH;
    float maxScroll = std::max(0.f, contentH - visibleH);
    m_rosterScroll = std::clamp(m_rosterScroll, 0.f, maxScroll);

    m_rosterRT.clear(sf::Color::Transparent);
    {
        sf::RectangleShape innerBg({ m_rosterRect.size.x, m_rosterRect.size.y });
        innerBg.setPosition({ 0.f, 0.f });
        innerBg.setFillColor(sf::Color(30,30,36,230));
        m_rosterRT.draw(innerBg);
    }

    const float contentTop    = padY;
    const float contentBottom = m_rosterRect.size.y - padY;

    float y = contentTop - m_rosterScroll;
    for (auto& row : m_rosterRows) {
        const float yBottom = y + rowH;
        if (y >= contentBottom) break;
        if (yBottom > contentTop) {
            row.setPosition({ padX, y + 2.f });
            m_rosterRT.draw(row);
        }
        y += rowH;
    }

    if (contentH > visibleH) {
        const float trackW   = 6.f;
        const float trackX   = m_rosterRect.size.x - 12.f;
        const float trackTop = padY;
        const float trackH   = visibleH;
        const float minThumb = 28.f;
        float thumbH = std::max(minThumb, trackH * (visibleH / contentH));

        float t = 0.f;
        if (maxScroll > 0.f) {
            t = m_rosterScroll / maxScroll;
        }
        float thumbY = trackTop + (trackH - thumbH) * t;

        sf::RectangleShape track({ trackW, trackH });
        track.setPosition({ trackX, trackTop });
        track.setFillColor(sf::Color(60,60,70));
        m_rosterRT.draw(track);

        sf::RectangleShape thumb({ trackW, thumbH });
        thumb.setPosition({ trackX, thumbY });
        thumb.setFillColor(sf::Color(160,160,185));
        m_rosterRT.draw(thumb);
    }

    m_rosterRT.display();
    m_rosterSprite.setTexture(m_rosterRT.getTexture(), true);
    m_rosterDirty = false;
}

sf::Vector2f SceneCheckIn::lerp(const sf::Vector2f& a, const sf::Vector2f& b, float k) {
    return { a.x + (b.x - a.x) * k, a.y + (b.y - a.y) * k };
}

float SceneCheckIn::easeOutCubic(float x) {
    float t = 1.f - x;
    return 1.f - t * t * t;
}

void SceneCheckIn::fitSprite(sf::Sprite& sprite, const sf::FloatRect& box) {
    auto b = sprite.getLocalBounds();
    sprite.setOrigin({ b.position.x + b.size.x * 0.5f, b.position.y + b.size.y * 0.5f });
    float sx = box.size.x / b.size.x;
    float sy = box.size.y / b.size.y;
    float s = std::max(sx, sy);
    sprite.setScale({ s, s });
    sprite.setPosition({ box.position.x + box.size.x * 0.5f, box.position.y + box.size.y * 0.5f });
}

void SceneCheckIn::centerScaleSpriteToBox(sf::Sprite& sprite, float boxSize) {
    auto lb = sprite.getLocalBounds();
    sprite.setOrigin({ lb.position.x + lb.size.x * 0.5f, lb.position.y + lb.size.y * 0.5f });
    float sx = boxSize / lb.size.x;
    float sy = boxSize / lb.size.y;
    float s = std::min(sx, sy);
    sprite.setScale({ s, s });
}
