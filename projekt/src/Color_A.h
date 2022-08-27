
class Color_A
{
public:
	float R;
	float G;
	float B;
	float A;

	Color_A();
	Color_A(float R, float G, float B, float A);

	Color_A operator*(const Color_A& c) const;
	Color_A operator*(const float Factor) const;
	Color_A operator+(const Color_A& c) const;
	Color_A& operator+=(const Color_A& c);
};
