#pragma once
#include "framework.h"
#include "renderstate.h"

class ComputeShader {
	unsigned int shaderProgramId = 0;

public:
	ComputeShader() { shaderProgramId = 0; }

	virtual void Bind() = 0;

	unsigned int getId() { return shaderProgramId; }

	void create(const char* const computeShaderSource) {
		GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(computeShader, 1, &computeShaderSource, NULL);
		glCompileShader(computeShader);

		shaderProgramId = glCreateProgram();
		glAttachShader(shaderProgramId, computeShader);
		glLinkProgram(shaderProgramId);
	}

	void setUniform(const std::string& name, int i) {
		int location = glGetUniformLocation(shaderProgramId, name.c_str());;
		if (location >= 0) glUniform1i(location, i);
	}

	void setUniform(const std::string& name, float f) {
		int location = glGetUniformLocation(shaderProgramId, name.c_str());;
		if (location >= 0) glUniform1f(location, f);
	}

	~ComputeShader() { if (shaderProgramId > 0) glDeleteProgram(shaderProgramId); }
};
