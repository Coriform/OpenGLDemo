#version 150

in vec2 position;
in vec2 texcoord;
out vec2 Texcoord;
out vec2 Position;

void main() 
{
	Texcoord = texcoord;
	Position = position;
	gl_Position = vec4( Position, 0, 1.0 );
}