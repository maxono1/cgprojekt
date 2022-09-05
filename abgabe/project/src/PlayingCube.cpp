#include "PlayingCube.h"
# define M_PI 3.14159265358979323846f

PlayingCube::PlayingCube(const char* cubeFile) : deathTimer(0.0f)
{
	
	Model* model = new Model(cubeFile, false);
	model->shader(new PhongShader(), true);

	this->setAngleInRadians(0);

	Matrix translation, scale, rotationZTest;
	scale.scale(0.4f);
	translation.translation(-16, 6, 0);
	startPosition = translation * scale;
	model->transform(startPosition);

	currentVelocityY = 0.0f;
	
	initJumpVariables();

	blockModel = model;
	state = PlayerStates::airborne; // start on the ground? maybe its better to start in the air, or always falling?
	setPreviousRotation(rotationZTest.rotationZ(0));
}

PlayingCube::~PlayingCube()
{
	// blockModel is deleted automaticallly
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
	state = PlayerStates::airborne;
	Matrix resetRotation;
	setPreviousRotation(resetRotation.rotationZ(0)); //könnte auch identity nehmen?
}

// Quellen :
// https://www.youtube.com/watch?v=h2r3_KjChf4
// https://www.youtube.com/watch?v=hG9SzQxaCm8
void PlayingCube::applyGravityWhileFalling(float dtime)
{
	float newVelocity = this->getCurrentVelocityY() + this->getGravity() * dtime;
	//std::cout << newVelocity << "newVel" << "\n";
	if (newVelocity < this->getMaxFallSpeed()) {
		newVelocity = this->getMaxFallSpeed();
	}
	this->setCurrentVelocityY(newVelocity);
}

void PlayingCube::setAngleInRadians(float angle)
{
	//if(state == PlayerStates::grounded)
	if (angle < -(0.05f)) {
		this->currentAngleInRadians = 0;
	}
	this->currentAngleInRadians = angle;

}
float PlayingCube::getAngleInRadians()
{
	return this->currentAngleInRadians;
}

void PlayingCube::setPreviousDTime(float previousDtime)
{
	this->previousDtime = previousDtime;
}

float PlayingCube::getPreviousDTime()
{
	return this->previousDtime;
}

void PlayingCube::setPreviousRotation(Matrix previousRotation)
{
	this->previousRotation = previousRotation;
}

Matrix PlayingCube::getPreviousRotation()
{
	return this->previousRotation;
}

// Quellen :
// https://www.youtube.com/watch?v=h2r3_KjChf4
// https://www.youtube.com/watch?v=hG9SzQxaCm8
void PlayingCube::initJumpVariables()
{
	maxJumpHeight = 9.1f;
	maxJumpTime = 0.3f; //fits with around 26frames for 60fps game

	float timeToApex = maxJumpTime / 2;
	gravity = (-2.0f * maxJumpHeight) / (timeToApex * timeToApex);

	//v0
	initialJumpVelocity = (2 * maxJumpHeight) / timeToApex;
	maxFallSpeed = -18.0f;
}
