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
uniform samplerCube			env_map_filtered;
uniform vec3 				eyepos;

uniform vec3 BaseColor;

vec3 prefilter_irradiance(vec3 N_world){
	return textureLod(env_map_filtered, N_world, 0.0).rgb;
}

vec3 Diffuse_Lambert( vec3 DiffuseColor )
{
	return DiffuseColor * PI_INV;
}

vec3 PrefilterEnvMap(float Roughness,vec3 R){
	return textureLod(env_map,R,Roughness*5).rgb;
}
vec2 IntegrateBRDF(float Roughness,float NoV ){
	return textureLod(IntegrateBRDFSampler,vec2(Roughness,NoV),0).xy;
}
vec3 ApproximateSpecularIBL( vec3 SpecularColor, float Roughness, vec3 N, vec3 V )
{
	float NoV = clamp( dot( N, V ),1e-5,1.0 );
	vec3 R = 2 * dot( V, N ) * N - V;
	vec3 PrefilteredColor = PrefilterEnvMap( Roughness, R );
	vec2 EnvBRDF = IntegrateBRDF( Roughness, NoV );
	return PrefilteredColor*( SpecularColor * EnvBRDF.x + EnvBRDF.y );
}

void main(){

	vec3 N=normalize(Normal);
	vec3 V = normalize(eyepos - Position);

	float metalness=ambient_color.x;
	float roughness=ambient_color.y;

	vec3 base_color=diffuse_color.xyz;
//test
	// metalness=0.3;
	// roughness=0.4;	

	// vec3 texcolor=texture(diffuse_texture,Texcoord).rgb;
	// if(length(texcolor)>0)
	// 	base_color=texcolor;
//
	//base_color=BaseColor;
	vec3 color=mix(base_color,vec3(0),metalness);
	vec3 f0=mix(vec3(0.04),base_color,metalness);


	vec3 diffuse= Diffuse_Lambert(color)*prefilter_irradiance(N);
	vec3 specular=ApproximateSpecularIBL(f0,roughness,N,V);


	image_out=specular+diffuse;
	// image_out=base_color;
	// image_out=texture(diffuse_texture,Texcoord).rgb;
	//image_out=pow(image_out,vec3(1/2.2));
	//image_out=vec3(roughness);
	//image_out=Normal;
	//image_out=fresnel_schlick(vec3(0.04),dot(V,Normal));
	//image_out=SpecularIBL(vec(1.0),)
}