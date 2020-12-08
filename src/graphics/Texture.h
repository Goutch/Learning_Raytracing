#pragma once


#include <string>

class Texture {
public:
    enum TEXTURE_TYPE {
        RGB8,
        RGBA8,
        RGBA32F,
        R8,
        DEPTH32,
    };
private:
    unsigned int texture_id;
    int width=1;
    int height=1;
public:
    Texture();

    ~Texture();
    void load(std::string path);
    void setData(void *data, int width, int height, TEXTURE_TYPE texture_type);

    void bind(unsigned int slot=0) const;

    void unbind(unsigned int slot=0) const;

    unsigned int getId();
};
