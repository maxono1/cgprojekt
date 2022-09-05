#pragma once
#include "ParticleShader.h"
#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

ParticleShader::ParticleShader(): colorA(1,1,1,1)
{
	if (!load(ASSET_DIRECTORY "vsparticle.glsl", ASSET_DIRECTORY"fsparticle.glsl")) {
		throw std::exception();
	}
	//die location bekommen
	ColorLoc = glGetUniformLocation(ShaderProgram, "Color");
	ColorsLoc = glGetUniformLocation(ShaderProgram, "Colors");

	ViewProjLoc = glGetUniformLocation(ShaderProgram, "ViewProjMat");
	ModelMatLoc = glGetUniformLocation(ShaderProgram, "ModelMats");


	//initialisierung mit standardwerten der matrix

	Matrix mat;
	mat.identity();
	
	for (size_t i = 0; i < PARTICLE_COUNT; i++) {
		for (size_t j = 0; j < MATRIX_NUMBER_OF_FLOATS; j++) {
			//i * 16 ist der offset, 0 ist das erste partikel, 16 das zweite etc.
			modelTransforms[i * MATRIX_NUMBER_OF_FLOATS + j] = mat.m[j];
		}
	}

	for (size_t i = 0; i < PARTICLE_COUNT; i++) {
		for (size_t j = 0; j < COLORA_NUMBER_OF_FLOATS; j++) {
			//i * 16 ist der offset, 0 ist das erste partikel, 16 das zweite etc.
			colors[i * COLORA_NUMBER_OF_FLOATS + j] = 0.0f;
		}
	}
}

void ParticleShader::addMatrix(Matrix& mat, int index)
{
	for (size_t i = 0; i < MATRIX_NUMBER_OF_FLOATS; i++)
	{
		modelTransforms[index * MATRIX_NUMBER_OF_FLOATS + i] = mat.m[i];
	}
	
}

void ParticleShader::addColorToArray(const Color_A& color, int index)
{
	colors[index * COLORA_NUMBER_OF_FLOATS] = color.R;
	colors[index * COLORA_NUMBER_OF_FLOATS + 1] = color.G;
	colors[index * COLORA_NUMBER_OF_FLOATS + 2] = color.B;
	colors[index * COLORA_NUMBER_OF_FLOATS + 3] = color.A;

}

void ParticleShader::activate(const BaseCamera& Cam) const
{
	BaseShader::activate(Cam);
	glUniform4f(ColorLoc, colorA.R, colorA.G, colorA.B, colorA.A);

	//wir multiplizieren den transform später dran, deswegen wird hier bei der view proj mat nicht dranmultiplizeiret
	Matrix ModelViewProj = Cam.getProjectionMatrix() * Cam.getViewMatrix();
	glUniformMatrix4fv(ViewProjLoc, 1, GL_FALSE, ModelViewProj.m);
	
	glUniformMatrix4fv(ModelMatLoc, PARTICLE_COUNT, GL_FALSE, modelTransforms);
	glUniform4fv(ColorsLoc, PARTICLE_COUNT, colors);
}


