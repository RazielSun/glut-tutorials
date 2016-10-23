attribute vec3 position;

uniform mat4 WVP;

varying vec3 uvCoord;

void main()
{
    vec4 WVP_Pos = WVP * vec4(position, 1.0);
    gl_Position = WVP_Pos.xyww;
    uvCoord = position;
}