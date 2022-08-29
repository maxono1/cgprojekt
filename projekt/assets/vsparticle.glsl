#version 400

layout(location=0) in vec4 VertexPos;
layout(location=1) in vec4 VertexNormal;

out vec3 Position;
out vec3 Normal;

#define PARTICLE_COUNT 100
//array von mat4
uniform mat4 ModelMats[PARTICLE_COUNT]; //jedes partikel hat seine eigene matrix 
uniform mat4 ViewProjMat;

void main()
{
    Position = (ModelMats[gl_InstanceID] * VertexPos).xyz;
    Normal = (ModelMats[gl_InstanceID] * VertexNormal).xyz;
    gl_Position = ViewProjMat * ModelMats[gl_InstanceID] * VertexPos;
}