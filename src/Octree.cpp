//
// Created by User on 29-Jul.-2020.
//

#include "Octree.h"

Octree::Octree(ui32 default_value) {
    data.push_back({setLeaf(default_value), ROOT});
}

void Octree::set(ui32 value, ui32 x, ui32 y, ui32 z, ui32 depth) {
    ui32 current = 0;
    ui8 child;
    for (int d = 1; d <= depth; ++d) {
        int radius = getDimentionAt(depth - d);
        bool x_greater = x >= radius;
        bool y_greater = y >= radius;
        bool z_greater = z >= radius;
        child = (x_greater * RIGHT) + (y_greater * TOP) + (z_greater * BACK);
        if (d == depth) {
            break;
        }
        x -= radius * x_greater;
        y -= radius * y_greater;
        z -= radius * z_greater;

        if (isLeaf(data[current].children[child])) {
            //add new node
            data.emplace_back(data[current].children[child], child);
            data[current].children[child] = (data.size() - 1);
        }
        current = data[current].children[child];
    }
    //if it is a node clearAndReplace it.
    if (!isLeaf(data[current].children[child])) {
        clearAndReplace(value, data[current].children[child], current);
    } else {
        data[current].children[child] = setLeaf(value);
    }
}

std::vector<Octree::Node> &Octree::getData() {
    return data;
}

void Octree::clearAndReplace(ui32 material, ui32 index, ui32 parent_index) {
    Node &n = data[data.size() - 1];
    data[index] = data[data.size() - 1];
    data[parent_index].children[n.node_type] = index;
    data.pop_back();
    for (int j = 0; j < 8; ++j) {
        if (!isLeaf(n.children[j])) {
            clearAndReplace(material, n.children[j], n.children[j]);
        }
    }
    data[index].children[data[index].node_type] = setLeaf(material);
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


