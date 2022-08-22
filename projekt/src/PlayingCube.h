#pragma once
#include "BaseModel.h"
#include "model.h"
#include "PhongShader.h"

enum class PlayerStates {
	jumping, falling, grounded
};

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
	void setPreviousDTime(float previousDtime);
	float getPreviousDTime();
	void setPreviousRotation(Matrix previousRotation);
	Matrix getPreviousRotation();

	PlayerStates getPlayerState() { return state; };
	void setPlayerState(PlayerStates newState) { this->state = newState; };

private:
	Model* blockModel;
	bool jumpPressed;
	Matrix startPosition;
	float currentAngleInRadians;
	PlayerStates state;
	float previousDtime;
	Matrix previousRotation;
};

