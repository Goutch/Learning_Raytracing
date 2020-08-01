//
// Created by User on 29-Jul.-2020.
//

#include "Octree.h"

#define RIGHT 4
#define TOP 2
#define BACK 1
#define EMPTY -1
#define EMPTY_CHILDREN {-1, -1, -1, -1, -1, -1, -1, -1}

Octree::Octree(data_type default_value) {
    data.push_back({default_value, -1, true, EMPTY_CHILDREN});
}

void Octree::insert(data_type value, int x, int y, int z, int depth) {
    int current = 0;
    int child;
    for (int d = 1; d <= depth; ++d) {
        const int radius = pow(2, depth - d);
        bool x_greater = x >= radius;
        bool y_greater = y >= radius;
        bool z_greater = z >= radius;
        child = (x_greater * RIGHT) + (y_greater * TOP) + (z_greater * BACK);
        x -= radius * x_greater;
        y -= radius * y_greater;
        z -= radius * z_greater;
        if (data[current].children[child] == EMPTY) {
            data[current].children[child] = data.size();
            if (free.empty()) {
                data.push_back({data[current].value, current, true, EMPTY_CHILDREN});
            } else {
                data[free.front()] = {data[current].value, current, true, EMPTY_CHILDREN};
                free.pop();
            }

            data[current].leaf = false;
        }
        current = data[current].children[child];
    }
    data[current].value = value;
    //todo fix this
   // compress(data[current].parent);
}

void Octree::clear(int i) {
    if (data[i].leaf) {
        free.emplace(i);
    } else {
        for (int j = 1; j < 8; ++j) {
            clear(data[i].children[j]);
        }
    }
    for (int k = 0; k < 8; ++k) {
        data[i].children[k] = EMPTY;
    }
}

void Octree::compress(int i) {
    for (int j = 0; j < 7; ++j) {
        if (data[i].children[j] == EMPTY || data[i].children[j + 1] == EMPTY ||
            data[data[i].children[j]].value != data[data[i].children[j + 1]].value) {
            return;
        }
    }

    data[i].value = data[i].children[0];
    for (int k = 0; k < 8; ++k) {
        if (data[i].children[k] != EMPTY)
            clear(data[i].children[k]);
    }

    if (data[i].parent != EMPTY) {
        compress(data[i].parent);
    }
}

std::vector<node> &Octree::getData() {
    return data;
}


