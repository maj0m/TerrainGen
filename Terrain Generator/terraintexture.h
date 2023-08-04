#pragma once
#include "geometry.h"
#include "FastNoiseLite.h"
#include "texture.h"

int terrainTextureWidth = 128;
int terrainTextureHeight = 128;
float terrainFrequency = 1.0;
int terrainOctaves = 8;
int terrainSeed = 500;
bool terrainErosion = true;

int erosionStepSize = 2;
float erosionDensity = 1.0;
float erosionEvaporationRate = 0.001;
float erosionDepositionRate = 0.1;
float erosionFriction = 0.16;
bool erosionRandomDistribution = false;


struct Particle {
	vec2 pos;
	vec2 speed = vec2(0.0);
	float volume = 1.0;
	float sediment = 0.0; //Fraction of Volume that is Sediment!

	Particle(vec2 _pos) {
		pos = _pos;
	}
};

class TerrainTexture : public Texture {
	std::vector<float> image;
	float frequency;
	int octaves;
	int seed;
	FastNoiseLite noise;
	
	vec3 surfaceNormal(float x, float y) {
		float epsilon = 0.01;
		float hL = getHeightNormalized(x - epsilon, y);
		float hR = getHeightNormalized(x + epsilon, y);
		float hD = getHeightNormalized(x, y - epsilon);
		float hU = getHeightNormalized(x, y + epsilon);

		vec3 surfaceNormal = vec3(hL - hR, 2.0 * epsilon, hD - hU);
		return normalize(surfaceNormal);
	}

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

		float normalizedHeight = (height + maxHeight) / (2 * maxHeight);
		if (normalizedHeight < 0.0) normalizedHeight = 0.0;
		if (normalizedHeight > 1.0) normalizedHeight = 1.0;
		return normalizedHeight;
	}

public:
	TerrainTexture(const int width = terrainTextureWidth, const int height = terrainTextureHeight,
		float _frequency = terrainFrequency, int _octaves = terrainOctaves, int _seed = terrainSeed, bool erosion = terrainErosion) : Texture() {
		frequency = _frequency;
		octaves = _octaves;
		seed = _seed;
		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		noise.SetSeed(seed);

		image.resize(width * height);
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				// Normalize
				float U = static_cast<float>(x) / (width - 1);
				float V = static_cast<float>(y) / (height - 1);
				// Between -1 and 1
				U = U * 2.0 - 1.0;
				V = V * 2.0 - 1.0;

				float height = getHeightNormalized(U, V);
				image[y * width + x] = height;
			}
		}

		if (erosion) erode(width, height);
		create(width, height, image, GL_LINEAR);
	}

	void erode(const int width, const int height) {
		for (int i = 0; i < width; i+= erosionStepSize) {
			for (int j = 0; j < height; j+= erosionStepSize) {
				//Spawn New Particle
				vec2 newPos = erosionRandomDistribution ? vec2(rand() % width, rand() % height) : vec2(i, j);
				Particle drop(newPos);

				//As long as the droplet exists
				while (drop.volume > 0) {
					vec2 startPos = drop.pos;								//Floored Droplet Initial Position
					vec3 normal = surfaceNormal(startPos.x, startPos.y);	//Surface Normal at Position
					if (normal.y > 0.99) break;

					drop.speed = drop.speed + vec2(normal.x, normal.z) / (drop.volume * erosionDensity);
					drop.pos = drop.pos + drop.speed;
					drop.speed = drop.speed * (1.0 - erosionFriction);       //Friction

					//Check if Particle is still in-bounds
					if (drop.pos.x < 0 || drop.pos.x > width)	break;
					if (drop.pos.y < 0 || drop.pos.y > height)	break;

					//Compute sediment capacity difference
					float maxSediment = drop.volume * length(drop.speed) * (image[(int)startPos.y * width + (int)startPos.x] - image[(int)drop.pos.y * width + (int)drop.pos.x]);
					if (maxSediment < 0.0) maxSediment = 0.0;
					float diffSediment = maxSediment - drop.sediment;

					//Act on the heightmap and drop
					drop.sediment += erosionDepositionRate * diffSediment;
					image[(int)startPos.y * width + (int)startPos.x] -= drop.volume * erosionDepositionRate * diffSediment;

					//Evaporate the Droplet (Note: Proportional to Volume! Better: Use shape factor to make proportional to the area instead.)
					drop.volume *= (1.0 - erosionEvaporationRate);
				}
			}
		}
	}
};



// Square Bump to force edges down
	//float nx = U / 50.0;
	//float nz = V / 50.0;
	//float dist = 1.0 - (1.0 - nx * nx) * (1.0 - nz * nz);
	//height -= dist * 5.0;
