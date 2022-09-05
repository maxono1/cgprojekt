//
//  LineBoxModel.hpp
//  CGXcode
//
//  Created by Philipp Lensing on 10.10.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef LineCubeModel_hpp
#define LineCubeModel_hpp

#include <stdio.h>
#include "basemodel.h"
#include "vertexbuffer.h"
#include "IndexBuffer.h"

class LineBoxModel : public BaseModel
{
public:
    LineBoxModel(float Width = 1, float Height = 1, float Depth = 1);
    LineBoxModel(Vector max, Vector min);
    virtual ~LineBoxModel() {}
    virtual void draw(const BaseCamera& Cam);
protected:
    VertexBuffer VB;
    IndexBuffer IB;
private:
    void initVB(float start, float width, float height, float depth);
};

#endif /* LineCubeModel_hpp */
