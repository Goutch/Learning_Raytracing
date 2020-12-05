#pragma once

class ShaderProgram;

class Mesh;
class Texture;
#include <GLFW/glfw3.h>
#include <string>
#include "list"
#include "Framebuffer.h"

class Renderer{
private:
    Framebuffer* framebuffer;
    Mesh* quad;
private:
    GLFWwindow *window;
    void createWindow(int width, int height, std::string name);
public:
    void clear() const;

    GLFWwindow* init();
    void terminate();
    void present(Texture* texture, ShaderProgram* shaderProgram);
};

