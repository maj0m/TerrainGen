#pragma once
#include "framework.h"
#include "shader.h"
#include "material.h"
#include "light.h"
#include "texture.h"
#include "renderstate.h"
#include "terraintexture.h"


class WaterShader : public Shader {
	const char* vertexSource = R"(
		#version 330
		precision highp float;

		struct Light {
			vec3 La, Le;
			vec4 wLightPos;
		};
		
		uniform float waveLength;
		uniform float waveAmplitude;
		uniform float time;						// Current time in ms
		uniform mat4  MVP, M;					// MVP, Model
		uniform Light[8] lights;				// Light sources 
		uniform int   nLights;
		uniform vec3  wEye;						// Eye position

		layout(location = 0) in vec3  vtxPos;   // pos in modeling space
		layout(location = 1) in vec2  vtxUV;

		out vec3 wView;							// view in world space
		out vec3 wLight[8];						// light dir in world space
		out vec2 texcoord;
		out float distance;						// Distance from camera



		vec3 waveOffset(vec3 vertex) {
			float x = (vertex.x / waveLength + time / 10000.0) * 2.0 * 3.1415;
			float z = (vertex.z / waveLength + time / 10000.0) * 2.0 * 3.1415;
			float offset = (sin(x) + cos(z)) * waveAmplitude;
			return vertex + vec3(offset);
		}
		
		void main() {
			vec3 vertexPos = vtxPos;
			vertexPos = waveOffset(vertexPos);
			gl_Position = vec4(vertexPos, 1) * MVP; // to NDC
			vec4 wPos = vec4(vertexPos, 1) * M;
			for(int i = 0; i < nLights; i++) {
				wLight[i] = lights[i].wLightPos.xyz * wPos.w - wPos.xyz * lights[i].wLightPos.w;
			}
		    wView  = wEye * wPos.w - wPos.xyz;
		    texcoord = vtxUV;
			distance = length(wPos.xyz - wEye);
		}
	)";

	const char* fragmentSource = R"(
	#version 330
	precision highp float;

	struct Light {
		vec3 La, Le;
		vec4 wLightPos;
	};

	struct Material {
		vec3 kd, ks, ka;
		float shininess;
	};

	uniform float waterAlpha;
	uniform float time;
	uniform vec3 waterColor;
	uniform Material material;
	uniform Light[8] lights;
	uniform int   nLights;
	uniform sampler2D terrainTexture;

	in  vec3 wView;						// interpolated world sp view
	in  vec3 wLight[8];					// interpolated world sp illum dir
	in  vec2 texcoord;
	in float distance;					
	
	out vec4 fragmentColor;				// output goes to frame buffer

	vec3 texColor = vec3(0.0, 0.5, 1.0);
	vec3 foamColor = vec3(1.0, 1.0, 1.0);
	
	void main() {
		vec3 xTangent = dFdx(wView);
		vec3 yTangent = dFdy(wView);
		vec3 N = normalize(cross(xTangent, yTangent));
		vec3 V = normalize(wView); 
		
		float aplha = waterAlpha;
		float terrainHeight = texture(terrainTexture, texcoord).r;
		float epsilon = 0.1 * (sin(time / 5000.0) + 1.2) / 2.0;
		if(terrainHeight > 0.5 - epsilon && terrainHeight < 0.5 + epsilon) {
			float difference = abs(0.5 - terrainHeight) / epsilon;
			texColor = mix(foamColor, texColor, difference);
			aplha = mix(1.0, waterAlpha, difference);
		}

		vec3 ka = material.ka * texColor;
		vec3 kd = material.kd * texColor;

		vec3 radiance = vec3(0, 0, 0);
		for(int i = 0; i < nLights; i++) {
			vec3 L = normalize(wLight[i]);
			vec3 H = normalize(L + V);
			float cost = max(dot(N,L), 0);
			float cosd = max(dot(N,H), 0);
			radiance += ka * lights[i].La + (kd * texColor * cost + material.ks * pow(cosd, material.shininess)) * lights[i].Le;
		}
		
		fragmentColor = vec4(radiance, aplha);
	}
)";

public:

	WaterShader() {
		create(vertexSource, fragmentSource, "fragmentColor");
	}

	void Bind(RenderState state) {
		Use();

		setUniform(*state.terrainTexture, std::string("terrainTexture"));
		setUniform(state.waveLength, "waveLength");
		setUniform(state.waveAmplitude, "waveAmplitude");
		setUniform(state.waterAlpha, "waterAlpha");
		setUniform(state.time, "time");
		setUniform(state.MVP, "MVP");
		setUniform(state.M, "M");
		setUniform(state.wEye, "wEye");
		setUniformMaterial(*state.material, "material");
		setUniform((int)state.lights.size(), "nLights");
		for (unsigned int i = 0; i < state.lights.size(); i++) {
			setUniformLight(state.lights[i], std::string("lights[") + std::to_string(i) + std::string("]"));
		}
	}
};
