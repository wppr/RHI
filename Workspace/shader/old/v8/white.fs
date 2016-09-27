#version 300 es
precision highp float;
precision highp sampler2D;
in vec2 v_texCoord;
in vec4 vScreenPos;
out vec4 fragColor;

uniform vec3 eyepos;


uniform mat4 InverseViewProject;

uniform sampler2D g_diffuse;	
uniform sampler2D g_normal;
uniform sampler2D g_specular;
uniform sampler2D g_normal2;
uniform sampler2D g_depth;


float decode(vec2 rg)
{
   return dot(rg, vec2(1.0, 1.0 / 255.0f));
}

vec3 decodeLocation()
{
  vec4 clipSpaceLocation;
  clipSpaceLocation.xy = v_texCoord * 2.0f - 1.0f;
  float depth=texture(g_depth, v_texCoord).x;
  //if(1.0-depth<0.000001) discard;
  clipSpaceLocation.z = depth*2.0-1.0;
  clipSpaceLocation.w = 1.0f;
  vec4 homogenousLocation = InverseViewProject * clipSpaceLocation;
  return homogenousLocation.xyz / homogenousLocation.w;
}
vec3 decodenormal(){
	vec4 normalraw=texture(g_normal, v_texCoord);
	vec4 normalraw2=texture(g_normal2, v_texCoord);
   	float nx=decode(normalraw.xy)*2.0-1.0;
   	float ny=decode(normalraw.zw)*2.0-1.0;
   	float nz=decode(normalraw2.xy)*2.0-1.0;
   	vec3 normal=vec3(nx,ny,nz);
   	return normal;
}

//material and light
vec3 Ka;
vec3 Ks;
vec3 Kd;
float Shininess=30.0; // Specular shininess factor

vec3  adsWithSpotlight(vec3 l,vec3 dir,vec3 v,vec3 n,vec3 intensity,float exponent,float cutoff )
{

float angle = acos( dot(-l, dir) );
cutoff=clamp(cutoff,0.0,90.0);
if( angle < cutoff ) {
float spotFactor = pow( dot(-l, dir),exponent );
vec3 h = normalize( v + l );
return spotFactor * intensity * (Kd * max( dot(l, n), 0.0 ) +Ks * pow(max(dot(h,n), 0.0),Shininess));
} 
else {
return vec3(0.0);
}
}


#define MAX_NUM_TOTAL_LIGHTS 20
struct Light {
  vec4 position; // Position in eye coords.
  vec3 intensity; // Amb., Diff., and Specular intensity
  float exponent; // Angular attenuation exponent
  vec3 direction; // Normalized direction of the spotlight
  float cutoff; // Cutoff angle (between 0 and 90)
  mat4 shadowMatrix;
};
layout (std140) uniform Lights_block {	
  Light light;
};
uniform sampler2D ShadowMap[MAX_NUM_TOTAL_LIGHTS];


void main() 
{ 


	Kd=texture(g_diffuse, v_texCoord).xyz;
	vec4 sp=texture(g_specular, v_texCoord);
	Ks=sp.xyz;
	Ka=vec3(sp.w);
	
    vec3  v_world_pos=decodeLocation();

   	vec3 normal=decodenormal();
   	

   	vec3 n=normal;
   	vec3 v=normalize(eyepos-v_world_pos);

 	vec4 d=texture(ShadowMap[0],v_texCoord);

 	float bias=0.001;
 	float shadow=1.0;
 	vec4 ShadowCoord;
 	float depthMap;
	vec3 color=Ka*Kd;

	vec3 lightpos=light.position.xyz;
	vec3 l=normalize(lightpos-v_world_pos);
	int i=0;
	ShadowCoord =(light.shadowMatrix) * vec4(v_world_pos,1.0);
		
	depthMap=texture(ShadowMap[i],ShadowCoord.xy/ShadowCoord.w).x;
		
	if(bias+depthMap<ShadowCoord.z/ShadowCoord.w) shadow=0.1;
    color+=shadow*adsWithSpotlight(l,light.direction,v,n,light.intensity,light.exponent,light.cutoff);

	fragColor=vec4(color,1.0);
	
	//fragColor=vec4(vec3(ShadowCoord.z/ShadowCoord.w),1.0);
	//fragColor=vec4(vec3(1.0-(1.0-d.x)),1.0);
	//fragColor=vec4(vec3(depthMap),1.0);
	//fragColor=vec4(Ks,1.0);
}