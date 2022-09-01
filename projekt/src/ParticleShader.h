#pragma once
#include "BaseShader.h"
#include "Color_A.h"

#define PARTICLE_COUNT 1000
#define MATRIX_NUMBER_OF_FLOATS 16
#define COLORA_NUMBER_OF_FLOATS 4


class ParticleShader : public BaseShader
{
public:
	ParticleShader();
	void addMatrix(Matrix& mat, int index);
	void addColorToArray(const Color_A& color, int index);
	void setColorA(const Color_A& color) { this->colorA = Color_A(color); }
	const Color_A& getColorA() { return colorA; }
	virtual void activate(const BaseCamera& Cam) const;

	
private:
	Color_A colorA;
	//locations der einzelnen uniforms
	GLint ColorLoc;
	GLint ViewProjLoc;
	GLint ModelMatLoc;
	GLint ColorsLoc;

	//color Array damit jedes Partikel eine individuelle farbe hat
	//4 floats in der Color_A klasse
	float colors[PARTICLE_COUNT * COLORA_NUMBER_OF_FLOATS];

	//es muss ein inline float array verwendet werden, da ein array von matrizen erst umgewandelt werden müsste
	//bevor es an den glsl shader übergeben werden kann
	//16 floats in der matrix klasse
	float modelTransforms[PARTICLE_COUNT * MATRIX_NUMBER_OF_FLOATS]; 
	//man könnte auch ein color array machen für jedes partikel
};
