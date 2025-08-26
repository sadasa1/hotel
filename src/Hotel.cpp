#include "Hotel.h"

Hotel::Hotel() : m_rng(std::random_device{}()) {
    m_rooms.reserve(m_numberOfRooms);
    for (int i = 1; i <= m_numberOfRooms; ++i) {
        m_rooms.emplace_back(i);
    }
    m_roundGuests.reserve(m_numGuestsPerRound);
    m_roundGuestsByID.reserve(m_numGuestsPerRound);
    m_outsiders.reserve(m_allGuests.size());

}

void Hotel::loadAllGuests(const std::string& filepath) {
    m_allGuests.clear();

    std::ifstream in(filepath);
    if (!in) return;

    std::string line;
    std::getline(in, line);

    while (std::getline(in, line)) {
        Guest g;
        std::istringstream iss(line);

        std::getline(iss, g.dateOfBirth.month, '\t');
        std::getline(iss, g.dateOfBirth.day,   '\t');
        std::getline(iss, g.dateOfBirth.year,  '\t');
        std::getline(iss, g.name,              '\t');
        std::getline(iss, g.guestID,           '\t');
        std::getline(iss, g.portraitID,        '\t');

        m_allGuests.push_back(g);
    }
}

void Hotel::selectRoundGuests() {
    m_roundGuests.clear();
    m_roundGuestsByID.clear();
    m_guestsInside.clear();
    m_roomNumbersByID.clear();
    m_outsiders.clear(); 

    clearRooms();
    m_badgeIndex = 0;

    std::vector<int> idx(m_allGuests.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::shuffle(idx.begin(), idx.end(), m_rng);

    for (int i = 0; i < m_numGuestsPerRound; ++i) {
        Guest& g = m_allGuests[idx[i]];
        m_roundGuests.push_back(g);
        m_roundGuestsByID[g.guestID] = static_cast<int>(m_roundGuests.size()) - 1;
    }

    for (auto& g : m_allGuests) {
        if (!m_roundGuestsByID.count(g.guestID)) {
            m_outsiders.push_back(g.guestID);
        }
    }
}

void Hotel::clearRooms() {
    for (auto& room : m_rooms) room.vacate();
}

std::vector<Guest>& Hotel::getRoundGuests() {
    return m_roundGuests; 
}

int Hotel::isValidID(const std::string& guestID) const {
    if (guestID.size() != 8) return 2;
    for (char ch : guestID) {
        if (!std::isdigit(static_cast<unsigned char>(ch))) return 2;
    }
    if (!m_roundGuestsByID.count(guestID)) return 1; 
    return 0;
}

Hotel::BadgeError Hotel::verifyBadge(const Badge& b, const LivePerson& live) {
    int valid = isValidID(b.guestID);
    if (valid == 2) return BadgeError::BAD_FORMAT;  
    if (valid == 1) return BadgeError::UNKNOWN_ID;
    if (m_guestsInside.count(b.guestID)) return BadgeError::ALREADY_INSIDE;
    if (b.portraitID != live.observedPortraitID) return BadgeError::PORTRAIT_MISMATCH;
    return BadgeError::NONE;
}

int Hotel::numberOfOccupied() const {
    return static_cast<int>(m_roomNumbersByID.size());
}

int Hotel::assignRoom(const std::string& guestID) {
    if (numberOfOccupied() == m_numberOfRooms) return -1;

    int roomNumber = 1;
    for (auto& room : m_rooms) {
        if (room.isEmpty()) {
            room.assignGuest(guestID);
            m_roomNumbersByID.insert({ guestID, roomNumber });
            return roomNumber;
        }
        ++roomNumber;
    }
    return -1;
}

bool Hotel::enter(const std::string& guestID) {
    if (assignRoom(guestID) == -1) return false;
    m_guestsInside.insert(guestID);
    return true;
}

bool Hotel::exit(const std::string& guestID) {
    if (!m_guestsInside.count(guestID)) return false;
    m_guestsInside.erase(guestID);
    return true;
}

bool Hotel::vacate(const std::string& guestID) {
    auto it = m_roomNumbersByID.find(guestID);
    if (it == m_roomNumbersByID.end()) return false;

    int roomNumber = it->second;
    m_rooms[roomNumber - 1].vacate();

    m_roomNumbersByID.erase(it);
    m_guestsInside.erase(guestID);
    return true;
}

Claimer* Hotel::loadClaimer() {
    if (m_currClaimer) return &*m_currClaimer;
    if (m_badgeIndex >= static_cast<int>(m_roundGuests.size())) return nullptr;

    const Guest& g = m_roundGuests[m_badgeIndex++];
    Badge b = makeBadgeFor(g);
    LivePerson live = makeLiveFor(g);

    m_currClaimer.emplace();
    m_currClaimer->badge = std::move(b);
    m_currClaimer->live  = std::move(live);

    injectAnomaly(*m_currClaimer);
    return &*m_currClaimer;
}

void Hotel::resolveClaimer() {
    m_currClaimer.reset();
}

bool Hotel::syncBadgeFromRound(const std::string& id, Badge& b) {
    auto it = m_roundGuestsByID.find(id);
    if (it == m_roundGuestsByID.end()) return false;
    const Guest& g = m_roundGuests[it->second];
    b.name       = g.name;
    b.portraitID = g.portraitID;
    return true;
}

std::string Hotel::pickRandomID(const std::unordered_set<std::string>& s, std::mt19937& rng) {
    std::uniform_int_distribution<size_t> di(0, s.size() - 1);
    size_t target = di(rng);
    auto it = s.begin();
    std::advance(it, target);
    return *it;
}

LivePerson Hotel::makeLiveFor(const Guest& g) {
    LivePerson live;
    live.observedPortraitID = g.portraitID;
    return live;
}

Badge Hotel::makeBadgeFor(const Guest& g) {
    Badge b;
    b.guestID   = g.guestID;
    b.name      = g.name;
    b.portraitID= g.portraitID;
    return b;
}

void Hotel::injectAnomaly(Claimer& c) {
    std::uniform_int_distribution<int> d(0, 99);
    int r = d(m_rng);

    if (r < 50) return;

    if (r < 66) {
        if (!m_outsiders.empty()) {
            std::uniform_int_distribution<size_t> oi(0, m_outsiders.size() - 1);
            std::string outsiderID = m_outsiders[oi(m_rng)];
            c.badge.guestID = outsiderID;
            std::uniform_int_distribution<int> p(0, 99);
            if (p(m_rng) < 80) {
                for (const auto& g : m_allGuests) {
                    if (g.guestID == outsiderID) {
                        c.badge.name = g.name;
                        break;
                    }
                }
            }
            return;
        }
    }


    if (r < 82) {
        std::uniform_int_distribution<int> flip(0, 2);
        int f = flip(m_rng);

        if (f == 0) {
            int r2 = d(m_rng);
            if (r2 < 20) {
                std::uniform_int_distribution<int> a(0, 35);
                int val = a(m_rng);
                if (val < 26) c.badge.guestID += static_cast<char>('A' + val);
                else          c.badge.guestID += static_cast<char>('0' + (val - 26));
            } else {
                std::uniform_int_distribution<int> digit(0, 9);
                c.badge.guestID += static_cast<char>('0' + digit(m_rng));
            }
        } else if (f == 1) {
            std::uniform_int_distribution<size_t> pos(0, c.badge.guestID.size() - 1);
            c.badge.guestID.erase(pos(m_rng), 1);
        } else {
            if (!c.badge.guestID.empty()) {
                std::uniform_int_distribution<size_t> idx(0, c.badge.guestID.size() - 1);
                size_t i = idx(m_rng);
                int r3 = d(m_rng);
                if (r3 < 20) {
                    std::uniform_int_distribution<int> a(0, 35);
                    int val = a(m_rng);
                    if (val < 26) c.badge.guestID[i] = static_cast<char>('A' + val);
                    else          c.badge.guestID[i] = static_cast<char>('0' + (val - 26));
                } else {
                    std::uniform_int_distribution<int> digit(0, 9);
                    char ch;
                    do { ch = static_cast<char>('0' + digit(m_rng)); }
                    while (ch == c.badge.guestID[i]);
                    c.badge.guestID[i] = ch;
                }
            }
        }
        return;
    }


    if (r < 100) {
        const std::string correct = c.live.observedPortraitID;
        const std::string onBadge = c.badge.portraitID;

        std::vector<std::string> pool;
        pool.reserve(m_allGuests.size());
        for (const auto& g : m_allGuests) {
            const std::string& pid = g.portraitID;
            if (pid != correct && pid != onBadge) pool.push_back(pid);
        }
        if (pool.empty()) return;

        std::uniform_int_distribution<size_t> pi(0, pool.size() - 1);
        c.live.observedPortraitID = pool[pi(m_rng)];
    }
}

std::string Hotel::badgeErrToString(BadgeError err) {
    switch (err) {
        case BadgeError::BAD_FORMAT:        return "ID WAS INVALID";
        case BadgeError::ALREADY_INSIDE:    return "GUEST WAS ALREADY INSIDE";
        case BadgeError::PORTRAIT_MISMATCH: return "PORTRAIT WAS INVALID";
        case BadgeError::UNKNOWN_ID:        return "NOT ON TODAY'S LIST";
        default:                            return "CLEAN";
    }
}

Hotel::DecisionResult Hotel::resolveDecision(DeskDecision decision) {
    DecisionResult res{ false, "CLEAN", "" };
    if (!m_currClaimer) return res;

    res.guestID = m_currClaimer->badge.guestID;
    BadgeError err = verifyBadge(m_currClaimer->badge, m_currClaimer->live);

    if (err == BadgeError::NONE) {
        res.correct = (decision == DeskDecision::APPROVE);
    } else {
        res.correct = (decision == DeskDecision::FLAG);
    }

    res.reason = badgeErrToString(err);
    if (res.correct && err == BadgeError::NONE) enter(res.guestID);

    resolveClaimer();
    return res;
}
