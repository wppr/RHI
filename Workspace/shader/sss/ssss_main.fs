
#define SSSSTexture2D sampler2D
#define SSSSSampleLevelZero(tex, coord) textureLod(tex, coord, 0.0)
#define SSSSSampleLevelZeroPoint(tex, coord) textureLod(tex, coord, 0.0)
#define SSSSSample(tex, coord) texture(tex, coord)
#define SSSSSamplePoint(tex, coord) texture(tex, coord)
#define SSSSSampleLevelZeroOffset(tex, coord, offset) textureLodOffset(tex, coord, 0.0, offset)
#define SSSSSampleOffset(tex, coord, offset) texture(tex, coord, offset)
#define SSSSSaturate(a) clamp(a, 0.0, 1.0)
#define SSSSMad(a, b, c) (a * b + c)
#define SSSSMul(v, m) (m * v)
#define SSSS_FLATTEN
#define SSSS_BRANCH
#define SSSS_UNROLL
#define float2 vec2
#define float3 vec3
#define float4 vec4
#define int2 ivec2
#define int3 ivec3
#define int4 ivec4
#define float4x4 mat4x4
#define LightNum 5

in vec2 texcoord;
in vec3 o_view;
in vec3 o_normal;
in vec3 o_tangent;
in vec4 worldPosition;


layout(location = 0) out vec4 color;
//layout(location = 1) out vec4 depth;
//layout(location = 2) out vec2 velocity;

struct Light {
	vec3 position;float nearPlane;
	vec3 direction;
	vec3 color;
	float falloffStart;
	float falloffWidth;
	float attenuation;
	float farPlane;
	float bias;
};

// Semantics


vec4 gamma=vec4(2.23,2.23,2.23,1.0);

// No Semantics

layout (std140) uniform ub0{
  Light lights[5]; 
};
//Light lights[LightNum];

uniform sampler2D shadowMaps[LightNum];
uniform mat4 shadowMat[LightNum];

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;
uniform sampler2D specularAOTex;

uniform sampler2D beckmannTex;
uniform samplerCube irradianceTex;
uniform float bumpiness;
uniform float specularIntensity;
uniform float specularRoughness;
uniform float specularFresnel;
uniform float translucency;
uniform int sssEnabled;
uniform int NormalMapEnabled;
uniform float sssWidth;
uniform float ambient;
uniform float n;
uniform float f;
uniform int mode;
#include "shadowMapUse.h"
float3 BumpMap( sampler2D normalTex, float2 texcoord) {
	float3 bump;
	bump.xy = -1.0 + 2.0 * textureLod(normalTex,texcoord,0).gr;
	bump.z = sqrt(1.0 - bump.x * bump.x - bump.y * bump.y);
	return normalize(bump);
}


float Fresnel(vec3 H, vec3 view, float f0) {
	float base = 1.0 - dot(view, H);
	float exponential = pow(base, 5.0);
	return exponential + f0 * (1.0 - exponential);
}



float SpecularKSK(sampler2D beckmannTex, vec3 normal, vec3 light, vec3 view, float roughness) {
	vec3 H = view + light;
	vec3 Hn = normalize(H);

	float ndotl = max(dot(normal, light), 0.0);
	float ndoth = max(dot(normal, Hn), 0.0);
	//adjust beacuse lut read error
	ndoth=clamp(ndoth,2/512.0,510/512.0);
	float ph = pow(2.0 * textureLod(beckmannTex, vec2(ndoth, roughness),0).r, 10.0);
	float f = mix(0.25, Fresnel(Hn, view, 0.028), specularFresnel);
	float ksk = max(ph * f / dot(H, H), 0.0);

	return ndotl * ksk;
}

