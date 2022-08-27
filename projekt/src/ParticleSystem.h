#pragma once
#include "vector.h"
#include "Color_A.h"
#include "Camera.h"
#include <vector>

struct ParticleProps
{
	Vector position;
	Vector velocity, velocityVariation;
	Color_A colorBegin, colorEnd;

	float sizeBegin, sizeEnd, sizeVariation;
	float lifeTime = 1.0f;
};

struct Particle
{

	Vector position;
	Vector velocity;
	Color_A colorBegin, colorEnd;
	

	float rotation;
	float sizeBegin, sizeEnd;

	float lifeTime = 1.0f;
	float lifeRemaining = 0.0f;
	//kann man auch lassen, da life remaining auch geht
	bool Active = false;
};

class ParticleSystem
{
public:
	void emit(const ParticleProps& particleProps);
	void update(float dtime);
	void draw(const BaseCamera& cam);
private:
	std::vector<Particle> particlePool;
	uint32_t poolIndex = 999;

	GLuint quadVA = 0;
	//shader
	//std::unique_ptr<
	GLint particleShaderViewProj, particleShaderTransform, particleShaderColor;
};



