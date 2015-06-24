#include "HaliteLocation.h"

HaliteLocation::HaliteLocation(short o, bool isO)
{
    owner = o;
    isSentient = isO;
}

HaliteLocation::HaliteLocation()
{
    owner = 0;
    isSentient = false;
}