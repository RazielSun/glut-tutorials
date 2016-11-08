const int MAX_BONES = 100;

attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;
attribute ivec4 BoneIDs;
attribute vec4 Weights;

uniform mat4 WVP;
uniform mat4 world;
uniform mat4 Bones[MAX_BONES];

varying vec2 uvCoord;
varying vec3 normal0;
varying vec3 worldPos0;

void main()
{
	gl_Position = WVP * vec4(position, 1.0);
	uvCoord = texCoord;
	normal0 = (world * vec4(normal, 0.0)).xyz;
	worldPos0 = (world * vec4(position, 1.0)).xyz;
}