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

	//vlt hier das mit dem bloom frame buffer hin

}



void Application::update(float dtime)
{
	//um teleportieren beim debugging zu vermeiden! dort sind die dtime 4+ sekunden
	if (dtime > 0.01f) {
		dtime = 0.01f;
	}
	//std::cout << dtime << "dtime" << "\n";
	handleKeyPresses();

	if (player->getPlayerState() != PlayerStates::dead && player->getPlayerState() != PlayerStates::finish) {
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
		// Quellen :
		// https://www.youtube.com/watch?v=h2r3_KjChf4
		// https://www.youtube.com/watch?v=hG9SzQxaCm8
		Matrix movementY;
		float completeVelocity = player->getCurrentVelocityY() * dtime + (player->getGravity() * dtime * dtime) / 2;
		//std::cout << completeVelocity << "completeVel" << "\n";
		movementY.translation(0, completeVelocity, 0);

		//falls player fällt
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
				//std::cout << newVelocity << "newVel" << "\n";
				player->setCurrentVelocityY(newVelocity);
			}
			else {
				player->setPlayerState(PlayerStates::airborne);
				player->applyGravityWhileFalling(dtime);
			}
		} //falls player sich nach oben bewegt
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
					//player->respawn(); //respawn setted falling anyway
					//player->setPlayerState(PlayerStates::dead);
					killPlayer();
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
				//player->respawn();
				killPlayer();


			}
		}

		for (int i = 0; i < obstacles.size(); i++)
		{
			AABB bbOfObstacle = obstacles[i]->boundingBox().transform(obstacles[i]->transform());
			bool collision = AABB::checkCollision(bbOfPlayer, bbOfObstacle);
			if (collision) {
				//player->respawn();
				killPlayer();
			}
		}
		AABB bbOfFinish = finishLine->boundingBox().transform(finishLine->transform());
		bool collision = AABB::checkCollision(bbOfPlayer, bbOfFinish);
		if (collision) {
			//player->respawn();
			reachFinish();
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
	}
	else if(player->getPlayerState() == PlayerStates::dead){
		player->setDeathTimer(player->getDeathTimer() + dtime);
		if (player->getDeathTimer() > 1.0f) {
			player->setDeathTimer(0.0f);
			player->respawn();
		}
	}
	else if (player->getPlayerState() == PlayerStates::finish) {
		player->setDeathTimer(player->getDeathTimer() + dtime);
		if (player->getDeathTimer() > 1.0f) {
			player->setDeathTimer(0.0f);
			glfwSetWindowShouldClose(pWindow, GL_TRUE);
		}
	}

	
	

	particleSystem->update(dtime);

	Vector playerPositionAfter = player->getBlockModel()->transform().translation();
	//make camera follow the block
	Cam.setPosition(Vector(playerPositionAfter.X - 1, playerPositionAfter.Y + 3, playerPositionAfter.Z - 10 ));
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
	if (player->getPlayerState() != PlayerStates::dead) {
		player->getBlockModel()->draw(Cam);
	}
	skyboxModel->draw(Cam);
	finishLine->draw(Cam);
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

	for (int i{ 0 }; i < 30; i++) {
		pModel = new Model(ASSET_DIRECTORY "woodenObje.obj", false);
		pModel->shader(phongShader, false); 
		translation.translation(0, 1000, 0);
		pModel->transform(translation);
		lvlObjects.push_back(pModel);
		Models.push_back(pModel);
	}
	//lvl objects [0] wird nicht verwendet

	translation.translation(-8.0f, -4, 0);
	lvlObjects[1]->transform(translation);

	

	translation.translation(-5.0f, -5.5f, 0);
	lvlObjects[2]->transform(translation);

	translation.translation(1.0f, -5.5f, 0);
	lvlObjects[3]->transform(translation);
	
	translation.translation(7.0f, -5.1f, 0); // hinter dem spike
	scale.scale(Vector(1, 2, 1));
	lvlObjects[4]->transform(translation * scale);

	//translation.translation(18.0f, -10.0f, 0);
	//lvlObjects[5]->transform(translation );

	translation.translation(14.0f, 1, 0);
	lvlObjects[14]->transform(translation);

	translation.translation(16.0f, -3.5f, 0); //oberer weg
	lvlObjects[6]->transform(translation);

	translation.translation(25.0f, -5.5f, 0);
	lvlObjects[7]->transform(translation);

	//translation.translation(31.0f, -5.5f, 0);
	//lvlObjects[8]->transform(translation);

	translation.translation(34.0f, -5.5f, 0);
	rotation.rotationX(AI_DEG_TO_RAD(90));
	lvlObjects[9]->transform(translation * rotation);


	translation.translation(40.0f, -5.5f, 0);
	rotation.rotationZ(AI_DEG_TO_RAD(90));
	lvlObjects[11]->transform(translation * rotation);

	translation.translation(43.0f, -3.5f, 0);
	rotation.rotationZ(AI_DEG_TO_RAD(90));
	lvlObjects[12]->transform(translation * rotation);

	translation.translation(46.0f, -1.5f, 0);
	rotation.rotationZ(AI_DEG_TO_RAD(90));
	lvlObjects[0]->transform(translation * rotation);

	translation.translation(49.0f, 0.4f, 0);
	rotation.rotationZ(AI_DEG_TO_RAD(90));
	lvlObjects[5]->transform(translation * rotation);

	translation.translation(52.0f, 1.5f, 0);
	rotation.rotationZ(AI_DEG_TO_RAD(90));
	lvlObjects[10]->transform(translation * rotation);

	translation.translation(55, 4.0f, 0);
	lvlObjects[8]->transform(translation);

	translation.translation(61, 4.0f, 0);
	lvlObjects[13]->transform(translation);




	translation.translation(75, 1.0f, 0);
	scale.scale(3, 1, 3);
	finishLine = new Model(ASSET_DIRECTORY "finishBOX.obj", false);
	finishLine->shader(phongShader, false);
	finishLine->transform(translation * scale);
	

	for (int i{ 0 }; i < lvlObjects.size(); i++) 
	{
		LineBoxModel* lvlHitbox = new LineBoxModel(lvlObjects[i]->boundingBox().Max, lvlObjects[i]->boundingBox().Min);
		//das delete on destruction könnte fehler aufrufen
		lvlHitbox->shader(pConstShader, true);
		lvlHitbox->transform(lvlObjects[i]->transform());
		//Models.push_back(lvlHitbox);
		lvlHitboxVisuals.push_back(lvlHitbox);
	}


	for (size_t i = 0; i < 8; i++)
	{
		pModel = new Model(ASSET_DIRECTORY "tripleSpikes.obj", false);
		pModel->shader(phongShader, true);
		translation.translation(0, 1000, 0);
		pModel->transform(translation);
		obstacles.push_back(pModel);
		Models.push_back(pModel);
	}
	
	translation.translation(3.0f, -5.5f, 0);
	scale.scale(1);
	obstacles[0]->transform(translation * scale);

	translation.translation(63, 4.0f, 0);
	scale.scale(0.8f);
	obstacles[1]->transform(translation);

	pModel = new Model(ASSET_DIRECTORY "deathplane.obj", false);
	pModel->shader(phongShader, true);
	translation.translation(0, -20, 0);
	pModel->transform(translation);
	obstacles.push_back(pModel); //dont need to put it in model for the collision to work
	Models.push_back(pModel);


	/*
	for (int i{ 0 }; i < obstacles.size(); i++) 
	{
		LineBoxModel* obstacleHitbox = new LineBoxModel(obstacles[i]->boundingBox().Max, obstacles[i]->boundingBox().Min);
		obstacleHitbox->shader(pConstShader, true);
		obstacleHitbox->transform(obstacles[i]->transform());
		//Models.push_back(obstacleHitbox);
		obstacleHitboxVisuals.push_back(obstacleHitbox);
	}*/

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
	//Models.push_back(player->getBlockModel());

	
	playerHitboxVisual = new LineBoxModel(player->getBlockModel()->boundingBox().Max, player->getBlockModel()->boundingBox().Min);
	playerHitboxVisual->shader(pConstShader, true);
	//Models.push_back(playerHitboxVisual);



	/*
	pModel = new Model(ASSET_DIRECTORY "skybox_bright.obj", false);
	pModel->shader(new PhongShader(), true);
	pModel->shadowCaster(false);
	Models.push_back(pModel);
	*/ 
	skyboxModel = new SkyboxModel(ASSET_DIRECTORY "skybox_bright.obj", false);
	skyboxModel->shader(new PhongShader(), true);
	skyboxModel->shadowCaster(false);
	//skyboxModel->createLerpedTextures(Texture(ASSET_DIRECTORY "skyBoxNightSky_resize.png"));


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

