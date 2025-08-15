#ifndef GUEST_H
#define GUEST_H

#include <string>

struct Guest {
    struct Date {
        int year;
        int month;
        int day;
    };

    Date dateOfBirth;
    std::string name; 
    int guestID; 


    std::string portraitID; 
};

#endif // GUEST_H