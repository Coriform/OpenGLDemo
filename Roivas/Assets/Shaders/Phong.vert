#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 Position;
out vec3 Normal;
out vec3 EyeDirection;
out vec3 LightDirection;
out vec4 ShadowCoord;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat4 P;
uniform vec3 LightInvDirection_worldspace;
uniform mat4 DepthBiasMVP;


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;



void main()
{
	//mat4 normmat = transpose( inverse( M ) );
	Normal = ( V * M* vec4(normal,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
	//Normal = ( normmat * vec4(normal,0) ).xyz;

	Position = (M * vec4(position,1)).xyz;
	gl_Position =  P * V * M * vec4(position,1);
	
	ShadowCoord = DepthBiasMVP * vec4(position,1);
	
	
	
	EyeDirection = vec3(0,0,0) - ( V * M * vec4(position,1)).xyz;
	LightDirection = (V*vec4(LightInvDirection_worldspace,0)).xyz;

	
	
	UV = uv;



	/*
	
	mat4 normmat = transpose( inverse( model ) );
	
	Position	= ( model * vec4( position, 1.0 ) ).xyz;    //get the position of the vertex after translation, rotation, scaling
    Normal		= ( normmat * vec4( normal, 1.0 ) ).xyz;       //get the normal direction, after translation, rotation, scaling

	gl_Position = proj * view * model * vec4( position, 1.0 );     //output position with projection

	//gl_Position = shadow_trans * gl_Position;
	ShadowCoord = DepthBiasMVP * vec4(position,1);//shadow_trans * view * model * vec4( position, 1.0 );

	EyeDirection = vec3(0,0,0) - ( V * M * vec4(position,1)).xyz;
	LightDirection = (V*vec4(LightInvDirection_worldspace,0)).xyz;

	UV	= uv;
	*/
}

