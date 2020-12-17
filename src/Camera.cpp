//
// Created by User on 11-Dec.-2020.
//

#include "Camera.h"
#include "Configs.h"
#include "GLFW/glfw3.h"
#include "glm/gtx/transform.hpp"
void Camera::update(float delta) {
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

    view_transform = glm::translate(view_transform, translation * 10.f*delta);

}

const mat4 &Camera::getMatrix() {
    return view_transform;
}
