#version 150

in vec2 Texcoord;
in vec2 Position;
out vec4 outColor;
uniform sampler2D texFramebuffer;
uniform sampler2D texFont;

void main() 
{
	outColor = texture( texFramebuffer, Texcoord );
	//outColor = vec4( 1-outColor.r, 1-outColor.g, 1-outColor.b, 1);
	//outColor = outColor - vec4(0, 0.2, 0.2, 0);

	//outColor.r = Position.y*10;
}