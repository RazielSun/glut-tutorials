
varying vec2 uvCoord;
varying vec3 normal0;
varying vec3 worldPos0;

uniform sampler2D sampler;

void main()									
{											
	gl_FragData[0] = vec4(worldPos0, 1.0);					
	gl_FragData[1] = texture2D(sampler, uvCoord);	
	gl_FragData[2] = vec4(normalize(normal0), 1.0);					
	gl_FragData[3] = vec4(uvCoord, 0.0, 1.0);				
}