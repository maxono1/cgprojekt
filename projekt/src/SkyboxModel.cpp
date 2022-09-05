#pragma once
#include "SkyboxModel.h"
#include "rgbimage.h"
#include "utils.h"
#include <FreeImage.h>

SkyboxModel::SkyboxModel(const char* ModelFile, bool FitSize) : Model(ModelFile, FitSize), skyboxChangeProgress(0)
{
}

SkyboxModel::~SkyboxModel()
{
	textures.clear();
}

//funktioniert nicht, verbraucht zu viel speicher
void SkyboxModel::createLerpedTextures(const Texture& tex2)
{
	const Texture* tex1 = this->pMaterials[1].DiffTex;
	const RGBImage* rgbImage1 = tex1->getRGBImage();
	
	const RGBImage* rgbImage2 = tex2.getRGBImage();

	for (size_t i = 0; i < NUMBER_OF_SKYBOX_TEXTURES; i++)
	{
		RGBImage* neuesRgbimage = new RGBImage(rgbImage1->width(), rgbImage2->height());
		float fractionForLerp = i / NUMBER_OF_SKYBOX_TEXTURES;
		for (size_t x = 0; x < rgbImage1->width(); x++)
		{
			for (size_t y = 0; y < rgbImage1->height(); y++)
			{
				float R = Utils::lerp(rgbImage1->getPixelColor(x,y).R, rgbImage2->getPixelColor(x, y).R, fractionForLerp);
				float G = Utils::lerp(rgbImage1->getPixelColor(x, y).G, rgbImage2->getPixelColor(x, y).G, fractionForLerp);
				float B = Utils::lerp(rgbImage1->getPixelColor(x, y).B, rgbImage2->getPixelColor(x, y).B, fractionForLerp);
				Color color = Color(R,G,B);
				neuesRgbimage->setPixelColor(x, y, color);
			}

		}
		textures.push_back(new Texture(*neuesRgbimage));
	}

	//hier zwischen rgbimage von den textures interpolieren
}

void SkyboxModel::changeToInterpTexture(int index)
{
	//0 bis 60 im finalen
	//0 bis 3 zum testen
}
