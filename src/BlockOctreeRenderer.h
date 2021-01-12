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

#include "Clock.h"
#include "BlockOctree.h"
#include "NoiseGen.h"
#include "glm/gtx/transform.hpp"

namespace BlockOctreeRenderer {
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

    void fillOctree(BlockOctree &octree) {

        NoiseGen noise = NoiseGen(1);
        noise.addLayer(NoiseType::simplex, 0.3, 0.5);
        noise.addLayer(NoiseType::fractal, 0.7, 0.5);

        int block_count = octree.getSize();
        int block_size = octree.getBlockSize();
        for (int x = 0; x < block_count; ++x) {
            for (int z = 0; z < block_count; ++z) {
                for (int y = block_count - 1; y >= 0; --y) {
                    BlockOctree::Block block;
                    bool empty=true;
                    for (int block_x = 0; block_x < block_size; ++block_x) {
                        for (int block_z = 0; block_z < block_size; ++block_z) {
                            float noise_value = noise.get((x * block_size) + block_x, (z * block_size) + block_z);
                            bool bellow_noise = false;
                            for (int block_y = 0; block_y < block_size; ++block_y) {
                                ui8 val = 0;
                                if (bellow_noise) {
                                    val = 1;
                                } else if (noise_value * 16. > y) {
                                    val = 2;
                                    bellow_noise = true;
                                }

                                if(val!=0)
                                    empty=false;
                                block.materials[block_x][block_z][block_y] = val;
                            }
                        }
                    }
                    if(!empty)
                        octree.setBlock(block,x,y,z);
                }
            }
        }
    }

    void start() {
        fps = 0;
        Renderer *renderer = new Renderer();

        window = renderer->init();

        Camera camera(window);

        Texture *color_buffer = new Texture();
        color_buffer->setData(nullptr, WIDTH, HEIGHT, Texture::RGBA32F);
        Texture *normal_buffer = new Texture();
        //normal_buffer->setData(nullptr, WIDTH, HEIGHT, Texture::RGBA32F);
        Texture *noise_texture = new Texture();
        noise_texture->load("../res/noise.png");

        BlockOctree octree(5);
        fillOctree(octree);

        StorageBuffer octree_ssbo = StorageBuffer<BlockOctree::Node>(octree.getNodes(), 0);
        StorageBuffer materials_ssbo = StorageBuffer<vec4>(voxel_materials, 1);

        ComputeShader *compute = new ComputeShader("../res/BlockOctreeTracer.glsl");

        compute->bind();
        compute->setGroups(WIDTH, HEIGHT, 1, 1, 1, 1);
        compute->setUniform("voxels_per_units", voxels_per_units);
        compute->setUniform("resolution", vec2(WIDTH, HEIGHT));
        compute->setUniform("chunk_size",static_cast<int>(octree.getSize()));
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