#ifndef ROOM_H
#define ROOM_H

#include <string>

class Room {
public:
    Room(int roomNumber);
    bool isEmpty(); 
    void assignGuest(std::string guestID);
    void vacate(); 
private: 
    int m_number;
    std::string m_currentGuestID;
    bool m_isOccupied;
};

#endif // ROOM_H