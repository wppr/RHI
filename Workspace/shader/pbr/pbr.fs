
#define PI										3.14159265358979323846
#define PI_INV									0.31830988618379067154


in vec2 texcoord;
out vec3 hdr_image_out;

uniform sampler2D			gbuffer_depth;
//uniform sampler2D			gbuffer_normal;
uniform highp usampler2D	gbuffer_normal;
uniform sampler2D			gbuffer_diffuse;
uniform sampler2D			gbuffer_specular;
uniform sampler2D			gbuffer_normal2;

uniform samplerCube		env_map_filtered_sampler;
uniform samplerCube		env_map_sampler;
uniform sampler2D			ao_map_sampler;
uniform highp sampler2D	shadow_map_sampler;
uniform sampler2D			random_direction_3x3_sampler;

uniform sampler2D			lut_sampler;
uniform sampler2D			ggx_sampler;

uniform mat4				proj_inv_mat;
uniform mat4				view_inv_mat;
uniform mat4				view_proj_inv_mat;
uniform mat4				shadow_mat;
uniform mat4				world_view_mat;
uniform mat4				ViewMatrix;
uniform vec3				sun_light_direction;
vec3				sun_light_radiance=vec3(0.5);
uniform vec3 				eyepos;
const int max_sample=128;
uniform int num_samples;
uniform vec3		cosine_hemisphere_sampler[max_sample];


uniform float	metalness;
uniform float	roughness;

vec3 fresnel_schlick(vec3 f0, float VdotH) {
	return f0 + (1.0 - f0) * pow(1.0 - VdotH, 5.0);
}

float ndf_ggx(float NdotH, float alpha) {
	float alpha2 = alpha * alpha;
	float deno = (NdotH * alpha2 - NdotH) * NdotH + 1.0;
	return alpha2 / (deno * deno) * PI_INV;
}

float ndf_ggx_mul_pi(float NdotH, float alpha) {
	float alpha2 = alpha * alpha;
	float deno = (NdotH * alpha2 - NdotH) * NdotH + 1.0;
	return alpha2 / (deno * deno);
}

float visibility_schlick_modified(float NdotL, float NdotV, float alpha) {
	float k = 0.5 * alpha;
	float gl = 1.0 / (NdotL * (1.0 - k) + k);
	float gv = 1.0 / (NdotV * (1.0 - k) + k);
	return 0.25 * gl * gv;
}

vec3 integrate_directional_light(float NdotL, float NdotV, float NdotH, float VdotH, vec3 f0, vec3 albedo, float alpha) {
    if (NdotL > 0.0) {
		vec3 fr = ndf_ggx(NdotH, alpha) * visibility_schlick_modified(NdotL, NdotV, alpha) * fresnel_schlick(f0, VdotH);
		vec3 fd = PI_INV * albedo;
		return sun_light_radiance * (fr + fd) * NdotL;
	}
	return vec3(0.0);
}

vec3 integrate_ambient_diffuse(mat3 local_base) {
	ivec2 env_map_size = textureSize(env_map_filtered_sampler, 0);
	float env_map_area = float(env_map_size.x) * float(env_map_size.y);
	float lod_constant = log2(env_map_area / float(num_samples));

	vec3 acc = vec3(0.0);
	for (int i = 0; i < num_samples; ++i) {
		//vec3 direction = texelFetch(cosine_hemisphere_sampler, i).xyz;
		vec3 direction = cosine_hemisphere_sampler[i].xyz;
		vec3 L = local_base * direction;
		vec3 L_world =mat3(view_inv_mat) * L;
		float lod = 0.5 * max(0.0, lod_constant - log2(0.666666667 * direction.z));
		acc += textureLod(env_map_filtered_sampler, L_world, lod).rgb;
	}
	return acc / float(num_samples);
}

vec3 integrate_ambient_specular(mat3 local_base, float alpha) {
	ivec2 env_map_size = textureSize(env_map_sampler, 0);
	float env_map_area = float(env_map_size.x) * float(env_map_size.y);
	float lod_constant = log2(env_map_area / float(num_samples));

	vec3 acc = vec3(0.0);
	for (int i = 0; i < num_samples; ++i) {
		vec3 direction = texture(ggx_sampler,vec2(float(i/num_samples),alpha)).xyz;		
		vec3 L = local_base * direction;
		vec3 L_world =  mat3(view_inv_mat) *L;
		vec3 N = local_base[2];
		float NdotH = dot(N, normalize(N + L));
		float lod = 0.5 * max(0.0, lod_constant - log2(0.166666667 * ndf_ggx_mul_pi(NdotH, alpha)));
		acc += textureLod(env_map_sampler, L_world, lod).rgb;
		return acc;
	}
	return acc / float(num_samples);

}
vec3 prefilter_diffuse(vec3 N_world){
	return textureLod(env_map_filtered_sampler, N_world, 0.0).rgb;
}
float decode(vec2 rg)
{
   return dot(rg, vec2(1.0, 1.0 / 255.0f));
}

