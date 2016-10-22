attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;

uniform mat4 WVP;

varying vec2 uvCoord;

void main()                                                                         
{                                                                                   
	gl_Position = WVP * vec4(position, 1.0);                                       
	uvCoord = texCoord;                                                         
}
