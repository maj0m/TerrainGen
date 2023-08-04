#pragma once
#include "framework.h"
#include <iostream>

class Camera {
private:
    vec3 wEye, wFront, wUp;
    float fov, asp, fp, bp;
    float speed = 40.0;
    float sensitivity = 0.25;
    float lerp_amount = 0.01;
    float lastX = windowWidth / 2;
    float lastY = windowHeight / 2;
    float yaw = 0.0;
    float pitch = 0.0;
    bool firstMouse = true;

public:
    Camera() {
        wEye = vec3(0.0, 0.0, 3.0);
        wFront = vec3(0.0, 0.0, -1.0);
        wUp = vec3(0.0, 1.0, 0.0);
        asp = (float)windowWidth / windowHeight;
        fov = 60.0;
        fp = 0.1;
        bp = 200.0;
    }

    void orbit(vec3 center, float radius, float speed, float dt) {
        wEye.x = center.x + cos(dt * speed) * radius;
        wEye.y = center.y;
        wEye.z = center.z + sin(dt * speed) * radius;
        wFront = center - wEye;
    }

    void rotate(int pX, int pY) {
        if (firstMouse) {
            lastX = static_cast<float>(pX);
            lastY = static_cast<float>(pY);
            firstMouse = false;
        }

        float xoffset = static_cast<float>(pX) - lastX;
        float yoffset = lastY - static_cast<float>(pY);
        lastX = static_cast<float>(pX);
        lastY = static_cast<float>(pY);

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // Clamp camera pitch
        if (pitch > 90.0) pitch = 90.0;
        if (pitch < -90.0) pitch = -90.0;

        vec3 direction;
        direction.x = cos(yaw * M_PI / 180) * cos(pitch * M_PI / 180);
        direction.y = sin(pitch * M_PI / 180);
        direction.z = sin(yaw * M_PI / 180) * cos(pitch * M_PI / 180);
        wFront = normalize(direction);
    }

    void move(int key) {
        vec3 front = normalize(wFront);
        vec3 right = normalize(cross(front, wUp));

        switch (key) {
        case GLFW_KEY_W:
            wEye = lerp(wEye, wEye + speed * wFront, lerp_amount);
            break;
        case GLFW_KEY_S:
            wEye = lerp(wEye, wEye - speed * wFront, lerp_amount);
            break;
        case GLFW_KEY_A:
            wEye = lerp(wEye, wEye - speed * normalize(cross(wFront, wUp)), lerp_amount);
            break;
        case GLFW_KEY_D:
            wEye = lerp(wEye, wEye + speed * normalize(cross(wFront, wUp)), lerp_amount);
            break;
        case GLFW_KEY_Q:
            wEye = lerp(wEye, wEye + speed * wUp, lerp_amount);
            break;
        case GLFW_KEY_E:
            wEye = lerp(wEye, wEye - speed * wUp, lerp_amount);
            break;
        default:
            break;
        }
    }

    vec3 getwEye() {
        return wEye;
    }

    void setwEye(vec3 pos) {
        wEye = pos;
    }

    void setFirstMouse() {
        firstMouse = true;
    }

    mat4 V() {
        vec3 w = normalize(-wFront);
        vec3 u = normalize(cross(wUp, w));
        vec3 v = cross(w, u);

        return TranslateMatrix(-wEye) * mat4(u.x, v.x, w.x, 0.0,
            u.y, v.y, w.y, 0.0,
            u.z, v.z, w.z, 0.0,
            0.0, 0.0, 0.0, 1.0);
    }

    mat4 P() {
        float sy = 1.0 / tan(fov / 2.0 * M_PI / 180.0);

        return mat4(sy / asp, 0.0, 0.0, 0.0,
            0.0, sy, 0.0, 0.0,
            0.0, 0.0, -(fp + bp) / (bp - fp), -1.0,
            0.0, 0.0, -2.0 * fp * bp / (bp - fp), 0.0);
    }
};
