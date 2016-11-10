const int MAX_BONES = 100;

attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;
attribute vec4 BoneIDs;
attribute vec4 Weights;

uniform mat4 WVP;
uniform mat4 world;
uniform mat4 Bones[MAX_BONES];

varying vec2 uvCoord;
varying vec3 normal0;
varying vec3 worldPos0;

void main()
{
	// mat4 BoneTransform = Bones[int(BoneIDs[0])] * Weights[0];
 //    BoneTransform     += Bones[int(BoneIDs[1])] * Weights[1];
 //    BoneTransform     += Bones[int(BoneIDs[2])] * Weights[2];
 //    BoneTransform     += Bones[int(BoneIDs[3])] * Weights[3];

    vec4 PosL    = vec4(position, 1.0);
    gl_Position  = WVP * PosL;

    vec4 NormalL = vec4(normal, 0.0);

	uvCoord = texCoord;
	normal0 = (world * NormalL).xyz;
	worldPos0 = (world * PosL).xyz;
}