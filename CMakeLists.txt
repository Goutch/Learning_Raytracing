cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(VoxTracer)

include_directories("dependencies/glm")
add_subdirectory("dependencies/glm")

include_directories("dependencies/glfw/include")
add_subdirectory("dependencies/glfw")

include_directories("dependencies/glad/include")
add_subdirectory("dependencies/glad")

include_directories("dependencies/FastNoise")
add_subdirectory("dependencies/FastNoise")
include_directories("src")



set(GL_FILES
         )

add_executable (${PROJECT_NAME}
        src/main.cpp
        src/graphics/Renderer.cpp
        src/graphics/Texture.cpp
        src/graphics/ShaderProgram.cpp
        src/graphics/Mesh.cpp
        src/graphics/Framebuffer.cpp
        src/graphics/Framebuffer.h
        src/graphics/GL_Debug.cpp
        src/graphics/GL_Debug.h
        src/Configs.h
        src/Clock.cpp src/graphics/ComputeShader.cpp src/graphics/ComputeShader.h src/Octree.cpp src/Octree.h src/graphics/StorageBuffer.cpp src/graphics/StorageBuffer.h src/FastNoise.cpp src/FastNoise.h)

target_link_libraries(${PROJECT_NAME} glm glfw glad FastNoise)