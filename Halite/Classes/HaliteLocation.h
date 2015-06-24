#ifndef HALITELOCATION_H
#define HALITELOCATION_H

#include <iostream> //testing purpose
#include "LOpenGL.h"

class HaliteLocation
{
public:
    short owner; //0 = blank; other indicates playerTag;
    bool isSentient;
    
    HaliteLocation(short o, bool isO);
    HaliteLocation();
};

#endif