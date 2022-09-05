//
//  LineBoxModel.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 10.10.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//
// Modified by Maximilian Jaesch to use Vectors and Index Buffer

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
	VB.begin();

	//bottom layer
	VB.addVertex(min);
	VB.addVertex(max.X, min.Y, min.Z);
	VB.addVertex(max.X, min.Y, max.Z);
	VB.addVertex(min.X, min.Y, max.Z);

	//top layer
	VB.addVertex(min.X, max.Y, max.Z);
	VB.addVertex(min.X, max.Y, min.Z);
	VB.addVertex(max.X, max.Y, min.Z);
	VB.addVertex(max);

	VB.end();

	IB.begin();

	//bottom lines
	IB.addIndex(0);
	IB.addIndex(1);

	IB.addIndex(1);
	IB.addIndex(2);

	IB.addIndex(2);
	IB.addIndex(3);

	IB.addIndex(3);
	IB.addIndex(0);

	//bottom to top lines
	IB.addIndex(0);
	IB.addIndex(5);

	IB.addIndex(1);
	IB.addIndex(6);

	IB.addIndex(2);
	IB.addIndex(7);

	IB.addIndex(3);
	IB.addIndex(4);

	//top lines

	IB.addIndex(5);
	IB.addIndex(6);

	IB.addIndex(6);
	IB.addIndex(7);

	IB.addIndex(7);
	IB.addIndex(4);

	IB.addIndex(4);
	IB.addIndex(5);

	IB.end();
}

void LineBoxModel::draw(const BaseCamera& Cam)
{
	// TODO: Add your code (Exercise 1)

	BaseModel::draw(Cam);
	VB.activate();
	IB.activate();

	//glDrawArrays(GL_LINES, 0, VB.vertexCount());
	glDrawElements(GL_LINES, IB.indexCount(), IB.indexFormat(), 0);

	IB.deactivate();
	VB.deactivate();
}

