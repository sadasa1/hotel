#ifndef BADGE_H
#define BADGE_H

#include <string>

struct Badge {
    std::string guestID;
    std::string name; 
    std::string portraitID;
};

struct LivePerson {
    std::string observedPortraitID;
};

struct Claimer {
    Badge badge;
    LivePerson live; 
};

#endif // BADGE_H