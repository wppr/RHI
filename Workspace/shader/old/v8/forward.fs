#version 300 es
precision highp float;

in vec3 v_normal;
in vec2 v_texcoord;
in vec3 v_world_pos;
out vec4 fragColor;

uniform vec3 eyepos;
       
uniform vec4 diffuse_color;
uniform sampler2D diffuse_texture;          

uniform vec4 specular_color;
uniform sampler2D specular_texture;

uniform vec4 ambient_color;
uniform sampler2D ambient_texture;





  //material
vec3 Ka;
vec3 Ks;
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

	int numLights=int(lights[0].position.x);
	
  vec4 dcolor = texture(diffuse_texture, v_texcoord).bgra;
  if(dot(dcolor.rgb, vec3(0.2126, 0.7152, 0.0722)) < 0.001) dcolor = diffuse_color;
  vec3 diffuseColor = dcolor.xyz;

  vec3 n=normalize(v_normal);
  vec3 v=normalize(eyepos-v_world_pos);


	vec3 color=Ka*diffuseColor;
	for(int i=1;i<=numLights;i++){
		vec3 lightpos=lights[i].position.xyz;
		vec3 l=normalize(lightpos-v_world_pos);
    color+=adsWithSpotlight(l,lights[i].direction,v,n,lights[i].intensity,diffuseColor,lights[i].exponent,lights[i].cutoff);

	}
	fragColor=vec4(color,1.0);
	
}