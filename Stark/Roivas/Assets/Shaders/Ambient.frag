
#version 330 core

// Ouput data
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outShadow;
layout(location = 2) out vec4 outBloom;

uniform sampler2D tDiffuse;
uniform sampler2D tDepth;

// Interpolated values from the vertex shaders
in vec2 UV;

float epsilon = 0.0001;

void main( void )
{
	vec3 ambient_light = vec3(0.4,0.4,0.4);

	float depth = texture(tDepth, UV).z;
	
	if( depth < epsilon )
		ambient_light = vec3(0.8,0.8,0.8);

	vec4 diffuse = texture( tDiffuse, UV );
	outColor = vec4( clamp( diffuse.xyz * ambient_light, 0.0, 1.0 ), 1);

	outShadow = vec4(0,0,0,0);
	outBloom = vec4(0,0,0,0);
}







