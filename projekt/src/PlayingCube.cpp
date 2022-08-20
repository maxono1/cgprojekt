#include "PlayingCube.h"

PlayingCube::PlayingCube(const char* cubeFile)
{
	
	Model* model = new Model(cubeFile, false);
	model->shader(new PhongShader(), true);

	Matrix translation, scale;
	scale.scale(0.4f);
	translation.translation(-8, 2, 0);
	startPosition = translation * scale;
	model->transform(startPosition);
	
	blockModel = model;
}

PlayingCube::~PlayingCube()
{
	//TODO: delete things that i created with new
	// blockModel is deleted automaticallly
	//delete blockModel;
}

void PlayingCube::jump()
{
}

Model* PlayingCube::getBlockModel()
{
	return blockModel;
}

//wird momentan nicht aufgerufen
void PlayingCube::update(float dtime)
{
	Matrix movement;
	movement.translation(-dtime, 0, 0);

	Matrix blockTransformCurrent = blockModel->transform();

	blockModel->transform(blockTransformCurrent * movement);
}

void PlayingCube::respawn()
{
	blockModel->transform(startPosition);
}
