//
// Created by User on 29-Jul.-2020.
//

#include "StorageBuffer.h"
#include "glad/glad.h"
#include "vector"

StorageBuffer::StorageBuffer(std::vector<int> &data) {
    glGenBuffers(1, &ssbo_id);
   /* bind();
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * data.size(), data.data()​, GLenum usage);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo_id);
    unbind();*/
}

void StorageBuffer::bind() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_id);
}

void StorageBuffer::unbind() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
}
