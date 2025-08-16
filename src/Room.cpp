#include "Room.h"

Room::Room(int roomNumber) : m_number(roomNumber), m_currentGuestID("-1"), m_isOccupied(false) { }

bool Room::isEmpty() {
    return m_isOccupied;
}

void Room::assignGuest(std::string guestID) {
    m_currentGuestID = guestID; 
    m_isOccupied = true; 
}

void Room::vacate() {
    m_currentGuestID = -1;
    m_isOccupied = false; 
}
