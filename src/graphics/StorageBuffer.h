#pragma once
#include "vector"
#include "octree.h"
class StorageBuffer {
    unsigned int ssbo_id;
public:
    void bind();
    void unbind();


    StorageBuffer(std::vector<node>& data);
    ~StorageBuffer();
};
