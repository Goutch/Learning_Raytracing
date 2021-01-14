#pragma once

#include "Configs.h"


#include <iostream>
#include "Camera.h"

#define GLFW_INCLUDE_NONE

#include "GLFW/glfw3.h"

#include <graphics/ShaderProgram.h>
#include <graphics/ComputeShader.h>
#include "graphics/Renderer.h"
#include "graphics/StorageBuffer.h"
#include "graphics/Image.h"
#include "Clock.h"
#include "PureOctree.h"
#include "BlockOctree.h"
#include "FastNoise.h"
#include "glm/gtx/transform.hpp"
#include "NoiseGen.h"

namespace PureOctreeRenderer {
    GLFWwindow *window;
    int fps = 0;
    float fps_time = 0.0f;
    Clock app_time;

    std::vector<vec4> voxel_materials = {vec4(0.0f, 0.0f, 0.0f, 0.0f),
                                         vec4(0.6f, 0.3f, 0.05f, 1.0f),
                                         vec4(0.0f, 1.0f, 0.0f, 1.0f),
                                         vec4(0.0f, 0.0f, 1.0f, 1.0f)};
    const float voxels_per_units = 16.0f;

    static void printFPS(float delta) {
        fps++;
        fps_time += delta;
        if (fps_time >= 1.0f) {
            std::cout << "fps:" << std::to_string(fps) << std::endl;
            fps_time = 0.0f;
            fps = 0;
        }
    }

    void fillOctree(PureOctree &octree) {

        NoiseGen noise = NoiseGen(1);
        noise.addLayer(NoiseType::simplex, 0.3, 0.5);
        noise.addLayer(NoiseType::fractal, 0.7, 0.5);

        int range = octree.getDimention();

        for (int x = 0; x < range; ++x) {
            for (int z = 0; z < range; ++z) {
                float noise_value = noise.get(x, z);
                bool bellow_noise = false;
                for (int y = range - 1; y >= 0; --y) {
                    if (bellow_noise) {
                        octree.set(1, x, y, z);
                    } else if (noise_value * 16. > y) {
                        octree.set(2, x, y, z);
                        bellow_noise = true;
                    }
                }
            }
        }
    }

    void start() {
        fps = 0;
        Renderer *renderer = new Renderer();

        window = renderer->init();

        Camera camera(window);

        Image *color_buffer = new Image();
        color_buffer->setData(nullptr, WIDTH, HEIGHT, Texture::RGBA32F);
        Image *normal_buffer = new Image();
        normal_buffer->setData(nullptr, WIDTH, HEIGHT, Texture::RGBA32F);
        Texture *noise_texture = new Texture();
        noise_texture->load("../res/noise.png");

        PureOctree octree(0, 5);
        fillOctree(octree);

        StorageBuffer octree_ssbo = StorageBuffer<PureOctree::Node>(octree.getData(), 0);
        StorageBuffer materials_ssbo = StorageBuffer<vec4>(voxel_materials, 1);

        ComputeShader *compute = new ComputeShader("../res/PureOctreeTracer.glsl");

        compute->bind();
        compute->setGroups(WIDTH, HEIGHT, 1, 1, 1, 1);
        compute->setUniform("voxels_per_units", voxels_per_units);
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
        shader->setUniform("resolution", vec2(float(WIDTH), float(HEIGHT)));
        shader->unbind();

        Clock delta_time;
        float delta = 0.0f;
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
                glfwSetWindowShouldClose(window, true);
            }
            camera.update(delta);
            octree_ssbo.bind();
            materials_ssbo.bind();
            color_buffer->bind(0);
            normal_buffer->bind(1);
            noise_texture->bind(2);
            compute->bind();
            compute->setUniform("view_transform", camera.getMatrix());
            compute->setUniform("time", app_time.ms() / SECOND_TO_MILISECOND);
            compute->dispatch();
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

    };
};