#include "HaliteLocation.h"

HaliteLocation::HaliteLocation(unsigned char o, bool isO)
{
    owner = o;
    isSentient = isO;
}

HaliteLocation::HaliteLocation()
{
    owner = 0;
    isSentient = false;
}