
out vec3 position;

void main()
{       
	gl_Position = proj * view * model * vec4( position, 1.0 );     //output position with projection
	position = gl_Position;
}
