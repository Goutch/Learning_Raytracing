#pragma once
#include "vector"

typedef int data_type;

struct node{
    data_type value;
    int parent;
    int children[8];
};
class Octree {
    std::vector<node> data;

    data_type at(int x,int y,int z,int depth);

public:
    Octree(data_type value);

    void insert(data_type value,int x,int y,int z,int depth);

    std::vector<node>& getData();
};
