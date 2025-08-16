#ifndef GUEST_H
#define GUEST_H

#include <string>

struct Guest {
    struct Date {
        std::string month;
        std::string day;
        std::string year;
    };

    Date dateOfBirth;
    std::string name; 
    std::string guestID; 

    std::string portraitID; 
};

#endif // GUEST_H