#version 330 core

layout(location = 0) out vec4 outColor;

in vec3 Texcoord;
uniform samplerCube tCube;

void main () 
{
	outColor = texture(tCube, Texcoord);
}
