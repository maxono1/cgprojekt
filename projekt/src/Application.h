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
#include "ShadowMapGenerator.h"
#include "PlayingCube.h"
#include "LineBoxModel.h"

class Application
{
public:
    typedef std::list<BaseModel*> ModelList;
    Application(GLFWwindow* pWin);
    void start();
    void update(float dtime);
    void draw();
    void end();
protected:
    void createGeometryTestScene();
	void createScene();
	void createNormalTestScene();
	void createShadowTestScene();
    Camera Cam;
    ModelList Models;
    ModelList lvlList;
    std::vector<AABB> aabbList;
    GLFWwindow* pWindow;
	BaseModel* pModel;
    PlayingCube* player;
    LineBoxModel* cubeTest;
	ShadowMapGenerator ShadowGenerator;
};

#endif /* Application_hpp */
