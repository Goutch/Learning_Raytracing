
#pragma once
#include "vector"
//store blocks of continuous memory in leaf instead of nodes

typedef std::uint32_t ui32;
typedef std::uint16_t ui16;
typedef std::uint8_t ui8;
class BlockOctree {
    struct Block{
        ui8 data[16*16*16];
    };
    struct Node
    {
        ui8 value_or_childmask;
        ui32 child_pointer;
    };
    std::vector<Node> nodes;
    std::vector<Block> blocks;

};


