#pragma once
#include "vector.h"
#include "Color_A.h"
#include "Camera.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Application.h"
#include <vector>
#include <random>

//Quelle: https://www.youtube.com/watch?v=GK0jHlv3e3w&t=222s the cherno particle system

struct ParticleProps
{
	Vector position;
	Vector velocity, velocityVariation;
	Vector rotation;
	Vector rotationSpeed;
	Color_A colorBegin, colorEnd;

	float sizeBegin, sizeEnd, sizeVariation;
	float lifeTime = 1.0f;

	ParticleProps();
};

struct Particle
{

	Vector position;
	Vector velocity;
	Color_A colorBegin, colorEnd;
	Vector rotation;
	Vector rotationSpeed;

	float sizeBegin, sizeEnd;

	float lifeTime = 1.0f;
	float lifeRemaining = 0.0f;
};

class ParticleSystem : public BaseModel
{
public:
	void emit(const ParticleProps& particleProps);
	void update(float dtime);
	void draw(const BaseCamera& cam);
	ParticleProps createExampleProps();
private:
	void createParticleModel();

private:
	std::vector<Particle> particlePool;
	uint32_t poolIndex = 999;

	GLuint quadVA = 0;
	//shader
	//std::unique_ptr<
	GLint particleShaderViewProj, particleShaderTransform, particleShaderColor;
	
	VertexBuffer VB;
	IndexBuffer IB;
};

class Random
{
public:
	static void init()
	{
		s_RandomEngine.seed(std::random_device()());
	}
	//returns random float between 0 and 1
	static float Float()
	{
		return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
	}
private:
	//Mersenne twister pseudo random generator
	static std::mt19937 s_RandomEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};

