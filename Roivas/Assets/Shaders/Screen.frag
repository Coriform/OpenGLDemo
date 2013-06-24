#version 150

in vec2 Texcoord;
in vec2 Position;
out vec4 outColor;
uniform sampler2D texFramebuffer;

void main() 
{
	outColor = texture( texFramebuffer, Texcoord );
}