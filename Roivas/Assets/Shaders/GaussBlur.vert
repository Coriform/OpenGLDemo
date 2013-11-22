#version 330 core

layout(location = 0) in vec3 position;

out vec2 Texcoord;

void main()
{
	gl_Position =  vec4(position, 1);
	Texcoord = (position.xy + vec2(1,1)) / 2.0;
}



/*
void main() {
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = ftransform();
}
*/