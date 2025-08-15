#ifndef ROOM_H
#define ROOM_H

class Room {
public:
    Room(int roomNumber);
    bool isEmpty(); 
    void assignGuest(int guestID);
    void vacate(); 
private: 
    int m_number;
    int m_currentGuestID;
    bool m_isOccupied;
};

#endif // ROOM_H