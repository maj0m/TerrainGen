#pragma once
#include "geometry.h"

class Sphere : public Geometry {
public:
	int tesselation = 100;
	float scale = 1;

	Sphere() { create(tesselation, tesselation); }

	void eval(float u, float v, vec3& pos) {
		float U = u * 2.0f * M_PI;
		float V = v * M_PI;

		float X = scale * sinf(V) * cosf(U);
		float Y = scale * sinf(U) * sinf(V);
		float Z = scale * -cosf(V);

		pos = vec3(X, Y, Z);
	}
};