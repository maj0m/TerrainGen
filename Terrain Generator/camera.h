#pragma once
#include "framework.h"
#include <iostream>

class Camera {
private:
    vec3 wEye, wFront, wUp;
    float fov, asp, fp, bp;
    float speed = 1.0;
    float sensitivity = 0.25;
    float lastX = windowWidth / 2;
    float lastY = windowHeight / 2;
    float yaw = 0.0;
    float pitch = 0.0;
    bool firstMouse = false;

public:
    Camera() {
        wEye = vec3(0.0, 0.0, 3.0);
        wFront = vec3(0.0, 0.0, -1.0);
        wUp = vec3(0.0, 1.0, 0.0);
        asp = (float)windowWidth / windowHeight;
        fov = 60.0;
        fp = 0.1;
        bp = 500.0;
    }

    void orbit(vec3 center, float radius, float speed, float dt) {
        wEye.x = center.x + cos(dt * speed) * radius;
        wEye.y = center.y;
        wEye.z = center.z + sin(dt * speed) * radius;
        wFront = center - wEye;
    }

    void rotate(int pX, int pY) {
        if (firstMouse) {
            lastX = pX;
            lastY = pY;
            firstMouse = false;
        }

        float xoffset = pX - lastX;
        float yoffset = lastY - pY;
        lastX = pX;
        lastY = pY;

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // Clamp camera pitch
        clamp(pitch, -89.0, 89.0);

        vec3 direction;
        direction.x = cos(radians(yaw)) * cos(radians(pitch));
        direction.y = sin(radians(pitch));
        direction.z = sin(radians(yaw)) * cos(radians(pitch));
        wFront = normalize(direction);
    }

    void move(int key) {
        vec3 front = normalize(wFront);
        vec3 right = normalize(cross(front, wUp));

        switch (key) {
        case GLFW_KEY_W:    wEye += speed * wFront;
            break;
        case GLFW_KEY_S:    wEye -= speed * wFront;
            break;
        case GLFW_KEY_A:    wEye -= speed * normalize(cross(wFront, wUp));
            break;
        case GLFW_KEY_D:    wEye += speed * normalize(cross(wFront, wUp));
            break;
        case GLFW_KEY_Q:    wEye += speed * wUp;
            break;
        case GLFW_KEY_E:    wEye -= speed * wUp;
            break;
        default:
            break;
        }
    }

    vec3 getEyePos() {
        return wEye;
    }

    void setEyePos(vec3 pos) {
        wEye = pos;
    }

    vec3 getEyeDir() {
        return wFront;
    }

    void setEyeDir(vec3 dir) {
        wFront = dir;
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
        float sy = 1.0 / tan(radians(fov / 2.0));

        return mat4(sy / asp, 0.0, 0.0, 0.0,
            0.0, sy, 0.0, 0.0,
            0.0, 0.0, -(fp + bp) / (bp - fp), -1.0,
            0.0, 0.0, -2.0 * fp * bp / (bp - fp), 0.0);
    }
};
