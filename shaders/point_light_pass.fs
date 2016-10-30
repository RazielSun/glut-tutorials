
struct BaseLight
{
	vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;
};

struct Attenuation
{
	float Constant;
	float Linear;
	float Exp;
};

struct PointLight
{
	BaseLight Base;
	vec3 Position;
	Attenuation Atten;
};

uniform PointLight pointLight;
uniform sampler2D sampler;
uniform sampler2D posMap;
uniform sampler2D normalMap;
uniform vec2 screenSize;

vec4 CalcLightInternal(BaseLight Light,
					   vec3 LightDirection,
					   vec3 WorldPos,
					   vec3 Normal)
{
    vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0);
    float DiffuseFactor = dot(Normal, -LightDirection);

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0.0) {
        DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0);
    }

    return (AmbientColor + DiffuseColor);
}

vec4 CalcPointLight(vec3 WorldPos, vec3 Normal, PointLight light)
{
    vec3 LightDirection = WorldPos - light.Position;
    float Distance = length(LightDirection);
    LightDirection = normalize(LightDirection);

    vec4 Color = CalcLightInternal(light.Base, LightDirection, WorldPos, Normal);

    float Atn =  light.Atten.Constant +
                 light.Atten.Linear * Distance +
                 light.Atten.Exp * Distance * Distance;

    Atn = max(1.0, Atn);

    return Color / Atn;
}

void main()
{
    vec2 TexCoord = gl_FragCoord.xy / screenSize;
	vec3 WorldPos = texture2D(posMap, TexCoord).xyz;
	vec3 Color = texture2D(sampler, TexCoord).xyz;
	vec3 Normal = texture2D(normalMap, TexCoord).xyz;
	Normal = normalize(Normal);

	gl_FragColor = vec4(Color, 1.0) * CalcPointLight(WorldPos, Normal, pointLight);
}
