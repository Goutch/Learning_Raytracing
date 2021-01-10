//
// Created by User on 11-Dec.-2020.
//

#include "BlockOctree.h"

BlockOctree::BlockOctree(ui32 block_depth) {
    this->block_depth = block_depth < MAX_DEPTH ? block_depth : MAX_DEPTH;
    nodes.emplace_back(true, 0);
}

ui32 BlockOctree::getSize() {
    return getSizeAt(0);
}

ui32 BlockOctree::getSizeAt(ui32 depth) {
    return 1 << (block_depth - depth);
}

ui32 BlockOctree::getBlockSize() { return BLOCK_SIZE; };

ui32 BlockOctree::getBlockNodeID(ui32 x, ui32 y, ui32 z) {
    if(node_cached&&x==cached_node_position_x&&y==cached_node_position_y&&z==cached_node_position_z)
    {
        return cached_node_id;
    }
    cached_node_position_x=x;
    cached_node_position_y=y;
    cached_node_position_z=z;
    ui32 index_stack[MAX_DEPTH];
    index_stack[0] = 0;
    ui32 depth;
    for (depth = 0; depth < block_depth; ++depth) {
        ui32 child_size = getSizeAt(depth + 1);
        bool right = x >= child_size;
        bool top = y >= child_size;
        bool back = z >= child_size;
        ui8 child = (right * RIGHT) + (top * TOP) + (back * BACK);
        //if child is leaf create set create children
        if (nodes[index_stack[depth]].is_leaf) {
            nodes[index_stack[depth]].child_pointer = nodes.size();
            for (int i = 0; i < 8; ++i) {
                nodes.emplace_back(true, 0);
            }
            nodes[index_stack[depth]].is_leaf = false;
        }

        index_stack[depth + 1] = nodes[index_stack[depth]].child_pointer + child;
        x -= right * child_size;
        y -= right * child_size;
        z -= right * child_size;
    }
    cached_node_id=index_stack[depth];
    return cached_node_id;
}

ui32 BlockOctree::getBlockID(ui32 block_node_id) {
    if (nodes[block_node_id].is_leaf) {
        if(free_blocks.empty())
        {
            nodes[block_node_id].child_pointer = blocks.size();
            mat_count.emplace_back();
            blocks.emplace_back();
        } else{
            nodes[block_node_id].child_pointer = free_blocks.front();
            free_blocks.pop_front();
        }
        nodes[block_node_id].is_leaf = false;
        block_parents[nodes[block_node_id].child_pointer] = block_node_id;
    }
    return nodes[block_node_id].child_pointer;
}

void BlockOctree::removeBlock(ui32 block_id) {
    free_blocks.push_back(block_id);
    nodes[block_parents[block_id]].is_leaf = true;
  //todo optimize tree by deleting nodes with all leaf children
}

void BlockOctree::setBlock(Block block, ui32 x, ui32 y, ui32 z) {
    ui32 block_node_id=getBlockNodeID(x, y, z);
    ui32 block_id=getBlockID(block_node_id);
    for (int x = 0; x < BLOCK_SIZE; ++x) {
        for (int z = 0; z < BLOCK_SIZE; ++z) {
            for (int y = 0; y <BLOCK_SIZE ; ++y) {
                if(block.materials[x][z][y]!=0)
                    mat_count[block_id]++;
            }
        }
    }
    blocks[block_id] = block;
}

void BlockOctree::setBlockMaterial(ui32 block_id, ui32 x, ui32 y, ui32 z, ui8 material) {
    ui8 temp = blocks[block_id].materials[x][z][y];
    blocks[block_id].materials[x][z][y] = material;
    if (temp == 0 && material != 0) {
        mat_count[block_id]++;
    } else if (temp != 0 && material == 0) {
        mat_count[block_id]--;
    }
    if (mat_count[block_id] == 0) {
        removeBlock(block_id);
    }
}

void BlockOctree::setMaterial(ui8 material, ui32 block_x, ui32 block_y, ui32 block_z, ui32 x, ui32 y, ui32 z) {
    setBlockMaterial(getBlockID(getBlockNodeID(block_x, block_y, block_z)), x, y, z, material);
}

std::vector<BlockOctree::Block> &BlockOctree::getBlocks() { return blocks; };

std::vector<BlockOctree::Node> &BlockOctree::getNodes() { return nodes; };