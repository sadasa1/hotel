#ifndef HOTEL_H
#define HOTEL_H

#include "Room.h"
#include "Guest.h"
#include <vector>
#include <string>
#include <unordered_set>
#include <random>
#include <cassert>

class Hotel {
public:
    Hotel(); 
    void loadAllGuests(const std::string& filepath);
    void selectRoundGuests();
    int numberOfOccupied() const; 
    int assignRoom(const std::string& guestID); 
    bool isValidID(const std::string& guestID) const;

    enum class EnterError {NONE, UNKNOWN_ID, ALREADY_INSIDE, NO_ROOMS};
    EnterError enter(const std::string& guestID);
    bool exit(const std::string& guestID);
    bool vacate(const std::string& guestID); 

private:
    const int m_numberOfRooms = 5; 
    int m_numGuestsPerRound = 10;
    std::vector<Room> m_rooms;
    std::vector<Guest> m_allGuests;
    std::vector<Guest> m_roundGuests; 

    std::unordered_map<std::string, int> m_roundGuestsByID;
    std::unordered_map<std::string, int> m_roomNumbersByID;
    std::unordered_set<std::string> m_guestsInside;
};

#endif // HOTEL_H 