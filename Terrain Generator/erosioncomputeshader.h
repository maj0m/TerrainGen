#pragma once
#include "framework.h"
#include "computeshader.h"

int terrainTextureWidth = 256;
int terrainTextureHeight = 256;
float terrainAmplitude = 12.0;
float terrainFrequency = 1.2;
int terrainOctaves = 8;
int terrainSeed = 500;

int erosionIterations = 5;
float erosionMinVolume = 0.2;
float erosionDensity = 1.2;
float erosionDepositionRate = 0.5;
float erosionEvaporationRate = 0.01;
float erosionFriction = 0.05;
bool terrainErosion = false;

class ErosionComputeShader : ComputeShader {
	const char* computeShaderSource = R"(
		#version 450 core

        layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
        layout(rgba32f, binding = 0) uniform image2D heightMap;

        // Parameters
        uniform int numIterations;
        uniform float minParticleVolume;
        uniform float particleDensity;
        uniform float frictionFactor;
        uniform float depositionRate;
        uniform float evaporationRate;

        // Compute surface normal
        vec3 computeSurfaceNormal(int i, int j) {
            float scale = 12.0;
            vec3 n = vec3(0.15) * normalize(vec3(scale * (imageLoad(heightMap, ivec2(i, j)).r - imageLoad(heightMap, ivec2(i + 1, j)).r), 1.0, 0.0)); // Positive X
            n += vec3(0.15) * normalize(vec3(scale * (imageLoad(heightMap, ivec2(i - 1, j)).r - imageLoad(heightMap, ivec2(i, j)).r), 1.0, 0.0));   // Negative X
            n += vec3(0.15) * normalize(vec3(0.0, 1.0, scale * (imageLoad(heightMap, ivec2(i, j)).r - imageLoad(heightMap, ivec2(i, j + 1)).r)));    // Positive Y
            n += vec3(0.15) * normalize(vec3(0.0, 1.0, scale * (imageLoad(heightMap, ivec2(i, j - 1)).r - imageLoad(heightMap, ivec2(i, j)).r)));    // Negative Y
            n += vec3(0.1) * normalize(vec3(scale * (imageLoad(heightMap, ivec2(i, j)).r - imageLoad(heightMap, ivec2(i + 1, j + 1)).r) / sqrt(2.0), sqrt(2.0), scale * (imageLoad(heightMap, ivec2(i, j)).r - imageLoad(heightMap, ivec2(i + 1, j + 1)).r) / sqrt(2.0))); // Positive diagonal
            n += vec3(0.1) * normalize(vec3(scale * (imageLoad(heightMap, ivec2(i, j)).r - imageLoad(heightMap, ivec2(i + 1, j - 1)).r) / sqrt(2.0), sqrt(2.0), scale * (imageLoad(heightMap, ivec2(i, j)).r - imageLoad(heightMap, ivec2(i + 1, j - 1)).r) / sqrt(2.0))); // Negative diagonal
            n += vec3(0.1) * normalize(vec3(scale * (imageLoad(heightMap, ivec2(i, j)).r - imageLoad(heightMap, ivec2(i - 1, j + 1)).r) / sqrt(2.0), sqrt(2.0), scale * (imageLoad(heightMap, ivec2(i, j)).r - imageLoad(heightMap, ivec2(i - 1, j + 1)).r) / sqrt(2.0))); // Positive diagonal
            n += vec3(0.1) * normalize(vec3(scale * (imageLoad(heightMap, ivec2(i, j)).r - imageLoad(heightMap, ivec2(i - 1, j - 1)).r) / sqrt(2.0), sqrt(2.0), scale * (imageLoad(heightMap, ivec2(i, j)).r - imageLoad(heightMap, ivec2(i - 1, j - 1)).r) / sqrt(2.0))); // Negative diagonal
            return n;
        }

        void main() {
            ivec2 dimensions = imageSize(heightMap);
            ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

            for (int iteration = 0; iteration < numIterations; iteration++) {
                vec2 dropletPosition = vec2(pixelCoords);
                float dropletVolume = 1.0;
                vec2 dropletSpeed = vec2(0.0);
                float dropletSediment = 0.0;

                while (dropletVolume > minParticleVolume) {
                    ivec2 intPosition = ivec2(dropletPosition);
                    vec3 normal = computeSurfaceNormal(intPosition.x, intPosition.y);

                    dropletSpeed += vec2(normal.x, normal.z) / (dropletVolume * particleDensity);
                    dropletPosition += dropletSpeed;
                    dropletSpeed *= (1.0 - frictionFactor);

                    if (dropletPosition.x < 0 || dropletPosition.x > dimensions.x || dropletPosition.y < 0 || dropletPosition.y > dimensions.y) {
                        break;
                    }

                    float maxSediment = dropletVolume * length(dropletSpeed) * (imageLoad(heightMap, intPosition).r - imageLoad(heightMap, ivec2(dropletPosition))).r;
                    maxSediment = max(0.0, maxSediment);
                    float sedimentDiff = maxSediment - dropletSediment;

                    dropletSediment += depositionRate * sedimentDiff;

                    float finalHeight = imageLoad(heightMap, intPosition).r - dropletVolume * depositionRate * sedimentDiff;
                    imageStore(heightMap, intPosition, vec4(finalHeight));

                    dropletVolume *= (1.0 - evaporationRate);
                }
            }
        }
		)";

public:
	ErosionComputeShader() { create(computeShaderSource); }

	void Bind() {
		glUseProgram(getId());	// make this program run

		setUniform("numIterations", erosionIterations);
		setUniform("minParticleVolume", erosionMinVolume);
		setUniform("particleDensity", erosionDensity);
		setUniform("frictionFactor", erosionFriction);
		setUniform("depositionRate", erosionDepositionRate);
		setUniform("evaporationRate", erosionEvaporationRate);

		glDispatchCompute(ceil(terrainTextureWidth / 8), ceil(terrainTextureHeight / 4), 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}
};
