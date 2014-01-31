#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;


uniform mat4 P, V, M;
out vec3 Texcoord;

void main () 
{
	Texcoord = normalize(position);
	gl_Position = P * V * M * vec4(position, 1.0);
}
