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
	Position = position;
	Normal = (view * model * vec4(normal, 0.0)).xyz;
	Texcoord = texcoord;
	Color = overrideColor;
	
	vec4 V = proj * view * model * vec4( position, 1.0 );
	EyePos = V.xyz;
	gl_Position = V;

	LightPos = lightpos;
	
}



/*
varying vec3 normalVec, lightVec, eyeVec, tanVec;
attribute vec3 vertexTangent;

void main(void)  
{     
	gl_Position = ftransform(); 
	gl_TexCoord[0] = vec4(gl_MultiTexCoord0.s*24, gl_MultiTexCoord0.t*16, 1, 1);

	vec4 V = gl_ModelViewMatrix * gl_Vertex;

	// Calculate normalVec, lightVec, eyeVec here
	normalVec = gl_NormalMatrix * gl_Normal;
	lightVec = gl_LightSource[0].position.xyz - V.xyz; 	
	eyeVec = vec3(0,0,0)-V.xyz;   

	tanVec = gl_NormalMatrix * vertexTangent;

	

}
*/