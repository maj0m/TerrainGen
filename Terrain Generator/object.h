#pragma once
#include "framework.h"
#include "shader.h"
#include "material.h"
#include "texture.h"
#include "geometry.h"
#include "renderstate.h"
#include "terraintexture.h"

struct Object {
	vec3 pos;
	vec3 scale;
	vec3 rotAxis;
	float rotAngle;
	Shader* shader;
	Material* material;
	Geometry* geometry;

public:
	Object(Shader* _shader, Material* _material, Geometry* _geometry) {
		pos = vec3(0, 0, 0);
		scale = vec3(1, 1, 1);
		rotAxis = vec3(0, 1, 0);
		rotAngle = 0;
		shader = _shader;
		material = _material;
		geometry = _geometry;
	}

	void Draw(RenderState state) {
		mat4 M = ScaleMatrix(scale) * RotationMatrix(rotAngle, rotAxis) * TranslateMatrix(pos);
		state.M = M;
		state.MVP = state.M * state.V * state.P;
		state.material = material;
		shader->Bind(state);
		geometry->Draw();
	}
};
