#pragma once

#include "glad/glad.h"
#include "unordered_map"
#include "glm/glm.hpp"
#include "vector"
using namespace glm;

class Mesh {
    bool has_index_buffer = false;
    unsigned int index_count = 0;
    unsigned int vertex_count = 0;
    unsigned int vao;
    unsigned int ebo;
    std::unordered_map<unsigned int, unsigned int> buffers;
public:
    Mesh();

    void setBuffer(unsigned int position, const std::vector<int> &data);

    ~Mesh();

    void bind() const;

    void unbind() const;

    void setIndices(const std::vector<unsigned int> &data);

    void setBuffer(unsigned int position, const std::vector<unsigned int> &data);

    void setBuffer(unsigned int position, const std::vector<float> &data);

    void setBuffer(unsigned int position, const std::vector<vec2> &data);

    void setBuffer(unsigned int position, const std::vector<vec3> &data);

    void setBuffer(unsigned int position, const std::vector<vec4> &data);

    void setBuffer(unsigned int position, const float *data, unsigned int data_count, unsigned int byte_size,
                   unsigned int count_per_vertex);

    unsigned int getVBO(unsigned int position);

    bool hasIndexBuffer();

    unsigned int getIndexCount();

    unsigned int getVertexCount();
};
