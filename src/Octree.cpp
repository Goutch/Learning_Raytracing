//
// Created by User on 29-Jul.-2020.
//

#include "Octree.h"

#define RIGHT 4
#define TOP 2
#define BACK 1

#define EMPTY_CHILDREN {-1, -1, -1, -1, -1, -1, -1, -1}

Octree::Octree(data_type value) {
    data.push_back({vec3(0, 0, 0), -1, true, EMPTY_CHILDREN});
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
        if (data[current].children[child] == -1) {
            data[current].children[child] = data.size();
            data.push_back({data[current].value, current, true, EMPTY_CHILDREN});
            data[current].leaf = false;
        }
        current = data[current].children[child];
    }
    data[current].value = vec3(x, y, z);
    //compress(data[current].parent);
}

void Octree::clear(int i) {
    if (i == -1) {
        return;
    }
    if (data[i].leaf) {
        free.emplace(i);
    } else {
        for (int j = 1; j < 8; ++j) {
            clear(data[i].children[j]);
        }
    }
}

void Octree::compress(int i) {
    if (i == -1) {
        return;
    }
    bool need_compression = true;
    data_type value = data[data[i].children[0]].value;
    for (int j = 1; j < 8; ++j) {
        need_compression &= (value == data[data[i].children[j]].value);
    }
    if (need_compression) {
        clear(i);
        data[i].value = value;
        compress(data[i].parent);
    }
}

std::vector<node> &Octree::getData() {
    return data;
}


