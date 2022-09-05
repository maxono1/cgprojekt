#pragma once
#include "BaseModel.h"
#include "model.h"
#include "PhongShader.h"
#include <math.h>

enum class PlayerStates {
	airborne, grounded, continuousJump, dead, finish
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
	void applyGravityWhileFalling(float dtime);


	void setAngleInRadians(float angle);
	float getAngleInRadians();
	void setPreviousDTime(float previousDtime);
	float getPreviousDTime();
	void setPreviousRotation(Matrix previousRotation);
	Matrix getPreviousRotation();

	PlayerStates getPlayerState() { return state; };
	void setPlayerState(PlayerStates newState) { this->state = newState; };

	float getCurrentVelocityY() { return currentVelocityY; };
	void setCurrentVelocityY(float velocity) { this->currentVelocityY = velocity; };

	float getGravity() { return gravity; };
	float getInitialJumpVelocity() { return initialJumpVelocity; };
	float getMaxFallSpeed() { return maxFallSpeed; };

	float getDeathTimer() { return deathTimer; };
	void setDeathTimer(float value) { this->deathTimer = value; };
private:
	void initJumpVariables();
public:
	const float horizontalSpeed = 18.0f;

private:
	Model* blockModel;
	
	Matrix startPosition;
	float currentAngleInRadians;
	
	float previousDtime;
	Matrix previousRotation;

	PlayerStates state;
	bool jumpPressed;
	float initialJumpVelocity;
	float maxJumpHeight;
	float maxJumpTime;
	float maxFallSpeed;
	float gravity;

	float currentVelocityY;
	
	float deathTimer;
};

