#define WIDTH 512
#define HEIGHT 512


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
#include "Octree.h"

GLFWwindow *window;
int fps = 0;
float fps_time = 0.0f;

static void printFPS(float delta) {
    fps++;
    fps_time += delta;
    if (fps_time >= 1.0f) {
        std::cout << "fps:" << std::to_string(fps) << std::endl;
        fps_time = 0.0f;
        fps = 0;
    }
}

#define _USE_MATH_DEFINES

#include <math.h>

mat4 view_matrix = mat4(1.0f);
float max_pitch = M_PI;
float current_pitch = 0;

static void updateCamera(float delta) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    glfwSetCursorPos(window, static_cast<float>(WIDTH) / 2.0f, static_cast<float>(HEIGHT) / 2.0f);


    float fov = 90;
    vec2 change;
    change.x = (((static_cast<float>(WIDTH) / 2.0f) - x) / static_cast<float>(WIDTH)) * glm::radians(fov);
    change.y = (((static_cast<float>(HEIGHT) / 2.0f) - y) / static_cast<float>(HEIGHT)) *
               (glm::radians(fov) * ((float) HEIGHT / (float) WIDTH));

    //go to pitch 0
    view_matrix = glm::rotate(view_matrix, -current_pitch, vec3(1, 0, 0));
    //rotate on y axis
    view_matrix = glm::rotate(view_matrix, change.x, vec3(0, 1, 0));
    //go back to cuurent pitch
    current_pitch += change.y;
    if (current_pitch > max_pitch) {
        current_pitch = max_pitch;
    } else if (current_pitch < -max_pitch) {
        current_pitch = -max_pitch;
    }
    view_matrix = glm::rotate(view_matrix, current_pitch, vec3(1, 0, 0));


    vec3 translation = vec3(0);
    if (glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS) {
        translation.z += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS) {
        translation.z = -1;
    }
    if (glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS) {
        translation.x += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_A)==GLFW_PRESS) {
        translation.x = -1;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE)==GLFW_PRESS) {
        translation.y += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)==GLFW_PRESS) {
        translation.y = -1;
    }

    view_matrix = glm::translate(view_matrix, translation * delta);
}

int main() {
    /*fps = 0;
    Renderer *renderer = new Renderer();

    window = renderer->init();

    Texture *texture = new Texture();

    texture->setData(nullptr, 512, 512, Texture::RGBA32F);


    ComputeShader *compute = new ComputeShader("../res/compute.glsl");

    compute->setGroups(512, 512, 1, 1, 1, 1);
    compute->bind();

    compute->setUniform("resolution", vec2(512, 512));
    compute->setUniform("img_output", 0);
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
        updateCamera(delta);
        //view_matrix = glm::rotate(view_matrix, (float) (delta * M_PI), vec3(1.0f, 1.0f, 0.0f));
        compute->bind();
        compute->setUniform("position",view_matrix[4]);
        compute->setUniform("view_matrix", glm::inverse(view_matrix));
        compute->execute();
        compute->unbind();

        renderer->render(texture, shader);

        glfwSwapBuffers(window);
        delta = delta_time.ns() / SECONDS_TO_NANOSECOND;
        printFPS(delta);
        delta_time.reset();
    }
    delete shader;
    delete texture;
    renderer->terminate();
    delete renderer;

    return 0;*/

    Octree octree(0);
    int depth=2;
    int range=(pow(2,2));
    for (int x = 0; x < range; ++x) {
        for (int y = 0; y < range; ++y) {
            for (int z = 0; z < range; ++z) {
                octree.insert(rand(),x,y,z,2);
            }
        }
    }
    int size=octree.getData().size();
}
