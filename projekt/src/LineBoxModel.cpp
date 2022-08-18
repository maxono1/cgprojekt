//
//  LineBoxModel.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 10.10.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "LineBoxModel.h"

LineBoxModel::LineBoxModel(float Width, float Height, float Depth)
{
	float start = -Height / 2;
	//initVB(start, Width, Height, Depth);
	
	VB.begin();

	//GROUND LEVEL
	VB.addVertex(start, start, start);
	VB.addVertex(Width + start, start, start);

	VB.addVertex(Width + start, start, start);
	VB.addVertex(Width + start, start, Depth + start);

	VB.addVertex(Width + start, start, Depth + start);
	VB.addVertex(start, start, Depth + start);

	VB.addVertex(start, start, Depth + start);
	VB.addVertex(start, start, start);


	//Vertical lines
	VB.addVertex(start, start, start);
	VB.addVertex(start, Height + start, start);

	VB.addVertex(Width + start, start, start);
	VB.addVertex(Width + start, Height + start, start);

	VB.addVertex(Width + start, start, Depth + start);
	VB.addVertex(Width + start, Height + start, Depth + start);

	VB.addVertex(start, start, Depth + start);
	VB.addVertex(start, Height + start, Depth + start);

	//Top layer
	VB.addVertex(start, Height + start, start);
	VB.addVertex(Width + start, Height + start, start);

	VB.addVertex(Width + start, Height + start, start);
	VB.addVertex(Width + start, Height + start, Depth + start);

	VB.addVertex(Width + start, Height + start, Depth + start);
	VB.addVertex(start, Height + start, Depth + start);

	VB.addVertex(start, Height + start, Depth + start);
	VB.addVertex(start, Height + start, start);
	//VB.addVertex(-Width, Height, Depth);

	VB.end();
}

LineBoxModel::LineBoxModel(Vector max, Vector min)
{
	float width = max.X - min.X;
	float height = max.Y - min.Y;
	float depth = max.Z - min.Z;
	float start = -height / 2;
	initVB(start, width, height, depth);
}

void LineBoxModel::draw(const BaseCamera& Cam)
{
	// TODO: Add your code (Exercise 1)

	BaseModel::draw(Cam);
	VB.activate();

	glDrawArrays(GL_LINES, 0, VB.vertexCount());

	VB.deactivate();
}

void LineBoxModel::initVB(float start, float Width, float Height, float Depth)
{
	VB.begin();

	//GROUND LEVEL
	VB.addVertex(start, 0, start);
	VB.addVertex(Width + start, 0, start);

	VB.addVertex(Width + start, 0, start);
	VB.addVertex(Width + start, 0, Depth + start);

	VB.addVertex(Width + start, 0, Depth + start);
	VB.addVertex(start, 0, Depth + start);

	VB.addVertex(start, 0, Depth + start);
	VB.addVertex(start, 0, start);


	//Vertical lines
	VB.addVertex(start, 0, start);
	VB.addVertex(start, 0, start);

	VB.addVertex(Width + start, 0, start);
	VB.addVertex(Width + start, Height, start);

	VB.addVertex(Width + start, 0, Depth + start);
	VB.addVertex(Width + start, Height, Depth + start);

	VB.addVertex(start, 0, Depth + start);
	VB.addVertex(start, Height, Depth + start);

	//Top layer
	VB.addVertex(start, Height, start);
	VB.addVertex(Width + start, Height, start);

	VB.addVertex(Width + start, Height, start);
	VB.addVertex(Width + start, Height, Depth + start);

	VB.addVertex(Width + start, Height, Depth + start);
	VB.addVertex(start, Height, Depth + start);

	VB.addVertex(start, Height, Depth + start);
	VB.addVertex(start, Height, start);

	VB.end();
}
