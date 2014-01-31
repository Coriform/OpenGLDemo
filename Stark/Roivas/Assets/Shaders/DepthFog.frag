#version 330 core

// CLEAN THIS SHADER UP!


layout(location = 0) out vec4 outColor;

in vec2 Texcoord;
uniform sampler2D tInRadiance;
uniform sampler2D tDepth;

const vec4 fog_color = vec4(0,0.5,1,1);
const float fog_density = 0.8;

float epsilon = 0.001;

void main() 
{
	vec4 color = texture(tInRadiance, Texcoord);
	float depth = texture(tDepth, Texcoord).z;

	if( depth < epsilon )
		depth = 1.0;

	float fogAmount = pow(depth, fog_density);

	outColor = mix(color, fog_color, fogAmount);
}