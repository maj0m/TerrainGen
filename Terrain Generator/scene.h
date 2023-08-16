#pragma once
#include "framework.h"
#include "camera.h"
#include "material.h"
#include "renderstate.h"
#include "light.h"
#include "material.h"
#include "object.h"
#include "sphere.h"
#include "terraintexture.h"
#include "terrainshader.h"
#include "plane.h"
#include "watershader.h"
#include <iostream>

const int gui_width = 300;
const int gui_height = 600;
const int tesselation = 256;
const int scale = 100;

int fps;
int frameCount = 0;
float previousTime = glfwGetTime();



class Scene {
	Camera camera;
	RenderState state;
	std::vector<Object*> objects;
	std::vector<Light> lights;

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
		state.wEye = camera.getEyePos();
		state.M = mat4(	1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1);
		state.V = camera.V();
		state.P = camera.P();
	}

public:
	void Render() {
		glViewport(0, 0, windowWidth, windowHeight);
		updateState(state);
		for (Object* obj : objects) { obj->Draw(state); }
		drawGUI(windowWidth - gui_width, 0, gui_width, gui_height);
	}

	void Build() {
		//Camera
		camera.setEyePos(vec3(0, 30, 80));
		camera.setEyeDir(vec3(0, -0.5, -1));

		// State
		state.waveLength = 10.0;
		state.waveAmplitude = 0.02;
		state.waterAlpha = 0.4;
		state.fogDensity = 0.1;
		state.fogColor = vec3(0.7f, 0.9f, 1.0f);
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

		// Lights
		lights.resize(1);
		lights[0].wLightPos = vec4(0, 50, 0, 1);
		lights[0].Le = vec3(1.0, 1.0, 1.0);
		lights[0].La = vec3(0.5, 0.5, 0.5);
	}



	void drawGUI(int x, int y, int w, int h) {
		ImGui::SetNextWindowPos(ImVec2(x, y));
		ImGui::SetNextWindowSize(ImVec2(w, h));
		ImGui::Begin("Settings");

		// Display FPS
		getFPS(fps);
		ImGui::Text("FPS: %d", fps);
		ImGui::SliderInt("texture dim", &terrainTextureWidth, 0, 256);
		ImGui::SliderInt("texture dim", &terrainTextureHeight, 0, 256);

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		// Terrain sliders
		ImGui::SliderFloat("noise freq", &terrainFrequency, 0.0, 10.0, "%.1f");
		ImGui::SliderFloat("noise ampl", &terrainAmplitude, 0.0, 25.0, "%.1f");
		ImGui::SliderInt("noise octs", &terrainOctaves, 0, 12);
		ImGui::SliderInt("noise seed", &terrainSeed, 0, 1000);


		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		// Water Sliders
		ImGui::SliderFloat("wavelength", &state.waveLength, 0.0, 25.0, "%.1f");
		ImGui::SliderFloat("wave ampl", &state.waveAmplitude, 0.0, 1.0, "%.2f");
		ImGui::SliderFloat("water alpha", &state.waterAlpha, 0.0, 1.0, "%.2f");

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		// Erosion sliders
		ImGui::Checkbox("erosion", &terrainErosion);
		ImGui::SliderInt("iterations", &erosionIterations, 1, 20);
		ImGui::SliderFloat("min volume", &erosionMinVolume, 0.0, 1.0, "%.1f");
		ImGui::SliderFloat("density", &erosionDensity, 0.0, 2.0, "%.1f");
		ImGui::SliderFloat("evap rate", &erosionEvaporationRate, 0.001, 0.1, "%.3f");
		ImGui::SliderFloat("depos rate", &erosionDepositionRate, 0.0, 1.0, "%.2f");
		ImGui::SliderFloat("friction", &erosionFriction, 0.0, 0.5, "%.2f");

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		// Generate new Terrain
		if (ImGui::Button("Update terrain")) {
			state.terrainTexture = new TerrainTexture();
		}

		ImGui::End();
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
