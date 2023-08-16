#pragma once
#include "FastNoiseLite.h"
#include "erosioncomputeshader.h"
#include "renderstate.h"

class TerrainTexture {
	std::vector<vec4> image;
	int width, height;
	float frequency;
	int octaves;
	int seed;
	FastNoiseLite noise;

	float getHeightNormalized(float U, float V) {
		float height = 0;
		float layerAmplitude = 1;
		float layerFrequency = frequency;
		float maxHeight = 0;

		for (int i = 0; i < octaves; i++) {
			maxHeight += layerAmplitude;
			noise.SetFrequency(layerFrequency);
			height += noise.GetNoise(U, V) * layerAmplitude;
			layerAmplitude *= 0.5;
			layerFrequency *= 2.0;
		}

		// Normalize height
		float normalizedHeight = (height + maxHeight) / (2 * maxHeight);
		if (normalizedHeight < 0.0) normalizedHeight = 0.0;
		if (normalizedHeight > 1.0) normalizedHeight = 1.0;
		return normalizedHeight;
	}

public:
	unsigned int textureId = 0;

	TerrainTexture() {
		width = terrainTextureWidth;
		height = terrainTextureHeight;
		frequency = terrainFrequency;
		octaves = terrainOctaves;
		seed = terrainSeed;
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		noise.SetSeed(seed);

		image.resize(width * height);
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				// Normalize
				float U = (float) x / (width - 1);
				float V = (float) y / (height - 1);
				float height = getHeightNormalized(U, V);
				image[y * width + x] = vec4(height, height, height, 1);
			}
		}
		
		// Create and bind texture
		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureStorage2D(textureId, 1, GL_RGBA32F, width, height);
		glTextureSubImage2D(textureId, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, image.data());
		glBindImageTexture(0, textureId, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		if(terrainErosion) erode();
		
	}

	void erode() {
		ErosionComputeShader* computeShader = new ErosionComputeShader();
		computeShader->Bind();
	}
};



// Square Bump to force edges down
//float dist = 1.0 - (1.0 - U * U) * (1.0 - V * V);
//normalizedHeight -= dist * terrainFallOffRate;

