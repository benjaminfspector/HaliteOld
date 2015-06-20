#ifndef HALITELOCATION_H
#define HALITELOCATION_H

#include <iostream> //testing purpose
#include "LOpenGL.h"

class HaliteLocation
{
public:
    unsigned char owner; //0 = blank; other indicates playerTag;
    bool isSentient;
    
    HaliteLocation(unsigned char o, bool isO);
    HaliteLocation();
};

#endif