#define PI										3.14159265358979323846
#define PI_INV									0.31830988618379067154

in vec2 Texcoord;
in vec3 Position;
in vec3 Normal;

out vec3 image_out;

uniform vec4 diffuse_color;
uniform sampler2D diffuse_texture;					

uniform vec4 specular_color;
uniform sampler2D specular_texture;

uniform vec4 ambient_color;
uniform sampler2D ambient_texture;


uniform sampler2D IntegrateBRDFSampler;
uniform samplerCube			env_map;
uniform samplerCube			env_map_diffuse;
uniform vec3 				eyepos;

uniform sampler2D ao_map;
uniform int w;
uniform int h;
uniform float roughness;
uniform float metalness;
uniform vec3 BaseColor;
uniform int mode;

#define LightNum 2
uniform vec3 LightColor[LightNum];
uniform vec3 LightPos[LightNum];

uniform sampler2D shadowMaps[LightNum];
uniform mat4 shadowMat[LightNum];
uniform float bias;

#include "brdf.h"
#include "shadowMapUse.h"
vec3 GetBaseColor(vec4 color,sampler2D tex){
	vec3 base_color=color.xyz*0.5;
	vec3 tex_color=texture(tex,Texcoord).rgb;
	//tex_color=pow(tex_color,vec3(2.2));
	if(color.w==1.0)	
		base_color=tex_color;
	//base_color=pow(base_color,vec3(2.2));
	return base_color;

}
vec3 prefilter_irradiance(vec3 N_world){
	return textureLod(env_map_diffuse, N_world, 0.0).rgb;
}

void main(){


	 vec3 BaseColor=GetBaseColor(diffuse_color,diffuse_texture);
	 vec3 DiffuseColor=mix(BaseColor,vec3(0),metalness);
	 vec3 SpecularColor=mix(vec3(0.01),BaseColor,metalness);


	 vec3 N=normalize(Normal);
	 vec3 V = normalize(eyepos - Position);
	 

	 vec3 direct=vec3(0);
	 vec2 fragCoord=gl_FragCoord.xy/vec2(w,h);
	 float ao=texture(ao_map,fragCoord).x;
	// ao=pow(ao,2);
	 vec3 L =LightPos[0];
	 vec3 direct0=StandardShading(DiffuseColor,SpecularColor, roughness, LightColor[0], L, V, N);

	 L =LightPos[1];
	 vec3 direct1=StandardShading(DiffuseColor,SpecularColor, roughness, LightColor[1], L, V, N);


	vec3 env=EnvShading(DiffuseColor,SpecularColor, roughness, env_map,env_map_diffuse,IntegrateBRDFSampler, V, N);
	//float shadow=Shadow(shadowMaps[0],shadowMat[0],Position,bias);
	float shadow=ShadowVSM(shadowMaps[0],shadowMat[0],Position,LightPos[0]);
	//shadow=1.0;
	if(mode==0)
		//image_out=(direct0*shadow+direct1)*ao;
		image_out=direct0*shadow+direct1+env;
	if(mode==1)
		image_out=(direct0*shadow+direct1+env)*ao;
	if(mode==2)
		image_out=vec3(ao);
	if(mode==3)
		image_out=env;
	if(mode==4)
		image_out=direct0;
	if(mode==5)
		image_out=N*0.5+0.5;
	if(mode==6){
		vec2 z=texture(shadowMaps[0],fragCoord).xy;
		image_out=vec3(z.x/12);
	}
	if(mode==7){
		image_out=vec3(shadow);
	}
	if(mode==8){
		image_out=shadow*direct0;
	}

	//image_out=env;
	 //image_out=a(diffuse_texture,vec2(0,0));
	 //image_out=b(env_map,N);
	

	//image_out=SimpleShading();
	// image_out=GetBaseColor(diffuse_color,diffuse_texture);
	// image_out=texture(diffuse_texture,Texcoord).rgb;
	//image_out=pow(image_out,vec3(1/2.2));
	//image_out=vec3(roughness);
	//image_out=Normal;
	//image_out=fresnel_schlick(vec3(0.04),dot(V,Normal));
	//image_out=SpecularIBL(vec(1.0),)
}



// vec3 SimpleDiffuse(vec3 lightPos,vec3 lightColor,vec3 N){
//     vec3 L = normalize(lightPos - Position);
//     float diff = max(dot(N, L), 0.0);
//     vec3 diffuse = diff*lightColor;
//     return diffuse;
// }
// vec3 SimpleShading(){
// 	vec3 color_d=GetBaseColor(diffuse_color,diffuse_texture);
// 	vec3 color_a=GetBaseColor(ambient_color,ambient_texture);
 	
// 	    // Ambient
//     float ambientStrength = 0.1f;
//     vec3 ambient = ambientStrength * color_a;
//     vec3 N=normalize(Normal);
//     ambient=Diffuse_Lambert(color_a)*prefilter_irradiance(N);
//     // Diffuse 

//     vec3 diffuse = color_d*(SimpleDiffuse(lightPos,lightColor,N)+SimpleDiffuse(lightPos2,lightColor2,N));

//     vec3 result = ao*PI_INV*(ambient + diffuse);

// 	return result;
// }
