//
// Created by User on 29-Jul.-2020.
//

#include "Octree.h"

Octree::Octree(ui32 default_value) {
    data.push_back({0, -default_value, ROOT});
}

void Octree::insert(ui32 value, int x, int y, int z, int depth) {
    ui32 current = 0;
    ui8 child;
    for (int d = 1; d <= depth; ++d) {
        int radius = 1 << depth - d;
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
        //if child is a leaf
        if (data[current].children[child] & LEAF_MASK) {
            //add new node
            if (free.empty()) {
                data.push_back({current, data[current].children[child], child});
                data[current].children[child] = (data.size()-1);
            } else {
                data[free.top()] = {current, data[current].children[child], child};
                data[current].children[child] = free.top();
                free.pop();
            }
        }
        current=data[current].children[child];
    }
    //if it is a node clear it.
    if (data[current].children[child] > 0) {
        clear(data[current].children[child]);
    }
    data[current].children[child] = value ^ LEAF_MASK;
}

void Octree::clear(ui32 i) {

}

std::vector<node> &Octree::getData() {
    return data;
}

void Octree::remove(ui32 i) {
    node& n = data[data.size()-1];
    data[i]=data[data.size()-1];
    data[n.parent].children[n.node_type]=i;
    data.pop_back();
    for (int j = 0; j < 8; ++j) {
        if(!(n.children[j] & LEAF_MASK)){
            remove(n.children[j]);
        }
    }
}


