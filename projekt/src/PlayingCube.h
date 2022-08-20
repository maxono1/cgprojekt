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
	void update(float dtime);
	void respawn();
	void setAngleInRadians(float angle);
	float getAngleInRadians();

private:
	Model* blockModel;
	bool jumpPressed;
	Matrix startPosition;
	float currentAngleInRadians;
};

