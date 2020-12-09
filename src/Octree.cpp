//
// Created by User on 29-Jul.-2020.
//

#include "Octree.h"

Octree::Octree(ui32 default_value) {
    data.emplace_back(setLeaf(default_value));
    index_stack[0] = 0;
}

void Octree::optimize() {

}

void Octree::set(ui32 material, ui32 x, ui32 y, ui32 z, ui32 depth) {

    index_stack[0] = 0;
    int d = 1;
    for (d = 1; d <= depth; ++d) {
        int radius = getDimentionAt(depth - d);
        bool x_greater = x >= radius;
        bool y_greater = y >= radius;
        bool z_greater = z >= radius;
        position_stack[d] = (x_greater * RIGHT) + (y_greater * TOP) + (z_greater * BACK);
        if (d == depth) {
            break;
        }
        x -= radius * x_greater;
        y -= radius * y_greater;
        z -= radius * z_greater;
        //if current node is leaf and depth is not the target
        if (d != depth && isLeaf(data[index_stack[d - 1]].children[position_stack[d]])) {
            //return if parent is already correct material
            if (getValue(data[index_stack[d - 1]].children[position_stack[d]]) == material) {
                return;
            }
            data.emplace_back(data[index_stack[d - 1]].children[position_stack[d]]);
            data[index_stack[d - 1]].children[position_stack[d]] = data.size() - 1;
            last_node_position=position_stack[d];
        }
        index_stack[d] = data[index_stack[d - 1]].children[position_stack[d]];

    }
    data[index_stack[d - 1]].children[position_stack[d]] = setLeaf(material);
}

std::vector<Octree::Node> &Octree::getData() {
    return data;
}
void Octree::remove(ui32 depth) {

}
void Octree::clearAndReplace(ui32 material, ui32 depth) {
    //if is not a leaf remove node
    if(!isLeaf(index_stack[depth]))
    {
        for (int i = 0; i < 8; ++i) {
            position_stack[depth+1]=i;
            remove(depth+1);
        }
        Node &n = data[data.size() - 1];
        data[index_stack[depth]] = data[data.size() - 1];
        data[index_stack[depth-1]].children[last_node_position] = index_stack[depth];
        data.pop_back();
        for (int j = 0; j < 8; ++j) {
            if (!isLeaf(n.children[j])) {
                position_stack[depth + 1] = j;
                clearAndReplace(material, depth + 1);
            }
        }
    }
    //set parent pointer to this leaf
    data[index_stack[depth - 1]].children[position_stack[depth]] = setLeaf(material);

}

ui32 Octree::getDimentionAt(ui32 depth) {
    return 1 << depth;
}

bool Octree::isLeaf(ui32 node) {
    return node & LEAF_FLAG;
}

ui32 Octree::getValue(ui32 node) {
    return node & VALUE_MASK;
}

ui32 Octree::setLeaf(ui32 node) {
    return node ^ LEAF_FLAG;
}

ui32 Octree::setTree(ui32 node) {
    return node & VALUE_MASK;
}

bool Octree::combine(ui32 depth) {
    if (depth == 0) return false;
    if (depth == MAX_DEPTH)return true;
    ui32 material;
    for (int i = 0; i < 8; ++i) {

        index_stack[depth + 1] = data[index_stack[depth]].children[i];
        //combine child if not a leaf if cant be combined then exit
        if (!isLeaf(index_stack[depth + 1]) && !combine(depth + 1)) {
            return false;
        }
        if (i == 0) {
            material = getValue(index_stack[depth + 1]);
        } else if (material != getValue(index_stack[depth + 1])) {
            return false;
        }
    }
    clearAndReplace(material, depth);
    return true;
}






