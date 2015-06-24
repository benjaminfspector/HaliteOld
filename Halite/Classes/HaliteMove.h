#ifndef HALITEMOVE_H
#define HALITEMOVE_H

class HaliteMove
{
public:
    enum moveTypes
    {
        STILL,
        NORTH,
        SOUTH,
        EAST,
        WEST
    };
    
    moveTypes moveType;
    short x, y;
    
    HaliteMove(moveTypes m, short xx, short yy);
    HaliteMove();
};

#endif