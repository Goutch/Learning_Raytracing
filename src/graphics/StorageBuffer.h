#pragma once

//https://www.geeks3d.com/20140704/tutorial-introduction-to-opengl-4-3-shader-storage-buffers-objects-ssbo-demo/
#include "vector"
#include "PureOctree.h"
#include "glad/glad.h"
#include "vector"
template <class T>
class StorageBuffer {
    unsigned int ssbo_id;
    unsigned int bind_index;
public:
    void StorageBuffer::bind() {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_id);
    };

    void StorageBuffer::unbind() {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    };

    StorageBuffer::~StorageBuffer() {
        glDeleteBuffers(1,&ssbo_id);
    };
    void setData(std::vector<T>& data)
    {
        bind();
        glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(T)*data.size(),data.data(),GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bind_index, ssbo_id);

        unbind();
    }
    StorageBuffer(std::vector<T>& data,unsigned int bind_index){
        glGenBuffers(1, &ssbo_id);
        this->bind_index=bind_index;
        setData(data);
    };

};
