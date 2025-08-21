#include "Hotel.h"

Hotel::Hotel() : m_rng(std::random_device{}()) {
    for (int i = 1; i <= m_numberOfRooms; i++) {
        m_rooms.push_back(Room(i));
    }

    m_roundGuests.reserve(m_numGuestsPerRound);
    m_roundGuestsByID.reserve(m_numGuestsPerRound);
}

void Hotel::loadAllGuests(const std::string& filepath) {
    m_allGuests.clear();

    std::ifstream in(filepath);
    if (!in) return;

    std::string currLine;
    std::getline(in, currLine);

    while (std::getline(in, currLine)) {
        Guest g; 
        std::istringstream iss(currLine);
        
        std::getline(iss, g.dateOfBirth.month, '\t');
        std::getline(iss, g.dateOfBirth.day, '\t');
        std::getline(iss, g.dateOfBirth.year, '\t');
        std::getline(iss, g.name, '\t');
        std::getline(iss, g.guestID, '\t');
        std::getline(iss, g.portraitID, '\t');
        
        m_allGuests.push_back(g);
    }
}

int Hotel::isValidID(const std::string& guestID) const {
    if (!m_roundGuestsByID.count(guestID)) {
        return 1; 
    }
    else if (guestID.size() != 8) {
        return 2; 
    }

    for (char guestChar : guestID) {
        if (!isdigit(guestChar)) {
            return 2; 
        }
    }

    return 0; 
}

int Hotel::numberOfOccupied() const {
    return m_roomNumbersByID.size(); 
}

int Hotel::assignRoom(const std::string& guestID) {
    if (numberOfOccupied() == m_numberOfRooms) {
        return -1; 
    }

    int roomNumber = 1; 
    for (auto it = m_rooms.begin(); it != m_rooms.end(); it++) {
        if ((*it).isEmpty()) {
            (*it).assignGuest(guestID);
            m_roomNumbersByID.insert({guestID, roomNumber});
            return roomNumber; 
        }

        roomNumber++; 
    }

    return -1; 
}

void Hotel::clearRooms() {
    for (auto it = m_rooms.begin(); it != m_rooms.end(); it++) {
        (*it).vacate(); 
    }

}

void Hotel::displayRoundGuests() {
    for (auto it = m_roundGuests.begin(); it != m_roundGuests.end(); it++) {
        std::cout << "Name: " << (*it).name << " with GuestID: " << (*it).guestID << std::endl; 
    }
}


void Hotel::selectRoundGuests() {
    m_roundGuests.clear();
    m_roundGuestsByID.clear(); 
    m_guestsInside.clear();
    m_roomNumbersByID.clear(); 
    clearRooms();
    m_badgeIndex = 0;

    std::vector<int> indices(m_allGuests.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::shuffle(indices.begin(), indices.end(), m_rng);

    for (int i = 0; i < m_numGuestsPerRound; i++) {
        Guest& g = m_allGuests[indices[i]];
        m_roundGuests.push_back(g);
        m_roundGuestsByID[g.guestID] = m_roundGuests.size() - 1; 
    }

}

Hotel::BadgeError Hotel::verifyBadge(const Badge& b, const LivePerson& live) {
    int valid = isValidID(b.guestID);

    if (valid == 1) {
        return BadgeError::UNKNOWN_ID;
    }
    if (valid == 2) {
        return BadgeError::BAD_FORMAT;
    }
    if (m_guestsInside.count(b.guestID)) {
        return BadgeError::ALREADY_INSIDE;
    }

    if (b.portraitID != live.observedPortraitID) {
        return BadgeError::PORTRAIT_MISMATCH; 
    }

    return BadgeError::NONE;

}

bool Hotel::enter(const std::string& guestID) {
    int roomNum = assignRoom(guestID);
    if (roomNum == -1) {
        return false; 
    }

    m_guestsInside.insert(guestID);
    return true;
}

bool Hotel::exit(const std::string& guestID) {
    if (m_guestsInside.count(guestID)) {
        m_guestsInside.erase(guestID);
        return true; 
    }
    return false; 
}

bool Hotel::vacate(const std::string& guestID) {
    if (!m_roomNumbersByID.count(guestID)) {
        return false; 
    }

    auto it = m_roomNumbersByID.find(guestID);
    int roomNumber = it->second; 
    m_rooms[roomNumber-1].vacate();

    m_roomNumbersByID.erase(it);
    m_guestsInside.erase(guestID);
    return true; 
}

LivePerson Hotel::makeLiveFor(const Guest& g) {
    LivePerson live; 
    live.observedPortraitID = g.portraitID;

    return live; 
}

Badge Hotel::makeBadgeFor(const Guest& g) {
    Badge b;
    b.guestID = g.guestID;
    b.name = g.name; 
    b.portraitID = g.portraitID;

    return b; 
}

bool Hotel::syncBadgeFromRound(const std::string& id, Badge& b) {
    auto it = m_roundGuestsByID.find(id);
    if (it == m_roundGuestsByID.end()) return false;
    const Guest& g = m_roundGuests[it->second];
    b.name       = g.name;
    b.portraitID = g.portraitID;
    return true;
}


void Hotel::injectAnomaly(Claimer& c) {
    std::uniform_int_distribution<int> dist(0, 99);

    int r = dist(m_rng);

    if (r < 60) {
        return; 
    }

    if (r < 72) {
        if (!m_guestsInside.empty()) {
            std::string stolenID = pickRandomID(m_guestsInside, m_rng);
            c.badge.guestID = stolenID; 

            if (dist(m_rng) < 5) {
                syncBadgeFromRound(stolenID, c.badge);
            }

            return; 
        }
    }

    if (r < 86) {
        std::uniform_int_distribution<int> appendOrRemove(0, 1);
        if (appendOrRemove(m_rng) == 0) {
            int r2 = dist(m_rng);
            if (r2 < 20) {
                    std::uniform_int_distribution<int> dist(0, 35); 
                    int val = dist(m_rng);

                    if (val < 26) {
                        c.badge.guestID += static_cast<char>('A' + val);  // A–Z
                    } 
                    else {
                        c.badge.guestID += static_cast<char>('0' + (val - 26)); // 0–9
                    }
            }
            else {
                std::uniform_int_distribution<int> dist1(0, 9);
                int val1 = dist1(m_rng);
                c.badge.guestID += static_cast<char> ('0' + val1);
            }
        }
        else {
            c.badge.guestID.erase(0, 1);
        }
        return; 
    }

    const std::string correctSprite = c.live.observedPortraitID; 
    const std::string badgePortrait = c.badge.portraitID;
    std::vector<std::string> pool;
    pool.reserve(m_allGuests.size());
    for (const auto& g : m_allGuests) {
        const std::string& pid = g.portraitID;
        if (pid != correctSprite && pid != badgePortrait) {
            pool.push_back(pid);
        }
    }

    if (pool.empty()) {
        return;
    }

    std::uniform_int_distribution<size_t> di(0, pool.size() - 1);
    c.live.observedPortraitID = pool[di(m_rng)];
    return;

}

Claimer* Hotel::loadClaimer() {
    if (m_currClaimer) {
        return &*m_currClaimer; 
    }

    if (m_badgeIndex >= m_roundGuests.size()) {
        return nullptr;
    }

    const Guest& g = m_roundGuests[m_badgeIndex];
    m_badgeIndex++;

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


std::string Hotel::pickRandomID(const std::unordered_set<std::string>& s, std::mt19937& rng) {
    std::uniform_int_distribution<size_t> di(0, s.size() - 1);
    size_t target = di(rng);
    auto it = s.begin(); std::advance(it, target);
    return *it;
}
