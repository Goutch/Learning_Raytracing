//
// Created by User on 11-Dec.-2020.
//

#pragma once
#include "glm/glm.hpp"
using namespace glm;

#define _USE_MATH_DEFINES
struct GLFWwindow;
#include <math.h>
class Camera {
    mat4 view_transform = mat4(1.0f);
    float MAX_PITCH = M_PI/2.;
    float current_pitch = 0.0f;
    GLFWwindow* window;
public:
    Camera(GLFWwindow* window):window(window){};
    void update(float delta) ;
    const mat4& getMatrix();
};


