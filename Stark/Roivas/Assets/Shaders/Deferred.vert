#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec2 UV;
out vec4 Position;
out vec3 EyeDirection;
out vec3 Normal;
out float Depth;


void main( void )
{
	//Position = (M * vec4(position,1)).xyz;	
	Normal = ( V * M * vec4(normal,0)).xyz;
	EyeDirection = vec3(0,0,0) - ( V * M * vec4(position,1)).xyz;
	UV = uv;
	Depth = -(V * M * vec4(position,1)).z;

	gl_Position =  P * V * M * vec4(position,1);
	Position = gl_Position;
}
