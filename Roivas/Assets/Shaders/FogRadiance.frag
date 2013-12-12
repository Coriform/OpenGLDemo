#version 330 core

// CLEAN THIS SHADER UP!


layout(location = 0) out vec4 outAttenuatedRadiance;
layout(location = 1) out vec4 outScatteredRadiance;

in vec2 Texcoord;
uniform sampler2D tInRadiance;
uniform sampler2D tDepth;

uniform float sigmaA = 0.2;
uniform float sigmaS = 0.8;

uniform float fog_density = 1.0;

const float e = 2.71828;

float epsilon = 0.001;


void main() 
{
	vec4 color = texture(tInRadiance, Texcoord);
	float depth = texture(tDepth, Texcoord).z;

	if( depth < epsilon )
		depth = 1.0;

	float sigmaT = sigmaA + sigmaS;	

	outAttenuatedRadiance = color * pow(10,-depth*sigmaT);
	outScatteredRadiance = color * pow(10,-depth*sigmaA) * (1 - pow(10,-depth*sigmaS));

	vec4 fog_color = vec4(0.25,0.25,0.25,0);
	float fogAmount = pow(depth, (1.9-fog_density)/1.5);
	outAttenuatedRadiance = mix(outAttenuatedRadiance, fog_color, fogAmount);
}