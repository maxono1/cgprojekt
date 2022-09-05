
#include "utils.h"
//Quelle: https://stackoverflow.com/questions/4353525/floating-point-linear-interpolation
//verändert weil das falschherum war
float Utils::lerp(float a, float b, float f)
{
	return (b * (1.0f - f)) + (a * f);
}
