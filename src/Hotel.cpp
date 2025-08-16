#include "Hotel.h"
#include <fstream>
#include <sstream>

Hotel::Hotel() {
    for (int i = 1; i <= m_numberOfRooms; i++) {
        m_rooms.push_back(Room(i));
    }
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

bool Hotel::isValidID(const std::string& guestID) const {
    return m_roundGuestsByID.count(guestID);
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

void Hotel::selectRoundGuests() {
    m_roundGuests.clear();
    m_roundGuestsByID.clear(); 
    m_guestsInside.clear();
    m_roomNumbersByID.clear(); 

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(0, m_allGuests.size()-1);
    std::unordered_set<int> used;

    for (int i = 0; i < m_numGuestsPerRound; i++) {
        int idx  = dist(gen);
        if (!used.count(idx)) {
            used.insert(idx);
            Guest g = m_allGuests[idx];

            m_roundGuests.push_back(g);
            m_roundGuestsByID[g.guestID] = m_roundGuests.size()-1; 
        }
        else {
            // reroll
            i--;
        }
    }
}

Hotel::EnterError Hotel::enter(const std::string& guestID) {
    if (!isValidID(guestID)) {
        return EnterError::UNKNOWN_ID; 
    }

    if (m_guestsInside.count(guestID)) {
        return EnterError::ALREADY_INSIDE; 
    }

    if (!m_roomNumbersByID.count(guestID)) {
        int roomNum = assignRoom(guestID);

        if (roomNum == -1) {
            return EnterError::NO_ROOMS;
        }
    }

    m_guestsInside.insert(guestID);
    assert(m_roomNumbersByID.count(guestID));
    return EnterError::NONE; 
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