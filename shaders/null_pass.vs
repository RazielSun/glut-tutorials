attribute vec3 position;

uniform mat4 WVP;

void main()
{
	gl_Position = WVP * vec4(position, 1.0);
}