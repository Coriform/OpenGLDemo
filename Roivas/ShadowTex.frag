#version 330

in vec2 Texcoord;
uniform sampler2D gShadowMap;

in vec3 Position;
out vec4 outColor;

void main()
{
    float Depth = texture(gShadowMap, Texcoord).x;
    Depth = 1.0 - (1.0 - Depth) * 25.0;
    //outColor = vec4(Depth);
	outColor = vec4(Position,1);
} 