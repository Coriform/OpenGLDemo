#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 EyeDirection;

// Values that stay constant for the whole mesh.
uniform mat4 V;
uniform mat4 M;


void main()
{
	EyeDirection = vec3(0,0,0) - ( V * M * vec4(position,1)).xyz;
	UV = (position.xy + vec2(1,1)) / 2.0;
	gl_Position =  vec4(position, 1);
}






