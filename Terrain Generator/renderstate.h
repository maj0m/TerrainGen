#pragma once
#include "framework.h"
#include "material.h"
#include "light.h"
#include "terraintexture.h"

struct RenderState {
	TerrainTexture* terrainTexture;
	float terrainAmplitude = 8.0;
	float waveLength = 10.0;
	float waveAmplitude = 0.02;
	float waterAlpha = 0.4;
	float time;
	float fogDensity;
	vec3 fogColor;
	vec3 wEye;
	mat4 MVP, M, Minv, V, P;
	Material* material;
	std::vector<Light> lights;
};
