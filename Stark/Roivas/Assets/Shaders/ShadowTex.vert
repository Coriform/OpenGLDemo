#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 depthMVP;
uniform mat4 M,V;

out vec4 Position;

void main()
{
	gl_Position =  depthMVP * vec4(position,1);
	Position = V * M * vec4(position,1);
}

