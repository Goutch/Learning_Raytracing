#pragma once
#include "Texture.h"

class Image : public Texture {

public:
    void bind(unsigned int slot) const override;

    void unbind(unsigned int slot) const override;
};


