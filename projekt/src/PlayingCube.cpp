#include "PlayingCube.h"

PlayingCube::PlayingCube(const char* cubeFile)
{
	
	Model* model = new Model(cubeFile, false);
	model->shader(new PhongShader(), true);

	Matrix m = Matrix();
	m.scale(0.4f);
	m.translation(40, 2, 0);
	model->transform(m);

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

void PlayingCube::update(float dtime)
{
	Matrix movement;
	movement.translation(-dtime, 0, 0);

	Matrix blockTransformCurrent = blockModel->transform();

	blockModel->transform(blockTransformCurrent * movement);
}
