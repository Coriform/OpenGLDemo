#version 150

out vec4 outColor;
in vec4 Position;

void main()
{
	//outColor = vec4(1,0,0,1);
	//outColor = gl_FragColor.zzzz;

	float z = gl_FragColor.z/1000;

	outColor = vec4(z,z,z,z);
} 