float Shadow(vec3 worldPosition, int i) {
	vec4 shadowPosition = shadowMat[i]* vec4(worldPosition, 1.0);
	shadowPosition.xy /= shadowPosition.w;
	shadowPosition.z += lights[i].bias;
	float z=shadowPosition.z*2-shadowPosition.w;//range -near ..far
	float z2=(z+n)/(f+n);//range 0..1
	float z1=texture(shadowMaps[i], shadowPosition.xy).r;
	if(z2>z1)
		return 0.0;
	else 
		return 1.0;
}


float3 SSSSTransmittance(
	/**
	* This parameter allows to control the transmittance effect. Its range
	* should be 0..1. Higher values translate to a stronger effect.
	*/
	float translucency,

	/**
	* This parameter should be the same as the 'SSSSBlurPS' one. See below
	* for more details.
	*/
	float sssWidth,

	/**
	* Position in world space.
	*/
	float3 worldPosition,

	/**
	* Normal in world space.
	*/
	float3 worldNormal,

	/**
	* Light vector: lightWorldPosition - worldPosition.
	*/
	float3 light,

	/**
	* Linear 0..1 shadow map.
	*/
	SSSSTexture2D shadowMap,

	/**
	* Regular world to light space matrix.
	*/
	float4x4 lightViewProjection,

	/**
	* Far plane distance used in the light projection matrix.
	*/
	float lightFarPlane) {
	/**
	* Calculate the scale of the effect.
	*/
	float scale = 8.25 * (1.0 - translucency) / sssWidth;

	/**
	* First we shrink the position inwards the surface to avoid artifacts:
	* (Note that this can be done once for all the lights)
	*/
	float4 shrinkedPos = float4(worldPosition - 0.005 * worldNormal, 1.0);

	/**
	* Now we calculate the thickness from the light point of view:
	*/
	float4 shadowPosition = SSSSMul(shrinkedPos, lightViewProjection);
	//vec4 shadowPosition=LightProj[i]*LightView[i]*shrinkedPos;
	float d1 = SSSSSample(shadowMap, shadowPosition.xy / shadowPosition.w).r; // 'd1' has a range of 0..1
	float d2 = shadowPosition.z; // 

	float z=d2*2-shadowPosition.w;//z range -near ..far
	d2=(z+n)/(f+n)*f;//range 0..far
	d1 *= lightFarPlane; // So we scale 'd1' accordingly:

	float d = scale * (abs(d1 - d2));
	/**
	* Armed with the thickness, we can now calculate the color by means of the
	* precalculated transmittance profile.
	* (It can be precomputed into a texture, for maximum performance):
	*/
	float dd = -d * d;

	float3 profile =float3(0.233, 0.455, 0.649) * exp(dd / 0.0064) +
					float3(0.1, 0.336, 0.344) * exp(dd / 0.0484) +
					float3(0.118, 0.198, 0.0)   * exp(dd / 0.187) +
					float3(0.113, 0.007, 0.007) * exp(dd / 0.567) +
					float3(0.358, 0.004, 0.0)   * exp(dd / 1.99) +
					float3(0.078, 0.0, 0.0)   * exp(dd / 7.41);

	/**
	* Using the profile, we finally approximate the transmitted lighting from
	* the back of the object:
	*/

	return profile * SSSSSaturate(0.3 + dot(light, -worldNormal));
}


