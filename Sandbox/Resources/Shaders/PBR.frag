#version 330 core

out vec4 FragColor;

in vec3 vPos;
in vec3 vNor;
in vec2 vTex;

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 albedo;
uniform vec3 emission;
uniform float roughness;
uniform float metallic;

uniform bool useAlbedoMap;
uniform sampler2D albedoMap;

uniform bool useEmissionMap;
uniform sampler2D emissionMap;

uniform bool useNormalMap;
uniform sampler2D normalMap;

uniform bool useRoughnessMap;
uniform sampler2D roughnessMap;

uniform samplerCube irradianceMap;

const float PI = 3.1415926535;

vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, vTex).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(vPos);
    vec3 Q2  = dFdy(vPos);
    vec2 st1 = dFdx(vTex);
    vec2 st2 = dFdy(vTex);

    vec3 N = normalize(vNor);
    vec3 T  = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

// GGX / Trowbridge-Reitz Normal Distribution Function
float D(float alpha, vec3 N, vec3 H)
{
	float a2 = alpha * alpha;

	float numerator = a2;

	float NdotH = max(dot(N, H), 0.0);
	float denominator = PI * pow(NdotH * NdotH * (a2 - 1.0) + 1.0, 2.0);
	denominator = max(denominator, 0.000001);

	return numerator / denominator;
}

// Schlick-Beckmann Geometry Shadowing Function
float G1(float alpha, vec3 N, vec3 X)
{
	float NdotX = max(dot(N, X), 0.0);
	float numerator = NdotX;

	float k = alpha / 2.0;
	float denominator = NdotX * (1.0 - k) + k;
	denominator = max(denominator, 0.000001);

	return numerator / denominator;
}

// Smith Model
float G(float alpha, vec3 N, vec3 V, vec3 L)
{
	return G1(alpha, N, V) * G1(alpha, N, L);
}

// Fresnel-Schlick Function
vec3 F(vec3 F0, vec3 V, vec3 H)
{
	float VdotH = max(dot(V, H), 0.0);
	return F0 + (vec3(1.0) - F0) * pow(1.0 - VdotH, 5.0);
}

void main()
{
	vec3 N;
	if (useNormalMap)
		N = GetNormalFromMap();
	else
		N = normalize(vNor);

	vec3 V = normalize(cameraPosition - vPos);
	// For directional lights
	vec3 L = normalize(lightPosition);

	vec3 H = normalize(V + L);
	
	vec3 F0 = vec3(0.04);
	vec3 fresnel = F(F0, V, H);

	vec3 Ks = fresnel;
	vec3 Kd = vec3(1.0) - Ks;

	float materialRoughness = roughness;
	float materialMetallic = metallic;
	if (useRoughnessMap) {
		materialRoughness += texture(roughnessMap, vTex).g;
		materialMetallic += texture(roughnessMap, vTex).b;
	}
	
    Kd *= 1.0 - materialMetallic;

	vec3 lambert = albedo / PI;
	if (useAlbedoMap)
		lambert *= texture(albedoMap,vTex).rgb;

	vec3 cookTorranceNumerator = D(materialRoughness, N, H) * G(materialRoughness, N, V, L) * fresnel;
	float cookTorranceDenominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	cookTorranceDenominator = max(cookTorranceDenominator, 0.000001);
	vec3 cookTorrance = cookTorranceNumerator / cookTorranceDenominator;

	vec3 BRDF = Kd * lambert + cookTorrance;
	
	vec3 emissionColor = emission;
	if (useEmissionMap)
		emissionColor += texture(emissionMap, vTex).rgb;

	vec3 ambient = vec3(0.03);

	vec3 outgoingLight = ambient + emissionColor + BRDF * lightColor * max(dot(L, N), 0.0);
	FragColor = vec4(outgoingLight, 1.0);
}