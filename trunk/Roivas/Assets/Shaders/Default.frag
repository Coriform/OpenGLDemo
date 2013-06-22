#version 150

in vec3 Position;
in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D tex;


void main()
{
	vec4 colTex = texture( tex, Texcoord );
	vec4 texColor = colTex;
	outColor = vec4( Color, 1.0 ) * texColor;
}