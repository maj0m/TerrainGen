#pragma once
#include "geometry.h"

class Plane : public Geometry {
    float scale;
    int tesselation;
public:


    Plane(int _tesselation, float _scale) {
        scale = _scale;
        tesselation = _tesselation;
        
        create(tesselation, tesselation);
    }

    void eval(float u, float v, vec3& pos) {
        float U = (u - 0.5f) * scale;
        float V = (v - 0.5f) * scale;

        pos = vec3(U, 0, V);
    }
};