void main()
{
		
	vec3 normal = normalize(o_normal);
	vec3 tangent = normalize(o_tangent);
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = transpose(mat3(tangent, bitangent, normal));

	// Transform bumped normal to world space, in order to use IBL for ambient lighting:
	vec3 tangentNormal = mix(vec3(0.0, 0.0, 1.0), BumpMap(normalTex, texcoord), bumpiness);
	if(NormalMapEnabled==1)
		normal = SSSSMul(tbn, tangentNormal);
	normal=normalize(normal);
	vec3 view = normalize(o_view);

	// Fetch albedo, specular parameters and static ambient occlusion:
	vec4 albedo = SSSSSample(diffuseTex, texcoord);
	albedo=pow(albedo,gamma);
	vec3 specularAO = SSSSSample(specularAOTex, texcoord).rgb;

	float occlusion = specularAO.b;
	float intensity = specularAO.r * specularIntensity;
	float roughness = (specularAO.g / 0.3) * specularRoughness;

	// Initialize the output:
	color = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 specularColor = vec4(0.0, 0.0, 0.0, 0.0);


	//for (int i = 0; i < LightNum; i++) {
	for (int i = 0; i < 3; i++) {
		float3 light = lights[i].position - worldPosition.xyz;
		float dist = length(light);
		light /= dist;

		float spot = dot(lights[i].direction, -light);
		//if (spot > lights[i].falloffStart) {
			// Calculate attenuation:
			float curve = min(pow(dist / lights[i].farPlane, 6.0), 1.0);
			float attenuation = mix(1.0 / (1.0 + lights[i].attenuation * dist * dist), 0.0, curve);

			// And the spot light falloff:
			spot = SSSSSaturate((spot - lights[i].falloffStart) / lights[i].falloffWidth);

			// Calculate some terms we will use later on:
			vec3 f1 = lights[i].color * attenuation ;//* spot;
			vec3 f2 = albedo.rgb * f1;
			
			// Calculate the diffuse and specular lighting:
			vec3 diffuse = vec3(SSSSSaturate(dot(light, normal)));
			float specular = intensity * SpecularKSK(beckmannTex, normal, light, view, roughness);

			// And also the shadowing:
			//float shadow = Shadow(worldPosition.xyz, i, 3, 1.0);

			float shadow=0;
			if(i<2) 
				shadow= Shadow(worldPosition.xyz, i);
			else
			 	shadow=ShadowVSM(shadowMaps[i],shadowMat[i],worldPosition.xyz,lights[i].position);
			// shadow=1.0;
			// Add the diffuse and specular components:
			color.rgb += shadow*(f2 * diffuse + f1 * specular);
			// Add the transmittance component:
			if (sssEnabled==1)
				color.rgb +=  SSSSTransmittance(translucency, sssWidth, worldPosition.xyz, normalize(o_normal), light, shadowMaps[i], shadowMat[i], lights[i].farPlane);
		//}
	}

	// Add the ambient component:
	color.rgb += occlusion * ambient * albedo.rgb * texture(irradianceTex,normal).rgb;
	//color.rgb=vec3(occlusion);
	// Store the SSS strength:
	color.a = 1.0;

	color=pow(color,1/gamma);


	float w=1280;
	float h=720;
	vec2 fragCoord=gl_FragCoord.xy/vec2(w,h);
	
	int i=0;
	if(mode==1)
		i=0;
	if(mode==2)
		i=1;
	if(mode==3)
		i=2;

	if(mode>0&&mode<=3){
		vec2 z=texture(shadowMaps[i],fragCoord).xy;
		color=vec4(z.x);
	}
	if(mode==4)
	{
		float shadow= Shadow(worldPosition.xyz, 0);
		color=vec4(shadow);
	}
	if(mode==5)
	{
		float shadow= Shadow(worldPosition.xyz, 1);
		color=vec4(shadow);
	}
	if(mode==6)
	{
		float shadow=ShadowVSM(shadowMaps[2],shadowMat[2],worldPosition.xyz,lights[2].position);
		color=vec4(shadow);
	}
	//color*=0.8;
	// float3 light = lights[0].position - worldPosition.xyz;
	// float dist = length(light);
	// float t=pow(dist / lights[0].farPlane, 6.0);
	// color=vec4(lights[0].farPlane/10);
	//float shadow = Sh=adowPCF(worldPosition.xyz, 0, 3, 1.0);
	//shadow=simpleShadow(worldPosition.xyz,0);
	//float shadow =simpleShadow(worldPosition.xyz,1);
	//color =vec4(shadow);
	
	//color=vec4(lights[3].color,1.0);
	//color=vec4(ambient);
	//color=texture(irradianceTex,normal);
	// Store the depth value:
	//depth = vec4(worldPosition.w);

}