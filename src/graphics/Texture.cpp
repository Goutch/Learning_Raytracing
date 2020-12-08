//
// Created by User on 04-Jun.-2020.
//

#include "Texture.h"
#include "glad/glad.h"
#include "fstream"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
Texture::Texture() {
    glGenTextures(1, &texture_id);
}

void Texture::setData(void *data, int width, int height, Texture::TEXTURE_TYPE texture_type) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    switch (texture_type) {
        case TEXTURE_TYPE::R8: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
            break;
        }
        case TEXTURE_TYPE::RGBA8: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            break;
        }
        case TEXTURE_TYPE::RGB8: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            break;
        }
        case TEXTURE_TYPE::RGBA32F: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, data);
            glBindImageTexture(0, texture_id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
            break;
        }
        case TEXTURE_TYPE::DEPTH32: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);
            break;
        }
    }

    unbind();
}
void Texture::load(std::string path) {
    std::ifstream file(path.c_str());
    if (file.good()) {
        unsigned char *buffer;
        int nb_channels;
        stbi_set_flip_vertically_on_load(true);
        buffer = stbi_load(path.c_str(), &width, &height, &nb_channels, 4);
        setData(buffer, width, height, TEXTURE_TYPE::RGBA8);
        if (buffer) {
            stbi_image_free(buffer);
        }
    }

}
void Texture::bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture_id);

}

void Texture::unbind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, 0);
}


unsigned int Texture::getId() {
    return texture_id;
}

Texture::~Texture() {
    glDeleteTextures(1, &texture_id);
}


