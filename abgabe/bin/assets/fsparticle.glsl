#version 400

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

#define PARTICLE_COUNT 1000

in flat int instanceID;

uniform vec4 Color;
uniform vec4 Colors[PARTICLE_COUNT];

void main()
{
	//FragColor = Color;
    FragColor = Colors[instanceID];
}