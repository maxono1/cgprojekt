#pragma once
#include "Model.h"

#define NUMBER_OF_SKYBOX_TEXTURES 5

class SkyboxModel : public Model
{
public:
	SkyboxModel(const char* ModelFile, bool FitSize);
	~SkyboxModel();
	void createLerpedTextures(const Texture& tex2);
	void changeToInterpTexture(int index = 0);
	int skyboxChangeProgress;
	std::vector<Texture*> textures;
};
