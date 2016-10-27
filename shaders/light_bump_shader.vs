attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;
attribute vec3 tangent;

uniform mat4 WVP;
uniform mat4 world;

varying vec2 uvCoord;
varying vec3 normal0;
varying vec3 worldPos0;
varying vec3 tangent0;

void main()
{
	gl_Position = WVP * vec4(position, 1.0);
	uvCoord = texCoord;
	normal0 = (world * vec4(normal, 0.0)).xyz;
	worldPos0 = (world * vec4(position, 1.0)).xyz;
	tangent0 = (world * vec4(tangent, 1.0)).xyz;
}