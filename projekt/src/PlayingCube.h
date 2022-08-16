#pragma once
#include "BaseModel.h"
#include "model.h"
#include "PhongShader.h"

class PlayingCube
{
public:
	PlayingCube(const char* cubeFile);
	virtual ~PlayingCube();
	void jump();
	Model* getBlockModel();

private:
	Model* blockModel;
	bool jumpPressed;
};

