
#include "ParticleShader.h"

ParticleShader::ParticleShader()
{

	ColorLoc = glGetUniformLocation(ShaderProgram, "Color");
	ViewProjLoc = glGetUniformLocation(ShaderProgram, "ViewProjMat");
	ModelMatLoc = glGetUniformLocation(ShaderProgram, "ModelMats");
	
}


