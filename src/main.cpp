#define WIDTH 512
#define HEIGHT 512


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
mat4 view_rotation = mat4(1.0f);
vec4 view_position = vec4(0.0f, 0.0f, -3.0f, 1.0f);
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
        std::cout << view_position.z << std::endl;
    }
}


float max_pitch = M_PI;
float current_pitch = 0;

static void updateCamera(float delta) {
    /*double x, y;
    glfwGetCursorPos(window, &x, &y);
    glfwSetCursorPos(window, static_cast<float>(WIDTH) / 2.0f, static_cast<float>(HEIGHT) / 2.0f);
    float fov = 90;
    vec2 change;
    change.x = (((static_cast<float>(WIDTH) / 2.0f) - x) / static_cast<float>(WIDTH)) * glm::radians(fov);
    change.y = (((static_cast<float>(HEIGHT) / 2.0f) - y) / static_cast<float>(HEIGHT)) * (glm::radians(fov) * ((float) HEIGHT / (float) WIDTH));

    //go to pitch 0
    view_rotation = glm::rotate(view_rotation, -current_pitch, vec3(1, 0, 0));
    //rotate on y axis
    view_rotation = glm::rotate(view_rotation, change.x, vec3(0, 1, 0));
    //go back to cuurent pitch
    current_pitch += change.y;
    glm::clamp(current_pitch, -max_pitch, max_pitch);
    view_rotation = glm::rotate(view_rotation, current_pitch, vec3(1, 0, 0));*/


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

    view_position += vec4(translation * delta, 1.0f);
}

void fillOctree(Octree &octree) {
    FastNoise noise;
    float scale = 0.33;
    int depth =4;
    int range = octree.getDimentionAt(depth);
    for (int x = 0; x <= range; ++x) {
        for (int y = 0; y <= range; ++y) {
            for (int z = 0; z <= range; ++z) {
                //octree.set(x%(voxel_materials.size()), x, y, z, depth);
                //octree.set(1, x, y, z, depth);
                // float n_value = noise.GetNoise((float) x * scale, (float) y * scale, (float) z * scale);
                // if (n_value > 0.0f) {
                //     octree.set(1, x, y, z, depth);
                // }
                if(distance(vec3(x,y,z),vec3(range/2.0f,range/2.0f,range/2.0f))<=(range/2.0f))
                {
                    octree.set((z+y+x)%(voxel_materials.size()-1)+1, x, y, z, depth);
                }
            }
        }
    }
}

int main() {
    fps = 0;
    Renderer *renderer = new Renderer();

    window = renderer->init();

    Texture *texture = new Texture();

    texture->setData(nullptr, WIDTH, HEIGHT, Texture::RGBA32F);

    Octree octree(0);
    fillOctree(octree);
    StorageBuffer materials_ssbo = StorageBuffer<vec4>(voxel_materials, 2);
    StorageBuffer octree_ssbo = StorageBuffer<Octree::Node>(octree.getData(), 1);
    ComputeShader *compute = new ComputeShader("../res/compute.glsl");
    compute->bind();
    compute->setGroups(512, 512, 1, 1, 1, 1);
    compute->setUniform("voxels_per_units", 256.0f);
    compute->setUniform("resolution", vec2(512, 512));
    compute->setUniform("img_output", 0);
    compute->unbind();
    ShaderProgram *shader = new ShaderProgram("../res/shader.vert", "../res/shader.frag");

    glm::mat4 projection = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -1.0f, 1.0f);
    shader->bind();
    shader->setUniform("projection_matrix", projection);
    shader->setUniform("texture_0", 0);

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
        compute->bind();
        compute->setUniform("view_position", view_position);
        compute->setUniform("view_rotation", glm::inverse(view_rotation));

        compute->execute();
        compute->unbind();
        octree_ssbo.unbind();
        materials_ssbo.unbind();
        renderer->present(texture, shader);

        glfwSwapBuffers(window);
        delta = delta_time.ns() / SECONDS_TO_NANOSECOND;
        printFPS(delta);
        delta_time.reset();
    }
    delete shader;
    delete texture;
    renderer->terminate();
    delete renderer;

    return 0;


}
