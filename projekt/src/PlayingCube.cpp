#include "PlayingCube.h"
# define M_PI 3.14159265358979323846f

PlayingCube::PlayingCube(const char* cubeFile)
{
	
	Model* model = new Model(cubeFile, false);
	model->shader(new PhongShader(), true);

	this->setAngleInRadians(0);

	Matrix translation, scale, rotationZTest;
	scale.scale(0.4f);
	translation.translation(-25, 6, 0);
	//rotationZTest.rotationZ(AI_DEG_TO_RAD(-45));
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
	state = PlayerStates::airborne;
	Matrix resetRotation;
	setPreviousRotation(resetRotation.rotationZ(0)); //könnte auch identity nehmen?
}

void PlayingCube::applyGravityWhileFalling(float dtime)
{
	float newVelocity = this->getCurrentVelocityY() + this->getGravity() * dtime;
	std::cout << newVelocity << "newVel" << "\n";
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

void PlayingCube::initJumpVariables()
{
	maxJumpHeight = 9.1f;
	maxJumpTime = 0.5f; //fits with around 26frames for 60fps game

	float timeToApex = maxJumpTime / 2;
	gravity = (-2.0f * maxJumpHeight) / (timeToApex * timeToApex);

	//v0
	initialJumpVelocity = (2 * maxJumpHeight) / timeToApex;
	maxFallSpeed = -9.0f;
}
