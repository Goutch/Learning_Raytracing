#include "Configs.h"


#include <iostream>

#define GLFW_INCLUDE_NONE

#include "GLFW/glfw3.h"

#include <graphics/ShaderProgram.h>
#include <graphics/ComputeShader.h>
#include "graphics/Renderer.h"

#include "glm/gtc/matrix_transform.hpp"
#include "Clock.h"
#include "Octree.h"
#include "graphics/StorageBuffer.h"
#include "FastNoise.h"

#define _USE_MATH_DEFINES

#include <math.h>

GLFWwindow *window;
int fps = 0;
float fps_time = 0.0f;
Clock clock_time;
mat4 view_transform = mat4(1.0f);

std::vector<vec4> voxel_materials = {vec4(0.0f, 0.0f, 0.0f, 0.0f),
                                     vec4(1.0f, 0.0f, 0.0f, 1.0f),
                                     vec4(0.0f, 1.0f, 0.0f, 1.0f),
                                     vec4(0.0f, 0.0f, 1.0f, 1.0f)};

static void printFPS(float delta) {
    fps++;
    fps_time += delta;
    if (fps_time >= 1.0f) {
        std::cout << "fps:" << std::to_string(fps) << std::endl;
        fps_time = 0.0f;
        fps = 0;
    }

}

float MAX_PITCH = M_PI/2.;
float current_pitch = 0.0f;

static void updateCamera(float delta) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    glfwSetCursorPos(window, static_cast<float>(WIDTH) / 2.0f, static_cast<float>(HEIGHT) / 2.0f);
    float fov = 90;
    vec2 change;
    change.x = (((static_cast<float>(WIDTH) / 2.0f) - x) / static_cast<float>(WIDTH)) * glm::radians(fov);
    change.y = (((static_cast<float>(HEIGHT) / 2.0f) - y) / static_cast<float>(HEIGHT)) * (glm::radians(fov) * ((float) HEIGHT / (float) WIDTH));

    //go to pitch 0
    view_transform = glm::rotate(view_transform, current_pitch, vec3(1, 0, 0));
    //rotate on y axis
    view_transform = glm::rotate(view_transform, -change.x, vec3(0, 1, 0));
    //go back to cuurent pitch
    current_pitch += change.y;
    glm::clamp(current_pitch, -MAX_PITCH, MAX_PITCH);
    view_transform = glm::rotate(view_transform, -current_pitch, vec3(1, 0, 0));


    vec3 translation = vec3(0);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        translation.z -= 1;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        translation.z += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        translation.x += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        translation.x -= 1;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        translation.y += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        translation.y -= 1;
    }

    view_transform = glm::translate(view_transform,translation * delta);
}

void fillOctree(Octree &octree) {
    FastNoise noise;
    float scale = 10;
    int depth = 4;
    int range = octree.getDimentionAt(depth);
    //octree.set(1, 0, 0, 0, depth);

    for (int x = 0; x < range; ++x) {
        for (int z = 0; z < range; ++z) {
            float n_value = (noise.GetSimplex((float) x * scale, (float) z * scale)+1.0f)/2.;
            for (int y = 0; y < range; ++y) {

                //if(z==range-1)
                //{
                //    octree.set(1, x, y, z, depth);
                //} else
                //{
                //    octree.set(0, x, y, z, depth);
                //}
                //octree.set(x%(voxel_materials.size()), x, y, z, depth);
                // octree.set(1, x, y, z, depth);

                if (n_value*16. > y) {
                    octree.set(2, x, y, z, depth);
                }

                //if(distance(vec3(x,y,z),vec3(range/2.0f,range/2.0f,range/2.0f))<=(range/2.0f))
                //{
                //   octree.set((z+y+x)%(voxel_materials.size()-1)+1, x, y, z, depth);
                // }
            }
        }
    }
}

int main() {
    fps = 0;
    Renderer *renderer = new Renderer();

    window = renderer->init();

    Texture *color_buffer = new Texture();
    color_buffer->setData(nullptr, WIDTH, HEIGHT, Texture::RGBA32F);
    Texture *normal_buffer = new Texture();
    normal_buffer->setData(nullptr, WIDTH, HEIGHT, Texture::RGBA32F);
    Texture *noise_texture = new Texture();
    noise_texture->load("../res/noise.png");

    Octree octree(0);
    fillOctree(octree);

    StorageBuffer octree_ssbo = StorageBuffer<Octree::Node>(octree.getData(), 0);
    StorageBuffer materials_ssbo = StorageBuffer<vec4>(voxel_materials, 1);

    ComputeShader *compute = new ComputeShader("../res/compute.glsl");

    compute->bind();
    compute->setGroups(WIDTH, HEIGHT, 1, 1, 1, 1);
    compute->setUniform("voxels_per_units", 256.0f);
    compute->setUniform("resolution", vec2(WIDTH, HEIGHT));
    compute->setUniform("color_output", 0);
    compute->setUniform("normal_output", 1);
    compute->setUniform("noise_texture", 2);
    compute->unbind();
    ShaderProgram *shader = new ShaderProgram("../res/shader.vert", "../res/shader.frag");

    glm::mat4 projection = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -1.0f, 1.0f);

    shader->bind();
    shader->setUniform("projection_matrix", projection);
    shader->setUniform("color_texture", 0);
    shader->setUniform("normal_texture", 1);
    shader->setUniform("resolution", vec2(float(WIDTH),float(HEIGHT)));
    shader->unbind();

    Clock delta_time;
    float delta = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, true);
        }
        updateCamera(delta);
        octree_ssbo.bind();
        materials_ssbo.bind();
        color_buffer->bind(0);
        normal_buffer->bind(1);
        noise_texture->bind(2);
        compute->bind();
        compute->setUniform("view_transform",view_transform );

        compute->setUniform("time", clock_time.ms() / SECOND_TO_MILISECOND);
        compute->execute();
        compute->unbind();
        noise_texture->unbind(2);

        octree_ssbo.unbind();
        materials_ssbo.unbind();

        renderer->present(color_buffer, shader);
        normal_buffer->unbind(1);
        color_buffer->unbind(0);
        glfwSwapBuffers(window);
        delta = delta_time.ns() / SECONDS_TO_NANOSECOND;

        printFPS(delta);
        delta_time.reset();
    }
    delete shader;
    delete color_buffer;
    delete noise_texture;
    delete normal_buffer;
    renderer->terminate();
    delete renderer;

    return 0;


}
