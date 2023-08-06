#pragma once
#include "framework.h"


class Geometry {
protected:
	unsigned int vao, vbo;
	unsigned int nVtxStrip, nStrips;

	struct VertexData {
		vec3 pos;
		vec2 tex;
	};

	virtual VertexData GenVertexData(float u, float v) {
		VertexData vd;
		vd.tex = vec2(u, v);
		eval(u, v, vd.pos);

		return vd;
	}

	virtual void eval(float u, float v, vec3& pos) = 0;

public:
	Geometry() {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
	}

	void create(int N, int M) {
		nVtxStrip = (M + 1) * 2;
		nStrips = N;
		std::vector<VertexData> vtxData;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j <= M; j++) {
				vtxData.push_back(GenVertexData((float)j / M, (float)i / N));
				vtxData.push_back(GenVertexData((float)j / M, (float)(i + 1) / N));
			}
		}
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vtxData.size() * sizeof(VertexData), &vtxData[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); // AttArr 0 = POSITION
		glEnableVertexAttribArray(1); // AttArr 1 = UV
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, pos));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, tex));
	}

	void Draw() {
		glBindVertexArray(vao);
		for (int i = 0; i < nStrips; i++)
			glDrawArrays(GL_TRIANGLE_STRIP, i * nVtxStrip, nVtxStrip);
	}

	virtual ~Geometry() {
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}
};
