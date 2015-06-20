#include "HaliteMove.h"

HaliteMove::HaliteMove(moveTypes m, unsigned char xx, unsigned char yy)
{
    moveType = m;
    x = xx;
    y = yy;
}

HaliteMove::HaliteMove()
{
    moveType = STILL;
    x = 0;
    y = 0;
}