//
// Created by User on 29-Jul.-2020.
//

#include "Octree.h"

#define RIGHT 4
#define TOP 2
#define BACK 1

#define EMPTY_CHILDREN {-1, -1, -1, -1, -1, -1, -1, -1}

Octree::Octree(data_type value) {
    data.push_back({value, -1, EMPTY_CHILDREN});
}

void Octree::insert(data_type value, int x, int y, int z, int depth) {
    int i = 0;
    node *current_node = &data[0];
    for (int d = 1; d < depth; ++d) {
        int radius = pow(2, d - 1);
        int child = (x > radius ? RIGHT : 0) | (y > radius ? TOP : 0) | (z > radius ? BACK : 0);
        if (current_node->children[child] == -1) {
            current_node->children[child] = data.size();
            data.push_back({current_node->value, i, EMPTY_CHILDREN});
            i = data.size() - 1;
        } else {
            i = current_node->children[child];
        }
        current_node = &data[i];
    }
    current_node->value = value;
}

std::vector<node> &Octree::getData() {
    return data;
}

