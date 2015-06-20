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
    unsigned char x, y;
    
    HaliteMove(moveTypes m, unsigned char xx, unsigned char yy);
    HaliteMove();
};

#endif