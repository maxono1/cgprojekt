
#include "utils.h"

float Utils::lerp(float a, float b, float f)
{
	return (a * (1.0f - f)) + (b * f);
}
