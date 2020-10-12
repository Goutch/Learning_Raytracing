#pragma once

#include "vector"
#include "queue"
#include "glm/glm.hpp"

using namespace glm;
typedef std::uint32_t ui32;
typedef std::uint16_t ui16;
typedef std::uint8_t ui8;

#define LEAF_MASK 0x10000000
#define VALUE_MASK 0xEFFFFFFF
#define ROOT 8
#define RIGHT 4
#define TOP 2
#define BACK 1

enum NODE_TYPE {
    R = ROOT,
    LDF = 0,
    LDB = BACK,
    LTF = TOP,
    LTB = TOP + BACK,
    RDF = RIGHT,
    RDB = RIGHT + BACK,
    RTF = RIGHT + TOP,
    RTB = RIGHT + TOP + BACK,
};

struct node {

    ui32 node_type;
    ui32 parent;
    ui32 children[8];


    node(ui32 parent, ui32 value, ui32 node_type) : parent(parent), node_type(node_type) {
        for (int i = 0; i < 8; ++i) {
            children[i] = value;
        }
    }
};

class Octree {
    std::vector<node> data;
    std::priority_queue<ui32, std::vector<ui32>, std::greater<>> free;
    void clearAndReplace(ui32 value, ui32 i);
    ui32 getDimentionAt(ui32 depth);
    bool isLeaf(ui32 i);
public:
    Octree(ui32 default_value);
    void set(ui32 value, ui32 x, ui32 y, ui32 z, ui32 depth);

    std::vector<node> &getData();

    ui32 setLeaf(ui32 node_value);
};
