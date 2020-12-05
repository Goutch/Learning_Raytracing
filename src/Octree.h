#pragma once

#include "vector"
#include "queue"
#include "glm/glm.hpp"

using namespace glm;
typedef std::uint32_t ui32;
typedef std::uint16_t ui16;
typedef std::uint8_t ui8;

#define LEAF_FLAG 0x10000000
#define VALUE_MASK 0xEFFFFFFF
#define ROOT 8
#define RIGHT 4
#define TOP 2
#define BACK 1

enum NODE_TYPE {
    LDF = 0,
    LDB = BACK,
    LTF = TOP,
    LTB = TOP + BACK,
    RDF = RIGHT,
    RDB = RIGHT + BACK,
    RTF = RIGHT + TOP,
    RTB = RIGHT + TOP + BACK,
};

class Octree {
public:
    struct Node {
        ui32 node_type;
        ui32 children[8];

        Node(ui32 material, ui8 node_type) : node_type(node_type) {
            for (int i = 0; i < 8; ++i) {
                children[i] = material;
            }
        }
    };

private:
    const ui32 MAX_DEPTH = 8;
    std::vector<Octree::Node> data;

    void clearAndReplace(ui32 value, ui32 i, ui32 parent);

    bool isLeaf(ui32 i);

public:
    Octree(ui32 default_value);

    void set(ui32 value, ui32 x, ui32 y, ui32 z, ui32 depth);

    std::vector<Octree::Node> &getData();

    ui32 getDimentionAt(ui32 depth);

    ui32 setLeaf(ui32 node_value);

    ui32 getValue(ui32 node);

    ui32 setTree(ui32 node);


};
