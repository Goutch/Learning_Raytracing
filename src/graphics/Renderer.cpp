//
// Created by User on 02-Jun.-2020.
//
#define GLFW_INCLUDE_NONE

#include "Renderer.h"
#include "glad/glad.h"

#include "GLFW/glfw3.h"
#include "Mesh.h"
#include "GL_Debug.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Configs.h"

void Renderer::render(Texture *texture, ShaderProgram *shaderProgram) {
    shaderProgram->bind();
    texture->bind();
    quad->bind();
    quad->hasIndexBuffer() ?
    glDrawElements(GL_TRIANGLES, quad->getIndexCount(), GL_UNSIGNED_INT, 0) :
    glDrawArrays(GL_TRIANGLES, 0, quad->getVertexCount());
    quad->unbind();
    texture->unbind();
    shaderProgram->unbind();
}

GLFWwindow *Renderer::init() {
    createWindow(WIDTH, HEIGHT, "TRACER");
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to load glad" << std::endl;
    }
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    GL_Debug::init();
    quad = new Mesh();

    std::vector<vec3> vertex_positions = {
            vec3(0.5f, -0.5f, .0f),
            vec3(0.5f, 0.5f, .0f),
            vec3(-0.5f, 0.5f, .0f),
            vec3(-0.5f, -0.5, .0f)
    };
    quad->setBuffer(0, vertex_positions);

    std::vector<vec2> vertex_uvs = {
            vec2(1.0f, 0.0f),
            vec2(1.0f, 1.0f),
            vec2(0.0f, 1.0f),
            vec2(0.0f, 0.0f),
    };
    quad->setBuffer(1, vertex_uvs);

    std::vector<unsigned int> indices = {
            0, 1, 2,
            2, 3, 0,
    };
    quad->setIndices(indices);
    return window;
}

void Renderer::createWindow(int width, int height, std::string name) {
    if (!glfwInit()) {
        std::cout << "Failed to load glfw" << std::endl;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window);

}

void Renderer::clear() const {
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::terminate() {
    delete quad;
    glfwTerminate();
}











