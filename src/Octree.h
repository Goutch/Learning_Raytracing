#pragma once
#include "vector"
#include "queue"

#include "glm/glm.hpp"
using namespace glm;
typedef vec3 data_type;
struct node{
    vec3 value;

    int parent;
    bool leaf;
    int children[8];
};
class Octree {
    std::vector<node> data;
    std::queue<int> free;
    data_type at(int x,int y,int z,int depth);

    void compress(int i);
    void clear(int i);
public:
    Octree(data_type value);

    void insert(data_type value,int x,int y,int z,int depth);

    std::vector<node>& getData();
};
