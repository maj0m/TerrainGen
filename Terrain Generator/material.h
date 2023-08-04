#pragma once
#include "framework.h"

struct Material {
	vec3 kd, ks, ka;
	float shininess;

public:
	Material(vec3 _kd, vec3 _ks, vec3 _ka, float _shininess) {
		kd = _kd;
		ks = _ks;
		ka = _ka;
		shininess = _shininess;
	}
};