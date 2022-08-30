//
//  Application.cpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Application.h"
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include "lineplanemodel.h"
#include "triangleplanemodel.h"
#include "trianglespheremodel.h"
#include "lineboxmodel.h"
#include "triangleboxmodel.h"
#include "model.h"
#include "ShaderLightmapper.h"
#include "utils.h"


#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif


Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin), pModel(NULL), ShadowGenerator(2048, 2048)
{
	//createScene();
	//createNormalTestScene();
	//createShadowTestScene();
	createGeometryTestScene();


}
void Application::start()
{
    glEnable (GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}



void Application::update(float dtime)
{
	//um teleportieren beim debugging zu vermeiden! dort sind die dtime 4+ sekunden
	if (dtime > 0.01f) {
		dtime = 0.01f;
	}
	std::cout << dtime << "dtime" << "\n";
	handleKeyPresses();

	//unrotate
	Matrix previousRotation = player->getPreviousRotation(); //needed at the end, to continue rotation
	Matrix invOfpreviousRotation = Matrix(previousRotation);
	invOfpreviousRotation.invert();

	Model* playerModel = player->getBlockModel();
	playerModel->transform(playerModel->transform() * invOfpreviousRotation);
	
	if (isJumpPressed() && (player->getPlayerState() == PlayerStates::grounded || player->getPlayerState() == PlayerStates::continuousJump)) {
		player->setCurrentVelocityY(player->getInitialJumpVelocity());
	}


	//movement down and collision detect
	Matrix movementY;
	//movementY.translation(0, dtime * (-5), 0);
	float completeVelocity = player->getCurrentVelocityY() * dtime + (player->getGravity() * dtime * dtime) / 2;
	std::cout << completeVelocity << "completeVel" << "\n";
	movementY.translation(0, completeVelocity ,0);
	
	if (completeVelocity <= 0.0f) {
		Matrix transformBeforeMoveDown = playerModel->transform(); //in case of collision, reset to this
		playerModel->transform(transformBeforeMoveDown * movementY);

		//das hier nur machen wenn movement down ist
		AABB bbOfPlayer = playerModel->boundingBox().transform(player->getBlockModel()->transform());
		bool collisionHappenedOnce = false;
		for (int i{ 0 }; i < lvlObjects.size(); i++)
		{
			AABB bbOfObject = lvlObjects[i]->boundingBox().transform(lvlObjects[i]->transform());
			bool collision = AABB::checkCollision(bbOfPlayer, bbOfObject);
			if (collision) {
				player->getBlockModel()->transform(transformBeforeMoveDown);
				collisionHappenedOnce = true;
			}
		}
		//setting the current velocity might be best if it happens here
		if (collisionHappenedOnce) {
			if (!isJumpPressed()) {
				player->setPlayerState(PlayerStates::grounded);
			}
			else {
				player->setPlayerState(PlayerStates::continuousJump);
				//this is to keep rotation when you hold jump while touching the ground
			}
			float newVelocity = -0.01f;
			std::cout << newVelocity << "newVel" << "\n";
			player->setCurrentVelocityY(newVelocity);
		}
		else {
			player->setPlayerState(PlayerStates::airborne);
			player->applyGravityWhileFalling(dtime);
		}
	}
	else {
		Matrix transformBeforeMoveUp = playerModel->transform();
		playerModel->transform(transformBeforeMoveUp * movementY);


		AABB bbOfPlayer = playerModel->boundingBox().transform(player->getBlockModel()->transform());
		for (int i{ 0 }; i < lvlObjects.size(); i++)
		{
			AABB bbOfObject = lvlObjects[i]->boundingBox().transform(lvlObjects[i]->transform());
			bool collision = AABB::checkCollision(bbOfPlayer, bbOfObject);
			if (collision)
			{
				player->respawn(); //respawn setted falling anyway
			}
		}
		player->setPlayerState(PlayerStates::airborne);
		player->applyGravityWhileFalling(dtime);
	}
	
	
	

	//sideways movement and collision:
	Matrix movementSide;
	movementSide.translation(dtime * player->horizontalSpeed, 0, 0);
	playerModel->transform(playerModel->transform() * movementSide);
	//update bounding box
	AABB bbOfPlayer = playerModel->boundingBox().transform(playerModel->transform());

	for (int i{ 0 }; i < lvlObjects.size(); i++)
	{
		AABB bbOfObject = lvlObjects[i]->boundingBox().transform(lvlObjects[i]->transform());
		bool collision = AABB::checkCollision(bbOfPlayer, bbOfObject);
		if (collision)
		{
			player->respawn();
		}
	}
	
	for (int i = 0; i < obstacles.size(); i++)
	{
		AABB bbOfObstacle = obstacles[i]->boundingBox().transform(obstacles[i]->transform());
		bool collision = AABB::checkCollision(bbOfPlayer, bbOfObstacle);
		if (collision) {
			player->respawn();
		}
	}

	rotatePlayerModel(dtime, previousRotation, playerModel);
	
	
	Vector playerPositionAfter = playerModel->transform().translation();
	if (player->getPlayerState() == PlayerStates::grounded) {
		particlePropsTest = ParticleProps();
		particlePropsTest.position = Vector(playerPositionAfter.X - 0.5f, playerPositionAfter.Y - 0.25f, playerPositionAfter.Z);
		particlePropsTest.sizeBegin = 0.2f;
		for (size_t i = 0; i < 1; i++)
		{
			particleSystem->emit(particlePropsTest);
		}
	}
	

	particleSystem->update(dtime);

	//make camera follow the block
	Cam.setPosition(Vector(playerPositionAfter.X - 2, playerPositionAfter.Y + 3, playerPositionAfter.Z - 10 ));
	Cam.setTarget(playerPositionAfter);
    Cam.update();
}

void Application::draw()
{
	ShadowGenerator.generate(Models);
	
    // 1. clear screen
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShaderLightMapper::instance().activate();
    // 2. setup shaders and draw models
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
    {
        (*it)->draw(Cam);
    }
	ShaderLightMapper::instance().deactivate();
	
    // 3. check once per frame for opengl errors
    GLenum Error = glGetError();
    assert(Error==0);

	particleSystem->draw(Cam);
}
void Application::end()
{
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
        delete *it;
    
    Models.clear();
	delete player;
	delete phongShader;
	delete particleSystem;
}

bool Application::isJumpPressed()
{
	return jumpPressed;
}

void Application::createGeometryTestScene()
{
	/// <summary>
	/// wenn man die skybox als erstes macht, dann ist einfach teil der skybox holz!!!
	/// </summary>

	//wooden box length in units : 6.0f genau
	//wooden box height in units : 0.4f 
	float woodenBoxLength = 6.0f;
	float woodenBoxHeight = 0.4f;

	Matrix translation, rotation, scale;
	ConstantShader* pConstShader = new ConstantShader();
	pConstShader->color(Color(0, 1, 0));

	phongShader = new PhongShader();

	for (int i{ 0 }; i < 15; i++) {
		pModel = new Model(ASSET_DIRECTORY "woodenObje.obj", false);
		pModel->shader(phongShader, false);
		lvlObjects.push_back(pModel);
		Models.push_back(pModel);
	}
	//lvl objects [0] wird nich verwendet

	translation.translation(-8.0f, -4, 0);
	lvlObjects[1]->transform(translation);

	translation.translation(-5.0f, -5.5f, 0);
	lvlObjects[2]->transform(translation);

	translation.translation(1.0f, -5.5f, 0);
	lvlObjects[3]->transform(translation);
	
	translation.translation(7.0f, -5.5f, 0);
	lvlObjects[4]->transform(translation);

	translation.translation(13.0f, -5.5f, 0);
	lvlObjects[5]->transform(translation);

	translation.translation(19.0f, -5.5f, 0);
	lvlObjects[6]->transform(translation);

	translation.translation(25.0f, -5.5f, 0);
	lvlObjects[7]->transform(translation);

	translation.translation(31.0f, -5.5f, 0);
	lvlObjects[8]->transform(translation);

	translation.translation(34.0f, -5.5f, 0);
	rotation.rotationX(AI_DEG_TO_RAD(90));
	lvlObjects[9]->transform(translation * rotation);

	translation.translation(37.0f, -5.5f, 0);
	lvlObjects[10]->transform(translation);

	translation.translation(40.0f, -5.5f, 0);
	rotation.rotationZ(AI_DEG_TO_RAD(90));
	lvlObjects[11]->transform(translation * rotation);

	translation.translation(43.0f, -5.5f, 0);
	lvlObjects[12]->transform(translation);


	

	for (int i{ 0 }; i < lvlObjects.size(); i++) 
	{
		LineBoxModel* lvlHitbox = new LineBoxModel(lvlObjects[i]->boundingBox().Max, lvlObjects[i]->boundingBox().Min);
		//das delete on destruction könnte fehler aufrufen
		lvlHitbox->shader(pConstShader, true);
		lvlHitbox->transform(lvlObjects[i]->transform());
		//Models.push_back(lvlHitbox);
		lvlHitboxVisuals.push_back(lvlHitbox);
	}


	pModel = new Model(ASSET_DIRECTORY "tripleSpikes.obj", false);
	pModel->shader(new PhongShader(), true);
	translation.translation(3.0f, -5.5f, 0);
	scale.scale(1);
	pModel->transform(translation * scale);
	obstacles.push_back(pModel);
	Models.push_back(pModel);

	for (int i{ 0 }; i < obstacles.size(); i++) 
	{
		LineBoxModel* obstacleHitbox = new LineBoxModel(obstacles[i]->boundingBox().Max, obstacles[i]->boundingBox().Min);
		obstacleHitbox->shader(pConstShader, true);
		obstacleHitbox->transform(obstacles[i]->transform());
		//Models.push_back(obstacleHitbox);
		obstacleHitboxVisuals.push_back(obstacleHitbox);
	}

	/*
	AABB bbOfModel = pModel->boundingBox();// .transform(pModel->transform());
	//std::cout << AABB::checkCollision(bbOfPlayer, bbOfModel);
	std::cout << "model box pos: " << bbOfModel.Max.X << ", " << bbOfModel.Max.Y << " , " << bbOfModel.Max.Z << " min"
		<< bbOfModel.Min.X << ", " << bbOfModel.Min.Y << ", " << bbOfModel.Min.Z << "\n";
	*/
	/*
	pModel = new Model(ASSET_DIRECTORY "woodenBox.dae", false);
	pModel->shader(new PhongShader(), true);
	translation.translation(0.0f, 0, 0);
	scale.scale(1);
	pModel->transform(translation * scale);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "woodenBox.dae", false);
	pModel->shader(new PhongShader(), true);
	translation.translation(6.0f, 0, 0);
	scale.scale(1);
	pModel->transform(translation * scale);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "woodenBox.dae", false);
	pModel->shader(new PhongShader(), true);
	translation.translation(12.0f, 0, 0);
	scale.scale(1);
	pModel->transform(translation * scale);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "woodenBox.dae", false);
	pModel->shader(new PhongShader(), true);
	translation.translation(12.0f, 0.4f, 0);
	scale.scale(1);
	pModel->transform(translation * scale);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "woodenBox.dae", false);
	pModel->shader(new PhongShader(), true);
	//jedes mal wird ne neue matrix erstellt!!!!"
	translation.translation(12.0f, 0, 0);
	rotation.rotationZ(AI_DEG_TO_RAD(90));
	scale.scale(1);
	pModel->transform(translation * rotation * scale );
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "woodenBox.dae", false);
	pModel->shader(new PhongShader(), true);
	//jedes mal wird ne neue matrix erstellt!!!!"
	translation.translation(18.0f, -6.0f, 0);
	//rotation.rotationZ(AI_DEG_TO_RAD(90));
	scale.scale(1);
	pModel->transform(translation * scale);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "woodenBox.dae", false);
	pModel->shader(new PhongShader(), true);
	//jedes mal wird ne neue matrix erstellt!!!!"
	translation.translation(24.0f, -6.0f, 0);
	//rotation.rotationZ(AI_DEG_TO_RAD(90));
	scale.scale(1);
	pModel->transform(translation * scale);
	Models.push_back(pModel);*/

	//aabbList.reserve(sizeof(AABB) * 20);
	/*
	for (ModelList::iterator it = Models.begin(); it != Models.end(); ++it)
	{
		lvlList.push_back((*it));

	}*/

	player = new PlayingCube(ASSET_DIRECTORY "blockPlayer.obj");
	Models.push_back(player->getBlockModel());

	
	playerHitboxVisual = new LineBoxModel(player->getBlockModel()->boundingBox().Max, player->getBlockModel()->boundingBox().Min);
	playerHitboxVisual->shader(pConstShader, true);
	//Models.push_back(playerHitboxVisual);

	/*
	dragonCube = new LineBoxModel(pModel->boundingBox().Max, pModel->boundingBox().Min);
	dragonCube->shader(pConstShader, true);
	Models.push_back(dragonCube);*/

	pModel = new Model(ASSET_DIRECTORY "skybox_bright.obj", false);
	pModel->shader(new PhongShader(), true);
	pModel->shadowCaster(false);
	Models.push_back(pModel);


	// directional lights
	DirectionalLight* dl = new DirectionalLight();
	dl->direction(Vector(0.2f, -1, 1));
	dl->color(Color(1, 1, 1));
	dl->castShadows(true);
	ShaderLightMapper::instance().addLight(dl);



	particlePropsTest = ParticleProps();
	particleSystem = new ParticloSystem();
	particleShader = new ParticleShader();
	particleShader->setColorA(Color_A(1, 0, 0, 1));
	particleSystem->shader(particleShader, true);
}

void Application::createScene()
{
	Matrix m,n;

	pModel = new Model(ASSET_DIRECTORY "skybox.obj", false);
	pModel->shader(new PhongShader(), true);
	pModel->shadowCaster(false);
	Models.push_back(pModel);


	pModel = new Model(ASSET_DIRECTORY "scene.dae", false);
	pModel->shader(new PhongShader(), true);
	m.translation(10, 0, -10);
	pModel->transform(m);
	Models.push_back(pModel);
	

	// directional lights
	DirectionalLight* dl = new DirectionalLight();
	dl->direction(Vector(0.2f, -1, 1));
	dl->color(Color(0.25, 0.25, 0.5));
	dl->castShadows(true);
	ShaderLightMapper::instance().addLight(dl);
	
	Color c = Color(1.0f, 0.7f, 1.0f);
	Vector a = Vector(1, 0, 0.1f);
	float innerradius = 45;
	float outerradius = 60;
	
	// point lights
	PointLight* pl = new PointLight();
	pl->position(Vector(-1.5, 3, 10));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(5.0f, 3, 10));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(-1.5, 3, 28));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(5.0f, 3, 28));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(-1.5, 3, -8));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(5.0f, 3, -8));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);
	
	
	// spot lights
	SpotLight* sl = new SpotLight();
	sl->position(Vector(-1.5, 3, 10));
	sl->color(c);
	sl->direction(Vector(1,-4,0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);

	sl = new SpotLight();
	sl->position(Vector(5.0f, 3, 10));
	sl->color(c);
	sl->direction(Vector(-1, -4, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);

	sl = new SpotLight();
	sl->position(Vector(-1.5, 3, 28));
	sl->color(c);
	sl->direction(Vector(1, -4, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);

	sl = new SpotLight();
	sl->position(Vector(5.0f, 3, 28));
	sl->color(c);
	sl->direction(Vector(-1, -4, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	
	sl = new SpotLight();
	sl->position(Vector(-1.5, 3, -8));
	sl->color(c);
	sl->direction(Vector(1, -4, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	
	sl = new SpotLight();
	sl->position(Vector(5.0f, 3, -8));
	sl->color(c);
	sl->direction(Vector(-1, -4, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	
}

void Application::createNormalTestScene()
{
	pModel = new LinePlaneModel(10, 10, 10, 10);
	ConstantShader* pConstShader = new ConstantShader();
	pConstShader->color(Color(0, 0, 0));
	pModel->shader(pConstShader, true);
	// add to render list
	Models.push_back(pModel);


	pModel = new Model(ASSET_DIRECTORY "cube.obj", false);
	pModel->shader(new PhongShader(), true);
	Models.push_back(pModel);


}

void Application::createShadowTestScene()
{
	pModel = new Model(ASSET_DIRECTORY "shadowcube.obj", false);
	pModel->shader(new PhongShader(), true);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "bunny.dae", false);
	pModel->shader(new PhongShader(), true);
	Models.push_back(pModel);
	
	// directional lights
	DirectionalLight* dl = new DirectionalLight();
	dl->direction(Vector(0, -1, -1));
	dl->color(Color(0.5, 0.5, 0.5));
	dl->castShadows(true);
	ShaderLightMapper::instance().addLight(dl);
	
	SpotLight* sl = new SpotLight();
	sl->position(Vector(2, 2, 0));
	sl->color(Color(0.5, 0.5, 0.5));
	sl->direction(Vector(-1, -1, 0));
	sl->innerRadius(10);
	sl->outerRadius(13);
	sl->castShadows(true);
	ShaderLightMapper::instance().addLight(sl);
}

void Application::handleKeyPresses()
{
	if (glfwGetKey(pWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
		this->jumpPressed = true;
	}
	else if (glfwGetKey(pWindow, GLFW_KEY_SPACE) == GLFW_RELEASE) {
		this->jumpPressed = false;
	}
}

void Application::rotatePlayerModel(float dtime, Matrix previousRotation, Model* playerModel)
{
	if (player->getPlayerState() == PlayerStates::airborne || player->getPlayerState() == PlayerStates::continuousJump) {
		//rotate the block furhter!
		Matrix rotation;
		rotation.rotationZ(-dtime * AI_DEG_TO_RAD(180));
		playerModel->transform(playerModel->transform() * previousRotation * rotation);

		//set for next frame
		player->setPreviousRotation(previousRotation * rotation);
	}
	else if (player->getPlayerState() == PlayerStates::grounded) {
		//Matrix rotation;
		//float cosValue = previousRotation.m00;
		//float sineValue = previousRotation.m10;
		//std::cout << "cosValue: " << cosValue << "\t" << "sineValue: " << sineValue << "\n";
		Matrix rotation = calcRotationSnapping(previousRotation.m00, previousRotation.m10);
		playerModel->transform(playerModel->transform() * rotation);
		player->setPreviousRotation(rotation);

	}
}

Matrix Application::calcRotationSnapping(float cosValue, float sineValue)
{
	Matrix rotation;
	//1.1f um rundungsfehler bei überprüfung der gleichheit mit 1.0f zu vermeiden

	if ((1.1f >= cosValue && cosValue > 0.707f) && (0.707f >= sineValue && sineValue > -0.707f)) {
		rotation.rotationZ(AI_DEG_TO_RAD(0));
	}
	else if ((0.707f >= cosValue && cosValue > -0.707f) && (1.1f >= sineValue && sineValue > 0.707f)) {
		rotation.rotationZ(AI_DEG_TO_RAD(90));
	}
	else if ((-0.707f > cosValue && cosValue >= -1.1f) && (0.707f >= sineValue && sineValue > -0.707f)) {
		rotation.rotationZ(AI_DEG_TO_RAD(180));
	}
	else if ((0.707f > cosValue && cosValue >= -0.707f) && (-0.707f > sineValue && sineValue >= -1.1f)) {
		rotation.rotationZ(AI_DEG_TO_RAD(270)); //upright
	}
	else {
		rotation.rotationZ(AI_DEG_TO_RAD(0)); //für testzwecke
	}
	return rotation;
}

////player->update(dtime);
//	//wir brauchen das level als Model damit collision detection gemacht werden kann?
//
//
//Matrix movementDown;
//movementDown.translation(0, -dtime * 5, 0);
//
//Matrix blockTransformBeforeMovement = player->getBlockModel()->transform();
//
////erst downward movement, aka ground collision
//player->getBlockModel()->transform(blockTransformBeforeMovement* movementDown); //* movementSide);
//AABB bbOfPlayer = player->getBlockModel()->boundingBox().transform(player->getBlockModel()->transform());
//
//for (int i{ 0 }; i < lvlObjects.size(); i++)
//{
//	AABB bbOfObject = lvlObjects[i]->boundingBox().transform(lvlObjects[i]->transform());
//	bool collision = AABB::checkCollision(bbOfPlayer, bbOfObject);
//	if (collision) {
//		/*
//		Matrix currentWrongTransform = player->getBlockModel()->transform();
//		//copy constructor!
//
//		Matrix moveUp = Matrix(movementDown);
//		moveUp.invert();
//		player->getBlockModel()->transform(currentWrongTransform * moveUp);*/
//		player->getBlockModel()->transform(blockTransformBeforeMovement);
//	}
//}
//
//
////sideways collision:
//Matrix movementSide;
//movementSide.translation(dtime * 3, 0, 0);
////update the movement, the down movement is already handled
//blockTransformBeforeMovement = player->getBlockModel()->transform();
//player->getBlockModel()->transform(blockTransformBeforeMovement * movementSide);
////update bounding box
//bbOfPlayer = player->getBlockModel()->boundingBox().transform(player->getBlockModel()->transform());
//
//for (int i{ 0 }; i < lvlObjects.size(); i++)
//{
//	AABB bbOfObject = lvlObjects[i]->boundingBox().transform(lvlObjects[i]->transform());
//	bool collision = AABB::checkCollision(bbOfPlayer, bbOfObject);
//	if (collision)
//	{
//		//player->getBlockModel()->transform(blockTransformBeforeMovement);
//		player->respawn();
//	}
//}
//
//for (int i = 0; i < obstacles.size(); i++)
//{
//	AABB bbOfObstacle = obstacles[i]->boundingBox().transform(obstacles[i]->transform());
//	bool collision = AABB::checkCollision(bbOfPlayer, bbOfObstacle);
//	if (collision) {
//		player->respawn();
//	}
//
//}
////update the hitbox visual
//playerHitboxVisual->transform(player->getBlockModel()->transform());
//
////std::cout << "bounding box pos: " << bbOfPlayer.Max.X << ", " << bbOfPlayer.Max.Y << " , " << bbOfPlayer.Max.Z << " min"
////	<< bbOfPlayer.Min.X << ", " << bbOfPlayer.Min.Y << ", " << bbOfPlayer.Min.Z << "\n";
///*
//AABB bbOfPLayerTest = player->getBlockModel()->boundingBox();
//std::cout << "bounding box pos: " << bbOfPLayerTest.Max.X << ", " << bbOfPLayerTest.Max.Y << " , " << bbOfPLayerTest.Max.Z << " min"
//	<< bbOfPLayerTest.Min.X << ", " << bbOfPLayerTest.Min.Y << ", " << bbOfPLayerTest.Min.Z << "\n";*/


/*
		if ((1.0f > cosValue && cosValue >= 0.0f) && (1.0f >= sineValue && sineValue > 0.0f)) {
			//rotationZ(pi/2)
			std::cout << "1" << "\n";
			rotation.rotationZ(AI_DEG_TO_RAD(-90));
		}
		else if ((0.0f > cosValue && cosValue >= -1.0f) && (1.0f > sineValue && sineValue >= 0.0f)) {
			//rotationZ(pi)
			std::cout << "2" << "\n";
			rotation.rotationZ(AI_DEG_TO_RAD(-180));
		}
		else if ((0.0f >= cosValue && cosValue > -1.0f) && (0.0f > sineValue && sineValue >= -1.0f)) {
			//rotationZ(3*pi/2)
			std::cout << "3" << "\n";
			rotation.rotationZ(AI_DEG_TO_RAD(-270));
		}
		else if ((1.0f >= cosValue && cosValue > 0.0f) && (0.0f >= sineValue && sineValue > -1.0f)) {
			rotation.rotationZ(0); //upright
			std::cout << "4" << "\n";

		}
		else {
			std::cout << "5" << "\n";

			rotation.rotationZ(AI_DEG_TO_RAD(0)); //für testzwecke
		}*/


/*
	Matrix rotation;
		float cosValue = previousRotation.m00;
		float sineValue = previousRotation.m10;

		if ((1.0f >= cosValue && cosValue > 0.0f) && (1.0f > sineValue && sineValue >= 0.0f)) {
			//rotationZ(pi/2)
			std::cout << "1" << "\n";
			rotation.rotationZ(AI_DEG_TO_RAD(0));
		}
		else if ((0.0f >= cosValue && cosValue > -1.0f) && (1.0f >= sineValue && sineValue > 0.0f)) {
			//rotationZ(pi)
			std::cout << "2" << "\n";
			rotation.rotationZ(AI_DEG_TO_RAD(90));
		}
		else if ((0.0f > cosValue && cosValue >= -1.0f) && (0.0f >= sineValue && sineValue > -1.0f)) {
			//rotationZ(3*pi/2)
			std::cout << "3" << "\n";
			rotation.rotationZ(AI_DEG_TO_RAD(180));
		}
		else if ((1.0f > cosValue && cosValue >= 0.0f) && (0.0f > sineValue && sineValue >= -1.0f)) {
			rotation.rotationZ(AI_DEG_TO_RAD(270)); //upright
			std::cout << "4" << "\n";

		}
		else {
			std::cout << "5" << "\n";

			rotation.rotationZ(AI_DEG_TO_RAD(0)); //für testzwecke
		}
		std::cout << "cosValue: " << cosValue << "\t" << "sineValue: " << sineValue << "\n";
		playerModel->transform(playerModel->transform() * rotation);
		player->setPreviousRotation(rotation);

*/

/*
//movement down and collision detect
	Matrix movementY;
	movementY.translation(0, -dtime * 5, 0);

	Matrix transformBeforeMoveDown = playerModel->transform(); //in case of collision, reset to this
	playerModel->transform(transformBeforeMoveDown * movementY);

	AABB bbOfPlayer = playerModel->boundingBox().transform(player->getBlockModel()->transform());
	bool collisionHappenedOnce = false;
	for (int i{ 0 }; i < lvlObjects.size(); i++)
	{
		AABB bbOfObject = lvlObjects[i]->boundingBox().transform(lvlObjects[i]->transform());
		bool collision = AABB::checkCollision(bbOfPlayer, bbOfObject);
		if (collision) {
			player->getBlockModel()->transform(transformBeforeMoveDown);
			collisionHappenedOnce = true;
		}
	}
	if (collisionHappenedOnce) {
		player->setPlayerState(PlayerStates::grounded);
	}
	else {
		player->setPlayerState(PlayerStates::falling);

	}


	//sideways movement and collision:
	Matrix movementSide;
	movementSide.translation(dtime * 3, 0, 0);
	playerModel->transform(playerModel->transform() * movementSide);
	//update bounding box
	bbOfPlayer = playerModel->boundingBox().transform(playerModel->transform());

	for (int i{ 0 }; i < lvlObjects.size(); i++)
	{
		AABB bbOfObject = lvlObjects[i]->boundingBox().transform(lvlObjects[i]->transform());
		bool collision = AABB::checkCollision(bbOfPlayer, bbOfObject);
		if (collision)
		{
			player->respawn();
		}
	}

	for (int i = 0; i < obstacles.size(); i++)
	{
		AABB bbOfObstacle = obstacles[i]->boundingBox().transform(obstacles[i]->transform());
		bool collision = AABB::checkCollision(bbOfPlayer, bbOfObstacle);
		if (collision) {
			player->respawn();
		}
	}

	rotatePlayerModel(dtime, previousRotation, playerModel);

	//make camera follow the block
	Vector playerPositionAfter = playerModel->transform().translation();

	Cam.setPosition(Vector(playerPositionAfter.X + 5, playerPositionAfter.Y + 3, playerPositionAfter.Z - 15));
	Cam.setTarget(playerPositionAfter);
	Cam.update();

*/