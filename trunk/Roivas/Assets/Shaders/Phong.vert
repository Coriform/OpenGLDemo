#version 150

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform mat4 shadow_trans;

in vec3 position;
in vec3 normal;
in vec2 texcoord;

out vec3 Position;
out vec3 Normal;
out vec2 Texcoord;
out vec3 Color;

out vec4 Shadowcoord;

void main()
{
	mat4 normmat = transpose( inverse( model ) );
	
	Position	= ( model * vec4( position, 1.0 ) ).xyz;    //get the position of the vertex after translation, rotation, scaling
    Normal		= ( normmat * vec4( normal, 1.0 ) ).xyz;       //get the normal direction, after translation, rotation, scaling
	Texcoord	= texcoord;

	gl_Position = proj * view * model * vec4( position, 1.0 );     //output position with projection

	//gl_Position = shadow_trans * gl_Position;
	Shadowcoord = shadow_trans * view * model * vec4( position, 1.0 );
}
