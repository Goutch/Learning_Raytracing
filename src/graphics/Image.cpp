
#include "Image.h"
#include "glad/glad.h"
void Image::bind(unsigned int slot) const {
    Texture::bind(slot);
    glBindImageTexture(slot, texture_id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void Image::unbind(unsigned int slot) const {
    Texture::unbind(slot);
    glBindImageTexture(slot, texture_id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}
