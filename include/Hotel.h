#ifndef HOTEL_H
#define HOTEL_H

#include "Room.h"
#include "Badge.h"
#include "Guest.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <random>
#include <cassert>
#include <algorithm>
#include <numeric>
#include <random>
#include <iostream>
#include <unordered_map>

class Hotel {
public:
    Hotel(); 
    void loadAllGuests(const std::string& filepath);
    void selectRoundGuests();
    int numberOfOccupied() const; 
    int assignRoom(const std::string& guestID); 
    int isValidID(const std::string& guestID) const;
    void clearRooms(); 
    void displayRoundGuests(); 

    enum class BadgeError {NONE, BAD_FORMAT, UNKNOWN_ID, ALREADY_INSIDE, PORTRAIT_MISMATCH};
    BadgeError verifyBadge(const Badge&b , const LivePerson& live);

    bool enter(const std::string& guestID);
    bool exit(const std::string& guestID);
    bool vacate(const std::string& guestID); 

    Claimer* loadClaimer(); 
    void resolveClaimer(); 
    bool syncBadgeFromRound(const std::string& id, Badge& b); 
    std::string pickRandomID(const std::unordered_set<std::string>& s, std::mt19937& rng);
    LivePerson makeLiveFor(const Guest& g);
    Badge makeBadgeFor(const Guest& g);

    void injectAnomaly(Claimer& c);


private:
    const int m_numberOfRooms = 5; 
    int m_numGuestsPerRound = 10;
    std::vector<Room> m_rooms;
    std::vector<Guest> m_allGuests;
    std::vector<Guest> m_roundGuests; 

    std::mt19937 m_rng;


    std::unordered_map<std::string, int> m_roundGuestsByID;
    std::unordered_map<std::string, int> m_roomNumbersByID;
    std::unordered_set<std::string> m_guestsInside;

    std::optional<Claimer> m_currClaimer; 
    int m_badgeIndex = 0;
};

#endif // HOTEL_H 