float getRandom2(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 getPosition(vec2 uv){
  float depth = texture(gbuffer_depth, texcoord).x;
  vec3 ndc = 2.0 * vec3(texcoord, depth) - 1.0;
  vec4 P = view_proj_inv_mat * vec4(ndc, 1.0); 
  P.xyz /= P.w;
  return P.xyz;
}


vec3 getNormal3(vec2 uv){
	uvec2 image_packed = texelFetch(gbuffer_normal, ivec2(gl_FragCoord.xy), 0).xy;
	vec4 image_unpacked = vec4(unpackHalf2x16(image_packed.x), unpackHalf2x16(image_packed.y));
	return image_unpacked.xyz;
}
void main() {
	float depth = texture(gbuffer_depth, texcoord).x;

	vec4 gbuffer1 = texture(gbuffer_diffuse, texcoord).rgba;
	vec3 base_color = gbuffer1.rgb;


	vec3 N=getNormal3(texcoord);

	
  	vec3 position_world=getPosition(texcoord);
  	vec4 position_view=ViewMatrix*vec4(position_world,1.0);
  	position_view/=position_view.w;



	if (1.0==depth) {
		hdr_image_out = textureLod(env_map_sampler, position_world.xyz, 0.0).rgb;
		
		return;
	}

  	//return;

	// testing
	float metalness=0.7;
	float roughness=0.1;

	
	vec3 f0 = vec3(0.04);
	vec3 albedo = base_color;
	if (metalness > 0.5) {
		f0 = base_color;
		albedo = vec3(0.0);
	}
	vec3 L=-sun_light_direction;

	vec3 V = normalize(eyepos - position_world);
    vec3 H = normalize(L + V);
    float NdotL = dot(N, L);
	float NdotV = clamp(dot(N, V), 0.0, 1.0);
	float NdotH = clamp(dot(N, H), 0.0, 1.0);
	float VdotH = clamp(dot(V, H), 0.0, 1.0);
	vec2 size=vec2(textureSize(gbuffer_depth, 0));
	vec3 D = texture(random_direction_3x3_sampler, 0.333333333 * texcoord * size).xyz;

	vec3 T = normalize(D - dot(N, D) * N);
	vec3 B = cross(N, T);

	vec3 R = reflect(-V, N);
	vec3 W = R.z == 0.0 ? vec3(D.x, 0.0, D.y) : D;
	vec3 X = normalize(W - dot(R, W) * R);
	vec3 Y = cross(R, X);

	float shadow_term=1.0;
	vec4 shadowcoord = shadow_mat * vec4(position_world.xyz,1.0);
	float depthmap=texture(shadow_map_sampler,shadowcoord.xy/shadowcoord.w).x;
	if(0.001+depthmap<shadowcoord.z/shadowcoord.w)
		shadow_term=0.3;
	//float shadow_term = textureProj(shadow_map_sampler, shadow_map_coord);
	float ao_term = texture(ao_map_sampler, texcoord).x;
	
	vec3 lut = texture(lut_sampler, vec2(NdotV, roughness)).rgb;
	vec3 dfg_term = lut.r * f0 + lut.g;
	float alpha = roughness * roughness;

	//test
	ao_term=1.0;

	vec3 result = vec3(0.0);
	result += integrate_directional_light(NdotL, NdotV, NdotH, VdotH, f0, albedo, alpha)*shadow_term;
	result += albedo * integrate_ambient_diffuse(mat3(T, B, N)) ;
	result += integrate_ambient_specular(mat3(X, Y, R), alpha) * dfg_term * ao_term;
    vec3 q=result;
    float gamma=1.0/2.2;
  	hdr_image_out=vec3(pow(q.r,gamma),pow(q.g,gamma),pow(q.b,gamma));

  //	if(spe>0.24&&spe<0.26)
  //  	hdr_image_out=vec3(1.0);
    //hdr_image_out= albedo;
    //hdr_image_out=N;
    //hdr_image_out=vec3(ao_term);
    //hdr_image_out=mix(base_color,result,0.0);
    //hdr_image_out=base_color;

    //hdr_image_out=position_world.xyz;
    //hdr_image_out=normalize((ViewMatrix*vec4(1.0)).xyz);
   // hdr_image_out=(normalize(position_view)*0.5+0.5).xyz;
    //hdr_image_out=vec3(shadow_term);
   // hdr_image_out=shadowcoord.xyz/shadowcoord.w;
    //hdr_image_out=texture(shadow_map_sampler,texcoord).xyz;
}
