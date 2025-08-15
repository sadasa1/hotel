#ifndef HOTEL_H
#define HOTEL_H

#include "Room.h"
#include "Guest.h"
#include <vector>

class Hotel {
public:
    Hotel(); 

private:
    int numberOfRooms = 10; 
    std::vector<Room> m_rooms;
    std::vector<Guest> m_guests; 
};

#endif // HOTEL_H 