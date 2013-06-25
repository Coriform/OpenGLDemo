#version 150

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 lightpos;
uniform vec3 eyepos;

uniform vec3 overrideColor;

in vec3 position;
in vec3 normal;
in vec2 texcoord;

out vec3 Position;
out vec3 Normal;
out vec2 Texcoord;
out vec3 Color;
out vec3 EyePos;
out vec3 LightPos;


void main()
{
	mat4 normmat = transpose( inverse( model ) );
	
	Position	= ( model * vec4( position, 1.0 ) ).xyz;    //get the position of the vertex after translation, rotation, scaling
    Normal		= ( normmat * vec4( normal, 1.0 ) ).xyz;       //get the normal direction, after translation, rotation, scaling
	Texcoord	= texcoord;
	LightPos	= lightpos;
	EyePos		= eyepos;

	gl_Position = proj * view * model * vec4( position, 1.0 );     //output position with projection
}
