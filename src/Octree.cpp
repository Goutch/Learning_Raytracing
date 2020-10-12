//
// Created by User on 29-Jul.-2020.
//

#include "Octree.h"

Octree::Octree(ui32 default_value) {
    data.push_back({0, -default_value, ROOT});
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
            if (free.empty()) {
                data.push_back({current, data[current].children[child], child});
                data[current].children[child] = (data.size() - 1);
            } else {
                data[free.top()] = {current, data[current].children[child], child};
                data[current].children[child] = free.top();
                free.pop();
            }
        }
        current = data[current].children[child];
    }
    //if it is a node clearAndReplace it.
    if (!isLeaf(data[current].children[child])) {
        clearAndReplace(value, data[current].children[child]);
    } else {
        data[current].children[child] = value ^ LEAF_MASK;
    }
}

std::vector<node> &Octree::getData() {
    return data;
}

void Octree::clearAndReplace(ui32 value, ui32 i) {
    node &n = data[data.size() - 1];
    data[i] = data[data.size() - 1];
    data[n.parent].children[n.node_type] = i;
    data.pop_back();
    for (int j = 0; j < 8; ++j) {
        if (!isLeaf(n.children[j])) {
            clearAndReplace(value, n.children[j]);
        }
    }
    data[i].children[data[i].node_type] = setLeaf(value);
}

ui32 Octree::getDimentionAt(ui32 depth) {
    return 1 << depth;
}

bool Octree::isLeaf(ui32 node_index) {
    return node_index & LEAF_MASK;
}

ui32 Octree::setLeaf(ui32 node_value)
{
    return node_value ^ LEAF_MASK;
}


