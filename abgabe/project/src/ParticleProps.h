#pragma once
#include "vector.h"
#include "Color_A.h"
class ParticleProps
{
public:
	Vector position;
	Vector velocity, velocityVariation;
	Vector rotation;
	Vector rotationSpeed;
	Color_A colorBegin, colorEnd;

	float sizeBegin, sizeEnd, sizeVariation;
	float lifeTime = 1.0f;

	ParticleProps();
};
