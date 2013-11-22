#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;


uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

uniform int lighttype;

void main()
{
	if( lighttype == 0 )
		gl_Position = vec4(position,1);
	else
		gl_Position = P * V * M * vec4(position,1);
}

