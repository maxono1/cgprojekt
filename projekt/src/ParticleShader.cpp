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
	ViewProjLoc = glGetUniformLocation(ShaderProgram, "ViewProjMat");
	ModelMatLoc = glGetUniformLocation(ShaderProgram, "ModelMats");

	//initialisierung mit standardwerten der matrix

	Matrix mat;
	mat.identity();
	
	for (size_t i = 0; i < PARTICLE_COUNT; i++) {
		for (size_t j = 0; j < 16; j++) {
			//i * 16 ist der offset, 0 ist das erste partikel, 16 das zweite etc.
			modelTransforms[i * 16 + j] = mat.m[j];
		}
	}
}

void ParticleShader::addMatrix(Matrix& mat, int index)
{
	for (size_t i = 0; i < 16; i++)
	{
		modelTransforms[index * 16 + i] = mat.m[i];
	}
	
}

void ParticleShader::activate(const BaseCamera& Cam) const
{
	BaseShader::activate(Cam);
	glUniform4f(ColorLoc, colorA.R, colorA.G, colorA.B, colorA.A);

	//wir multiplizieren den transform später dran, deswegen wird hier bei der view proj mat nicht dranmultiplizeiret
	Matrix ModelViewProj = Cam.getProjectionMatrix() * Cam.getViewMatrix();
	glUniformMatrix4fv(ViewProjLoc, 1, GL_FALSE, ModelViewProj.m);
	
	glUniformMatrix4fv(ModelMatLoc, PARTICLE_COUNT, GL_FALSE, modelTransforms);

}


