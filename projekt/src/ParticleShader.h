#pragma once
#include "BaseShader.h"
#include "Color_A.h"

#define PARTICLE_COUNT 100

class ParticleShader : public BaseShader
{
public:
	ParticleShader();
	void addMatrix(Matrix& mat, int index);
	void setColorA(const Color_A& color) { this->colorA = Color_A(color); }
	const Color_A& getColorA() { return colorA; }
	virtual void activate(const BaseCamera& Cam) const;
	
private:
	Color_A colorA;
	//locations der einzelnen uniforms
	GLint ColorLoc;
	GLint ViewProjLoc;
	GLint ModelMatLoc;
	//es muss ein inline float array verwendet werden, da ein array von matrizen erst umgewandelt werden m�sste
	//bevor es an den glsl shader �bergeben werden kann
	float modelTransforms[PARTICLE_COUNT * 16]; 
	//man k�nnte auch ein color array machen f�r jedes partikel
};