void Application::initBloomFramebuffer()
{
	//Quelle:https://learnopengl.com/Advanced-Lighting/Bloom
	// set up floating point framebuffer to render scene to
	/*
	unsigned int hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	unsigned int colorBuffers[2];
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0
		);
	}
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);


	bool horizontal = true, first_iteration = true;
	int amount = 10;
	shaderBlur.use();
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		shaderBlur.setInt("horizontal", horizontal);
		glBindTexture(
			GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongBuffers[!horizontal]
		);
		RenderQuad();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	*/
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
	if (player->getPlayerState() == PlayerStates::airborne  || player->getPlayerState() == PlayerStates::continuousJump) {
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

void Application::killPlayer()
{
	player->setPlayerState(PlayerStates::dead);
	ParticleProps deathAnimParticles = ParticleProps();
	deathAnimParticles.position = player->getBlockModel()->transform().translation();
	deathAnimParticles.colorBegin = Color_A(1, 1.0f, 0, 1);
	deathAnimParticles.colorEnd = Color_A(1, 0, 0, 0.2f);
	deathAnimParticles.lifeTime = 0.7f;

	float speed = 3;

	for (size_t i = 0; i < 20; i++)
	{
		deathAnimParticles.velocity = Vector(0, speed, 0); //top
		particleSystem->emit(deathAnimParticles);
		deathAnimParticles.velocity = Vector(0, speed, speed); //top side
		particleSystem->emit(deathAnimParticles);
		deathAnimParticles.velocity = Vector(0, speed, -speed); //top side
		particleSystem->emit(deathAnimParticles);
		deathAnimParticles.velocity = Vector(0, 0, speed);  //side
		particleSystem->emit(deathAnimParticles);
		deathAnimParticles.velocity = Vector(0, 0, -speed); //side
		particleSystem->emit(deathAnimParticles);
		deathAnimParticles.velocity = Vector(speed, speed, 0); //diag forward
		particleSystem->emit(deathAnimParticles);
		deathAnimParticles.velocity = Vector(speed, speed, -speed); //diag fwd side
		particleSystem->emit(deathAnimParticles);
		deathAnimParticles.velocity = Vector(speed, speed, speed); //diag fwd side
		particleSystem->emit(deathAnimParticles);
		deathAnimParticles.velocity = Vector(-speed, speed, 0); //diag back 
		particleSystem->emit(deathAnimParticles);
		deathAnimParticles.velocity = Vector(-speed, speed, -speed); //diag bside
		particleSystem->emit(deathAnimParticles);
		deathAnimParticles.velocity = Vector(-speed, speed, speed); //diag bside
		particleSystem->emit(deathAnimParticles);
		deathAnimParticles.velocity = Vector(-speed, 0, -speed); //back side
		particleSystem->emit(deathAnimParticles);
		deathAnimParticles.velocity = Vector(-speed, 0, 0);//back
		particleSystem->emit(deathAnimParticles);
		deathAnimParticles.velocity = Vector(-speed, 0, speed);//back side
		particleSystem->emit(deathAnimParticles);
		deathAnimParticles.velocity = Vector(speed, 0, -speed); //front side
		particleSystem->emit(deathAnimParticles);
		deathAnimParticles.velocity = Vector(speed, 0, 0);//front
		particleSystem->emit(deathAnimParticles);
		deathAnimParticles.velocity = Vector(speed, 0, speed);//front side
		particleSystem->emit(deathAnimParticles);
	}
	
}

void Application::reachFinish()
{
	player->setPlayerState(PlayerStates::finish);
	ParticleProps finishParticles = ParticleProps();
	finishParticles.position = player->getBlockModel()->transform().translation();
	finishParticles.colorBegin = Color_A(0, 1.0f, 0, 1);
	finishParticles.colorEnd = Color_A(0, 0, 1, 0.2f);
	finishParticles.lifeTime = 0.7f;

	float speed = 3;

	for (size_t i = 0; i < 20; i++)
	{
		finishParticles.velocity = Vector(0, speed, 0); //top
		particleSystem->emit(finishParticles);
		finishParticles.velocity = Vector(0, speed, speed); //top side
		particleSystem->emit(finishParticles);
		finishParticles.velocity = Vector(0, speed, -speed); //top side
		particleSystem->emit(finishParticles);
		finishParticles.velocity = Vector(0, 0, speed);  //side
		particleSystem->emit(finishParticles);
		finishParticles.velocity = Vector(0, 0, -speed); //side
		particleSystem->emit(finishParticles);
		finishParticles.velocity = Vector(speed, speed, 0); //diag forward
		particleSystem->emit(finishParticles);
		finishParticles.velocity = Vector(speed, speed, -speed); //diag fwd side
		particleSystem->emit(finishParticles);
		finishParticles.velocity = Vector(speed, speed, speed); //diag fwd side
		particleSystem->emit(finishParticles);
		finishParticles.velocity = Vector(-speed, speed, 0); //diag back 
		particleSystem->emit(finishParticles);
		finishParticles.velocity = Vector(-speed, speed, -speed); //diag bside
		particleSystem->emit(finishParticles);
		finishParticles.velocity = Vector(-speed, speed, speed); //diag bside
		particleSystem->emit(finishParticles);
		finishParticles.velocity = Vector(-speed, 0, -speed); //back side
		particleSystem->emit(finishParticles);
		finishParticles.velocity = Vector(-speed, 0, 0);//back
		particleSystem->emit(finishParticles);
		finishParticles.velocity = Vector(-speed, 0, speed);//back side
		particleSystem->emit(finishParticles);
		finishParticles.velocity = Vector(speed, 0, -speed); //front side
		particleSystem->emit(finishParticles);
		finishParticles.velocity = Vector(speed, 0, 0);//front
		particleSystem->emit(finishParticles);
		finishParticles.velocity = Vector(speed, 0, speed);//front side
		particleSystem->emit(finishParticles);
	}
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