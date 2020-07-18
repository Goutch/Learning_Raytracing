#pragma once

#include <vector>
#include "Texture.h"

class Framebuffer {

    unsigned int fbo;
    unsigned int rbo;
    int width,height;

    unsigned int texture;
public:
    Framebuffer();
    Framebuffer(int width, int height);
    ~Framebuffer();

    void setResolution(int width,int height);
    void bindTexture() const;
    void unbindTexture() const;
    void bind() const;
    void unbind() const;
};
