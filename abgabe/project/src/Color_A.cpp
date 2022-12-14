
#include "Color_A.h"

Color_A::Color_A()
{
	this->R = 0.0f;
	this->G = 0.0f;
	this->B = 0.0f;
	this->A = 0.0f;
}

Color_A::Color_A(float R, float G, float B, float A)
{
	//if (R > 1.0f) { this->R = 1.0f; }
	if (R < 0.0f) { this->R = 0.0f; }
	else { this->R = R; }

	//if (G > 1.0f) { this->G = 1.0f; }
	if (G < 0.0f) { this->G = 0.0f; }
	else { this->G = G; }
	
	//if (B > 1.0f) { this->B = 1.0f; }
	if (B < 0.0f) { this->B = 0.0f; }
	else { this->B = B; }

	//if (A > 1.0f) { this->A = 1.0f; }
	if (A < 0.0f) { this->A = 0.0f; }
	else { this->A = A; }
}

Color_A::Color_A(const Color_A& toCopy)
{
	float R = toCopy.R;
	float G = toCopy.G;
	float B = toCopy.B;
	float A = toCopy.A;
	if (R > 1.0f) { this->R = 1.0f; }
	else if (R < 0.0f) { this->R = 0.0f; }
	else { this->R = R; }

	if (G > 1.0f) { this->G = 1.0f; }
	else if (G < 0.0f) { this->G = 0.0f; }
	else { this->G = G; }

	if (B > 1.0f) { this->B = 1.0f; }
	else if (B < 0.0f) { this->B = 0.0f; }
	else { this->B = B; }

	if (A > 1.0f) { this->A = 1.0f; }
	else if (A < 0.0f) { this->A = 0.0f; }
	else { this->A = A; }
}

Color_A Color_A::operator*(const Color_A& c) const
{
	return Color_A(this->R * c.R, this->G * c.G, this->B * c.B, this->A * c.A);
}

Color_A Color_A::operator*(const float Factor) const
{
	return Color_A(this->R * Factor, this->G * Factor, this->B * Factor, this->A * Factor);
}

Color_A Color_A::operator+(const Color_A& c) const
{
	return Color_A(this->R + c.R, this->G + c.G, this->B + c.B, this->A + c.A);
}

Color_A& Color_A::operator+=(const Color_A& c)
{
	this->R += c.R;
	this->G += c.G;
	this->B += c.B;
	this->A += c.A;

	return *this;
}
