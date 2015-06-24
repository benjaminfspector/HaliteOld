#include "HaliteMove.h"

HaliteMove::HaliteMove(moveTypes m, short xx, short yy)
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