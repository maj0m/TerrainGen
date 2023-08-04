#pragma once
#include "framework.h";
#include "texture.h"


class GPUProgram {
	unsigned int shaderProgramId = 0;
	unsigned int vertexShader = 0, geometryShader = 0, fragmentShader = 0;
	bool waitError = true;

	void getErrorInfo(unsigned int handle) { // shader error report
		int logLen, written;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			std::string log(logLen, '\0');
			glGetShaderInfoLog(handle, logLen, &written, &log[0]);
			printf("Shader log:\n%s", log.c_str());
			if (waitError) getchar();
		}
	}

	bool checkShader(unsigned int shader, std::string message) { // check if shader could be compiled
		int OK;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &OK);
		if (!OK) {
			printf("%s!\n", message.c_str());
			getErrorInfo(shader);
			return false;
		}
		return true;
	}

	bool checkLinking(unsigned int program) { 	// check if shader could be linked
		int OK;
		glGetProgramiv(program, GL_LINK_STATUS, &OK);
		if (!OK) {
			printf("Failed to link shader program!\n");
			getErrorInfo(program);
			return false;
		}
		return true;
	}

	int getLocation(const std::string& name) {	// get the address of a GPU uniform variable
		int location = glGetUniformLocation(shaderProgramId, name.c_str());
		if (location < 0) printf("uniform %s cannot be set\n", name.c_str());
		return location;
	}

public:
	GPUProgram(bool _waitError = true) { shaderProgramId = 0; waitError = _waitError; }

	GPUProgram(const GPUProgram& program) {
		if (program.shaderProgramId > 0) printf("\nError: GPU program is not copied on GPU!!!\n");
	}

	void operator=(const GPUProgram& program) {
		if (program.shaderProgramId > 0) printf("\nError: GPU program is not copied on GPU!!!\n");
	}

	unsigned int getId() { return shaderProgramId; }

	bool create(const char* const vertexShaderSource,
		const char* const fragmentShaderSource, const char* const fragmentShaderOutputName,
		const char* const geometryShaderSource = nullptr)
	{
		// Create vertex shader from string
		if (vertexShader == 0) vertexShader = glCreateShader(GL_VERTEX_SHADER);
		if (!vertexShader) {
			printf("Error in vertex shader creation\n");
			exit(1);
		}
		glShaderSource(vertexShader, 1, (const GLchar**)&vertexShaderSource, NULL);
		glCompileShader(vertexShader);
		if (!checkShader(vertexShader, "Vertex shader error")) return false;

		// Create geometry shader from string if given
		if (geometryShaderSource != nullptr) {
			if (geometryShader == 0) geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
			if (!geometryShader) {
				printf("Error in geometry shader creation\n");
				exit(1);
			}
			glShaderSource(geometryShader, 1, (const GLchar**)&geometryShaderSource, NULL);
			glCompileShader(geometryShader);
			if (!checkShader(geometryShader, "Geometry shader error")) return false;
		}

		// Create fragment shader from string
		if (fragmentShader == 0) fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		if (!fragmentShader) {
			printf("Error in fragment shader creation\n");
			exit(1);
		}

		glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		if (!checkShader(fragmentShader, "Fragment shader error")) return false;

		shaderProgramId = glCreateProgram();
		if (!shaderProgramId) {
			printf("Error in shader program creation\n");
			exit(1);
		}
		glAttachShader(shaderProgramId, vertexShader);
		glAttachShader(shaderProgramId, fragmentShader);
		if (geometryShader > 0) glAttachShader(shaderProgramId, geometryShader);

		// Connect the fragmentColor to the frame buffer memory
		glBindFragDataLocation(shaderProgramId, 0, fragmentShaderOutputName);	// this output goes to the frame buffer memory

		// program packaging
		glLinkProgram(shaderProgramId);
		if (!checkLinking(shaderProgramId)) return false;

		// make this program run
		glUseProgram(shaderProgramId);
		return true;
	}

	void Use() { 		// make this program run
		glUseProgram(shaderProgramId);
	}

	void setUniform(int i, const std::string& name) {
		int location = getLocation(name);
		if (location >= 0) glUniform1i(location, i);
	}

	void setUniform(float f, const std::string& name) {
		int location = getLocation(name);
		if (location >= 0) glUniform1f(location, f);
	}

	void setUniform(const vec2& v, const std::string& name) {
		int location = getLocation(name);
		if (location >= 0) glUniform2fv(location, 1, &v.x);
	}

	void setUniform(const vec3& v, const std::string& name) {
		int location = getLocation(name);
		if (location >= 0) glUniform3fv(location, 1, &v.x);
	}

	void setUniform(const vec4& v, const std::string& name) {
		int location = getLocation(name);
		if (location >= 0) glUniform4fv(location, 1, &v.x);
	}

	void setUniform(const mat4& mat, const std::string& name) {
		int location = getLocation(name);
		if (location >= 0) glUniformMatrix4fv(location, 1, GL_TRUE, mat);
	}

	void setUniform(const Texture& texture, const std::string& samplerName, unsigned int textureUnit = 0) {
		int location = getLocation(samplerName);
		if (location >= 0) {
			glUniform1i(location, textureUnit);
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_2D, texture.textureId);
		}
	}

	// List of vec3s
	void setUniform(const std::vector<vec3>& values, const std::string& name) {
		int location = getLocation(name);
		if (location >= 0) glUniform3fv(location, values.size(), &values[0].x);

	}

	~GPUProgram() { if (shaderProgramId > 0) glDeleteProgram(shaderProgramId); }
};
