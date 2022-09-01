
#include "Model.h"
class SkyboxModel : public Model
{
public:

	void createLerpedTextures(const Texture& tex1, const Texture& tex2);
	void changeToInterpTexture(int index = 0);
	std::vector<Texture*> textures;
};
