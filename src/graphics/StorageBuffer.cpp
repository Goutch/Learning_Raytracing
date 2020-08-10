//
// Created by User on 29-Jul.-2020.
//

#include "StorageBuffer.h"
#include "glad/glad.h"
#include "vector"
//https://www.geeks3d.com/20140704/tutorial-introduction-to-opengl-4-3-shader-storage-buffers-objects-ssbo-demo/
StorageBuffer::StorageBuffer(std::vector<node> &data) {
    glGenBuffers(1, &ssbo_id);
    bind();
    glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(node)*data.size(),data.data(),GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_id);

    unbind();
}

void StorageBuffer::bind() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_id);
}

void StorageBuffer::unbind() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

StorageBuffer::~StorageBuffer() {
    glDeleteBuffers(1,&ssbo_id);
}
