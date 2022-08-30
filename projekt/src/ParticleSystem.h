#pragma once
#include "vector.h"
#include "Color_A.h"
#include "Camera.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BaseModel.h"
#include "ParticleProps.h"
#include "Random.h"
#include "ParticleShader.h"
#include <vector>
#include <random>


static float randomFloat(float a, float b);

//Quelle: https://www.youtube.com/watch?v=GK0jHlv3e3w&t=222s the cherno particle system

class ParticloSystem : public BaseModel
{
public:
	ParticloSystem();
	void emit(const ParticleProps& particleProps);
	void update(float dtime);
	void draw(const BaseCamera& cam);
	ParticleProps createExampleProps();
private:
	void createParticleModel();
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

private:
	std::vector<Particle> particlePool;
	int poolIndex = PARTICLE_COUNT - 1;

	GLuint quadVA = 0;
	//shader
	//std::unique_ptr<
	GLint particleShaderViewProj, particleShaderTransform, particleShaderColor;
	
	VertexBuffer VB;
	IndexBuffer IB;
};



