#pragma once
#include "vector"
class StorageBuffer {
    unsigned int ssbo_id;

    StorageBuffer(std::vector<int>& data);
    void bind();
    void unbind();
};
