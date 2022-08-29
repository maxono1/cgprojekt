//
//  Application.hpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp


#include <stdio.h>
#include <list>
#include "camera.h"
#include "phongshader.h"
#include "constantshader.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "basemodel.h"
#include "ParticleProps.h"
#include "ParticleSystem.h"
#include "ShadowMapGenerator.h"
#include "PlayingCube.h"
#include "LineBoxModel.h"


//constexpr auto epsilon = 1e-6;
//constexpr float gravity = 9.8f;
//constexpr float initialJumpVelocity = 15.0f;

class Application
{
public:
    typedef std::list<BaseModel*> ModelList;
    Application(GLFWwindow* pWin);
    void start();
    void update(float dtime);
    void draw();
    void end();
    bool isJumpPressed();
protected:
    void createGeometryTestScene();
	void createScene();
	void createNormalTestScene();
	void createShadowTestScene();
private:
    void handleKeyPresses();
    void rotatePlayerModel(float dtime, Matrix previousRotation, Model* playerModel);
    Matrix calcRotationSnapping(float cosValue, float sineValue);

protected:
    Camera Cam;
    ModelList Models;
    ModelList lvlList;
    //std::vector<AABB> aabbList;
    std::vector<BaseModel*> obstacles; //everything that kills you no matter how you touch it
    std::vector<LineBoxModel*> obstacleHitboxVisuals;

    std::vector<BaseModel*> lvlObjects; //only die when hit from the side
    std::vector<LineBoxModel*> lvlHitboxVisuals;

    GLFWwindow* pWindow;
	BaseModel* pModel;
    PlayingCube* player;
    LineBoxModel* playerHitboxVisual;
    LineBoxModel* dragonCube;
	ShadowMapGenerator ShadowGenerator;

    PhongShader* phongShader;

    //Particle stuff
    ParticleProps particlePropsTest;
    ParticloSystem* particleSystem;
private:
    bool jumpPressed;
    
};

#endif /* Application_hpp */
