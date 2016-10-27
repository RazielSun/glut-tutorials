const int MAX_LIGHTS = 2;

struct BaseLight
{
	vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;
};

struct DirectionalLight
{
	BaseLight Base;
	vec3 Direction;
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

struct SpotLight
{
	PointLight Base;
	vec3 Direction;
	float Cutoff;
};

uniform DirectionalLight directionLight;
uniform PointLight pointLights[MAX_LIGHTS];
uniform SpotLight spotLights[MAX_LIGHTS];
uniform int numPointLights;
uniform int numSpotLights;
uniform sampler2D sampler;
uniform sampler2D normalMap;

varying vec2 uvCoord;
varying vec3 normal0;
varying vec3 worldPos0;
varying vec3 tangent0;

vec3 CalcBumpedNormal()
{
    vec3 Normal = normalize(normal0);

    vec3 Tangent = normalize(tangent0);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);

    vec3 Bitangent = cross(Tangent, Normal);

    vec3 BumpMapNormal = texture2D(normalMap, uvCoord).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);

    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}

vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal)
{
    vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0);
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);

    float DiffuseFactor = dot(Normal, -LightDirection);
    if (DiffuseFactor > 0.0) {
        DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0);
    }
    
    return (AmbientColor + DiffuseColor);
}

vec4 CalcDirectionalLight(vec3 Normal)
{
	return CalcLightInternal(directionLight.Base, directionLight.Direction, Normal);
}

vec4 CalcPointLight(PointLight l, vec3 Normal)
{
	vec3 LightDirection = worldPos0 - l.Position;
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);
	vec4 Color = CalcLightInternal(l.Base, LightDirection, Normal);
	float Atn = l.Atten.Constant + l.Atten.Linear * Distance + l.Atten.Exp * Distance * Distance;
	return Color / Atn;
}

vec4 CalcSpotLight(SpotLight l, vec3 Normal)
{
	vec3 LightToPixel = normalize(worldPos0 - l.Base.Position);
	float SpotFactor = dot(LightToPixel, l.Direction);

	if (SpotFactor > l.Cutoff) {
		vec4 Color = CalcPointLight(l.Base, Normal);
		return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));
	} else {
		return vec4(0.0, 0.0, 0.0, 0.0);
	}
}

void main()
{
	vec3 Normal = CalcBumpedNormal();
	vec4 TotalLight = CalcDirectionalLight(Normal);

	for (int i = 0; i < numPointLights; i++) {
		TotalLight += CalcPointLight(pointLights[i], Normal);
	}

	for (int i = 0; i < numSpotLights; i++) {
		TotalLight += CalcSpotLight(spotLights[i], Normal);
	}

	gl_FragColor = texture2D(sampler, uvCoord) * TotalLight;
}
