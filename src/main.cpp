#include <iostream>

#define GLFW_INCLUDE_NONE

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <graphics/ShaderProgram.h>
#include <graphics/ComputeShader.h>
#include "graphics/Renderer.h"

#include "glm/gtc/matrix_transform.hpp"
#include "Timer.h"
#include "array"
int main() {
    Renderer *renderer = new Renderer();

    GLFWwindow *window = renderer->init();

    Texture *texture = new Texture();

    texture->setData(nullptr, 512, 512, Texture::RGBA8);

    ComputeShader *compute= new ComputeShader("../res/compute.glsl");

    compute->setGroups(512,512,1,1,1,1);
    compute->bind();
    compute->setUniform("img_output",0);
    compute->unbind();
    ShaderProgram *shader = new ShaderProgram("../res/shader.vert", "../res/shader.frag");

    glm::mat4 projection = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -1.0f, 1.0f);
    shader->bind();
    shader->setUniform("projection_matrix", projection);
    shader->setUniform("texture_0", 0);
    shader->unbind();
    Timer time;
    Timer delta_time;
    float delta = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, true);
        }


        compute->bind();
        texture->bind();
        glBindImageTexture(0, texture->getId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        compute->execute();
        glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        texture->unbind();
        compute->unbind();

        renderer->render(texture, shader);

        glfwSwapBuffers(window);
        delta = delta_time.ms() / 1000;
        delta_time.reset();
    }
    delete shader;
    delete texture;
    renderer->terminate();
    delete renderer;

    return 0;
}
