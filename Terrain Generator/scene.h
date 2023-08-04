#pragma once
#include "framework.h"
#include "camera.h"
#include "material.h"
#include "renderstate.h"
#include "light.h"
#include "material.h"
#include "object.h"
#include "texture.h"
#include "sphere.h"
#include "terraintexture.h"
#include "terrainshader.h"
#include "plane.h"
#include "watershader.h"
#include <iostream>

const int tesselation = 64;
const int scale = 100;

class Scene {
	Camera camera;
	RenderState state;
	std::vector<Object*> objects;
	std::vector<Light> lights;
	int fps;
	int frameCount = 0;
	float previousTime = glfwGetTime();

public:
	void drawGUI(int x, int y, int w, int h) {
		ImGui::SetNextWindowPos(ImVec2(x, y));
		ImGui::SetNextWindowSize(ImVec2(w, h));
		ImGui::Begin("Settings");

		// Display FPS
		getFPS(fps);
		ImGui::Text("FPS: %d", fps);
		ImGui::SliderInt("texture size", &terrainTextureWidth, 0, 500);
		ImGui::SliderInt("texture size", &terrainTextureHeight, 0, 500);

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		// Terrain sliders
		ImGui::SliderFloat("noise frequency", &terrainFrequency, 0.0, 10.0);
		ImGui::SliderFloat("noise amplitude", &state.terrainAmplitude, 0.0, 10.0);
		ImGui::SliderInt("noise octaves", &terrainOctaves, 0, 12);
		ImGui::SliderInt("noise seed", &terrainSeed, 0, 1000);

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		// Water Sliders
		ImGui::SliderFloat("wavelength", &state.waveLength, 0.0, 25.0);
		ImGui::SliderFloat("wave amplitude", &state.waveAmplitude, 0.0, 1.0);
		ImGui::SliderFloat("water alpha", &state.waterAlpha, 0.0, 1.0);

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		// Erosion sliders
		ImGui::Checkbox("erosion", &terrainErosion);
		ImGui::SliderInt("step size", &erosionStepSize, 1, 20);
		ImGui::SliderFloat("density", &erosionDensity, 0.0, 2.0);
		ImGui::SliderFloat("evaporation rate", &erosionEvaporationRate, 0.0, 0.01, "%.4f");
		ImGui::SliderFloat("deposition rate", &erosionDepositionRate, 0.0, 0.5);
		ImGui::SliderFloat("friction", &erosionFriction, 0.0, 0.5);
		ImGui::Checkbox("random distribution", &erosionRandomDistribution);

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		// Generate new Terrain
		if (ImGui::Button("Update terrain")) {
			state.terrainTexture = new TerrainTexture();
		}

		ImGui::End();
	}

	void Render() {
		glViewport(0, 0, windowWidth, windowHeight);
		updateState(state);
		for (Object* obj : objects) { obj->Draw(state); }
		drawGUI(0, 0, 400, 600);
	}

	void Build() {
		// Textures
		state.terrainTexture = new TerrainTexture();

		// Shaders
		Shader* terrainShader	= new TerrainShader();
		Shader* waterShader		= new WaterShader();

		// Materials
		Material* terrainMaterial	= new Material(vec3(1.0f, 1.0f, 1.0f), vec3(0.2f, 0.2f, 0.2f), vec3(0.4f, 0.4f, 0.4f), 0.2f);
		Material* waterMaterial		= new Material(vec3(1.0f, 1.0f, 1.0f), vec3(0.2f, 0.2f, 0.2f), vec3(0.4f, 0.4f, 0.4f), 10.2f);

		// Geometries
		Geometry* planeGeometry	= new Plane(tesselation, scale);

		// Objects
		Object* terrainObject = new Object(terrainShader, terrainMaterial, planeGeometry);
		terrainObject->pos = vec3(0, 0, 0);
		objects.push_back(terrainObject);

		Object* waterObject = new Object(waterShader, waterMaterial, planeGeometry);
		waterObject->pos = vec3(0, 0, 0);
		objects.push_back(waterObject);
		
		//Camera
		camera.setwEye(vec3(0, 5, 0));

		// Lights
		lights.resize(1);
		lights[0].wLightPos = vec4(50, 50, 50, 1);
		lights[0].Le = vec3(1.0, 1.0, 1.0);
		lights[0].La = vec3(0.5, 0.5, 0.5);
	}

	


	void getFPS(int& fps) {
		float currentTime = glfwGetTime();
		frameCount++;
		if (currentTime - previousTime >= 1.0) {
			fps = frameCount;
			frameCount = 0;
			previousTime = currentTime;
		}
	}

	void updateState(RenderState& state) {
		state.lights = lights;
		state.time = getTime();
		state.fogDensity = 0.1f;
		state.fogColor = vec3(0.7f, 0.9f, 1.0f);
		state.wEye = camera.getwEye();
		state.M = state.Minv = mat4(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
		state.V = camera.V();
		state.P = camera.P();
	}

	void moveCamera(int key) {
		camera.move(key);
	}

	void rotateCamera(float x, float y) {
		camera.rotate(x, y);
	}

	void setCameraFirstMouse() {
		camera.setFirstMouse();
	}
};
