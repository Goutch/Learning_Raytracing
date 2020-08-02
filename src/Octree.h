#pragma once
#include "vector"
#include "queue"
#include "glm/glm.hpp"
using namespace glm;
typedef int data_type;
struct node{
    data_type value;

    int parent;
    bool leaf;
    int children[8];
};
class Octree {
    std::vector<node> data;
    std::priority_queue<int,std::vector<int>, std::greater<>> free;
    data_type at(int x,int y,int z,int depth);

    void compress(int i);
    void clear(int i);
public:
    Octree(data_type default_value);

    void insert(data_type value,int x,int y,int z,int depth);

    std::vector<node>& getData();
};
