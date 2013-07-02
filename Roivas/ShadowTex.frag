#version 330

in vec2 Texcoord;
uniform sampler2D gShadowMap;

out vec4 outColor;

void main()
{
    float Depth = texture(gShadowMap, Texcoord).x;
    Depth = 1.0 - (1.0 - Depth) * 25.0;
    outColor = vec4(Depth);
} 