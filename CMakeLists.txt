cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(SVO_Raytracer)

include_directories("dependencies/glm")
add_subdirectory("dependencies/glm")

include_directories("dependencies/glfw/include")
add_subdirectory("dependencies/glfw")

include_directories("dependencies/glad/include")
add_subdirectory("dependencies/glad")

include_directories("dependencies/FastNoise")
add_subdirectory("dependencies/FastNoise")
include_directories("src")

include_directories("dependencies/stb")
add_subdirectory(dependencies/stb/)
include_directories(dependencies/stb/)
set(GL_FILES
         )

add_executable (${PROJECT_NAME}
        src/main.cpp
        src/graphics/Renderer.cpp
        src/graphics/Texture.cpp
        src/graphics/ShaderProgram.cpp
        src/graphics/Mesh.cpp
        src/graphics/Framebuffer.cpp
        src/graphics/GL_Debug.cpp
        src/Clock.cpp
        src/graphics/ComputeShader.cpp
        src/PureOctree.cpp
        src/utils/FastNoise.cpp
        src/Camera.cpp
        src/BlockOctree.cpp
        src/LODOctree.cpp src/PureOctreeRenderer.h src/BlockOctreeRenderer.h src/graphics/Image.cpp src/graphics/Image.h)

target_link_libraries(${PROJECT_NAME} glm glfw glad FastNoise stb)