#pragma once
#include <vector>
#include <random>
class Random
{
public:
	static void init()
	{
		s_RandomEngine.seed(std::random_device()());
	}
	//returns random float between 0 and 1
	static float Float()
	{
		return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
	}
public:
	//Mersenne twister pseudo random generator
	static std::mt19937 s_RandomEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;

};