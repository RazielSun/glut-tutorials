
uniform samplerCube cubemapTexture;

varying vec3 uvCoord;

void main()
{
    gl_FragColor = textureCube(cubemapTexture, uvCoord);
}