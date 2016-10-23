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
uniform sampler2D shadowMap;

varying vec2 uvCoord;
varying vec3 normal0;
varying vec3 worldPos0;
varying vec4 lightPos0;

vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal, float ShadowFactor)
{
    vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0);

    float DiffuseFactor = dot(Normal, -LightDirection);
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0.0) {
        DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0);
    }

    return (AmbientColor + ShadowFactor * DiffuseColor);
}

vec4 CalcDirectionalLight(vec3 Normal)
{
	return CalcLightInternal(directionLight.Base, directionLight.Direction, Normal, 1.0);
}

float CalcShadowFactor(vec4 LightPos)
{
    vec3 coords = LightPos.xyz / LightPos.w;

    vec2 uv;
    uv.x = 0.5 * coords.x + 0.5;
    uv.y = 0.5 * coords.y + 0.5;

    float z = 0.5 * coords.z + 0.5;
    float Depth = texture2D(shadowMap, uv).x;
    if (Depth < (z + 0.00001)) {
        return 0.5;
    }
    else {
        return 1.0;
    }
}

vec4 CalcPointLight(PointLight l, vec3 Normal, vec4 LightPos)
{
	vec3 LightDirection = worldPos0 - l.Position;
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);
	float ShadowFactor = CalcShadowFactor(LightPos);

	vec4 Color = CalcLightInternal(l.Base, LightDirection, Normal, ShadowFactor);
	float Atn = l.Atten.Constant + l.Atten.Linear * Distance + l.Atten.Exp * Distance * Distance;
	return Color / Atn;
}

vec4 CalcSpotLight(SpotLight l, vec3 Normal, vec4 LightPos)
{
	vec3 LightToPixel = normalize(worldPos0 - l.Base.Position);
	float SpotFactor = dot(LightToPixel, l.Direction);

	if (SpotFactor > l.Cutoff) {
		vec4 Color = CalcPointLight(l.Base, Normal, LightPos);
		return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));
	} else {
		return vec4(0.0, 0.0, 0.0, 0.0);
	}
}

void main()
{
	vec3 Normal = normalize(normal0);
	vec4 TotalLight = CalcDirectionalLight(Normal);

	for (int i = 0; i < numPointLights; i++) {
		TotalLight += CalcPointLight(pointLights[i], Normal, lightPos0);
	}

	for (int i = 0; i < numSpotLights; i++) {
		TotalLight += CalcSpotLight(spotLights[i], Normal, lightPos0);
	}

	gl_FragColor = texture2D(sampler, uvCoord) * TotalLight;
}
