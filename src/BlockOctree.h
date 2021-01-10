
#pragma once

#include "vector"
#include "list"
#define MAX_DEPTH 8
#define BLOCK_SIZE 16
#include "glm/glm.hpp"
typedef std::uint32_t ui32;
typedef std::uint16_t ui16;
typedef std::uint8_t ui8;
#define RIGHT 4
#define TOP 2
#define BACK 1

/*
 * This octree implmentation have blocks as leaves.Blocks are continuous array of materals.
 *
 * exemple of a tree of depth blocks at depth 2
 *
 * depth0              0
 * depth1          1       2
 * depth2       3     4 5     6
 * blocks:      0     1 2     3
 */
class BlockOctree {
public:
    struct Block {
        ui8 materials[BLOCK_SIZE][BLOCK_SIZE][BLOCK_SIZE]{};
    };
    struct Node {
        bool is_leaf;
        ui32 child_pointer;

        Node(bool is_leaf, ui32 child_pointer) : is_leaf(is_leaf), child_pointer(child_pointer) {}
    };
private:
    ui32 block_depth;
    std::vector<Node> nodes;
    std::vector<Block> blocks;
    std::vector<ui32> mat_count;
    std::vector<ui32> block_parents;
    std::list<ui32> free_blocks;


    bool node_cached=false;
    ui32 cached_node_id=0;
    ui32 cached_node_position_x=0;
    ui32 cached_node_position_y=0;
    ui32 cached_node_position_z=0;

public:
    BlockOctree(ui32 block_depth);

    ui32 getSize();
    ui32 getSizeAt(ui32 depth);

    ui32 getBlockSize();

    ui32 getBlockNodeID(ui32 x, ui32 y, ui32 z);
    ui32 getBlockID(ui32 block_node_id);
    void removeBlock(ui32 block_id);
    void setBlock(Block block, ui32 x, ui32 y, ui32 z);
    void setBlockMaterial(ui32 block_id,ui32 x,ui32 y,ui32 z,ui8 material);
    void setMaterial(ui8 material,ui32 block_x,ui32 block_y,ui32 block_z,ui32 x,ui32 y,ui32 z);
    std::vector<Block>& getBlocks();
    std::vector<Node>& getNodes();
};


