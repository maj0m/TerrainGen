#pragma once
#include "framework.h"
#include "material.h"
#include "light.h"
#include "terraintexture.h"

struct RenderState {
	TerrainTexture* terrainTexture;
	float waterLevel;
	float waveLength;
	float waveAmplitude;
	float waterAlpha;
	float time;
	float fogDensity;
	vec3 fogColor;
	vec3 wEye;
	mat4 MVP, M, V, P;
	Material* material;
	std::vector<Light> lights;
};
