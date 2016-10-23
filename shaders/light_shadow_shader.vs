attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;

uniform mat4 WVP;
uniform mat4 lightWVP;
uniform mat4 world;

varying vec2 uvCoord;
varying vec3 normal0;
varying vec3 worldPos0;
varying vec4 lightPos0;

void main()
{
	gl_Position = WVP * vec4(position, 1.0);
	uvCoord = texCoord;
	normal0 = (world * vec4(normal, 0.0)).xyz;
	worldPos0 = (world * vec4(position, 1.0)).xyz;
	lightPos0 = lightWVP * vec4(position, 1.0);  
}