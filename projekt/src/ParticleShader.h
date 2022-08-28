
#include "BaseShader.h"
#include "Color_A.h"
class ParticleShader : public BaseShader
{
public:
	ParticleShader();



private:
	Color_A colorA; //hierdraus vlt ein array machen
	//locations der einzelnen uniforms
	GLint ColorLoc;
	GLint ViewProjLoc;
	GLint ModelMatLoc;
};
