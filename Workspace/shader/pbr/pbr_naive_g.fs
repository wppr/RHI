#define PI										3.14159265358979323846
#define PI_INV									0.31830988618379067154

in vec2 texcoord;


out vec3 image_out;

uniform sampler2D			gbuffer_depth;
//uniform sampler2D			gbuffer_normal;
uniform highp usampler2D	gbuffer_normal;
uniform sampler2D			gbuffer_diffuse;
uniform sampler2D			gbuffer_specular;
uniform sampler2D			gbuffer_normal2;
uniform samplerCube			env_map;
uniform samplerCube			env_map_filtered;
uniform mat4				view_proj_inv_mat;
uniform mat4				proj_inv_mat;
uniform vec3 				eyepos;
 float radicalInverse_VdC(uint bits) {
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
 }

 vec2 Hammersley(uint i, uint N) {
     return vec2(float(i)/float(N), radicalInverse_VdC(i));
 }
float G_Smith(float roughness, float ndotv,float ndotl)
{
	// = G_Schlick / (4 * ndotv * ndotl)
	float a = roughness + 1.0;
	float k = a * a * 0.125;

	float Vis_SchlickV = ndotv * (1 - k) + k;
	float Vis_SchlickL = ndotl * (1 - k) + k;

	return 0.25 / (Vis_SchlickV * Vis_SchlickL);
}
 vec3 ImportanceSampleGGX( vec2 Xi, float Roughness, vec3 N )
{
	float a = Roughness * Roughness;
	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt( (1 - Xi.y) / ( 1 + (a*a - 1) * Xi.y ) );
	float SinTheta = sqrt( 1 - CosTheta * CosTheta );
	vec3 H;
	H.x = SinTheta * cos( Phi );
	H.y = SinTheta * sin( Phi );
	H.z = CosTheta;
	vec3 UpVector = abs(N.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
	vec3 TangentX = normalize( cross( UpVector, N ) );
	vec3 TangentY = cross( N, TangentX );
	// Tangent to world space
	return TangentX * H.x + TangentY * H.y + N * H.z;
}
vec3 SpecularIBL( vec3 SpecularColor, float Roughness, vec3 N, vec3 V )
{
	vec3 SpecularLighting = vec3(0);
	const uint NumSamples = 1024;
	for( uint i = 0; i < NumSamples; i++ )
	{
		vec2 Xi = Hammersley( i, NumSamples );
		vec3 H = ImportanceSampleGGX( Xi, Roughness, N );
		vec3 L = 2 * dot( V, H ) * H - V;
		float NoV = clamp( dot( N, V ),0,1 );
		float NoL = clamp( dot( N, L ),0,1 );
		float NoH = clamp( dot( N, H ),0,1 );
		float VoH = clamp( dot( V, H ),0,1 );
		if( NoL > 0 )
		{
			vec3 SampleColor = textureLod(env_map, L, 0 ).rgb;
			float G = G_Smith( Roughness, NoV, NoL );
			float Fc = pow( 1 - VoH, 5 );
			vec3 F = (1 - Fc) * SpecularColor + Fc;
			// Incident light = SampleColor * NoL
			// Microfacet specular = D*G*F / (4*NoL*NoV)
			// pdf = D * NoH / (4 * VoH)
			SpecularLighting += SampleColor * F * G * VoH / (NoH * NoV);
		}
	}
	return SpecularLighting / NumSamples;
}
vec3 prefilter_diffuse(vec3 N_world){
	return textureLod(env_map_filtered, N_world, 0.0).rgb;
}

vec3 getPosition(vec2 uv){
  float depth = texture(gbuffer_depth, texcoord).x;
  vec3 ndc = 2.0 * vec3(texcoord, depth) - 1.0;
  vec4 P = view_proj_inv_mat * vec4(ndc, 1.0); 
  P.xyz /= P.w;
  return P.xyz;
}

vec3 getNormal2(vec2 uv){
  vec4 Normalraw = texture(gbuffer_normal2, texcoord);
  return normalize(Normalraw.xyz);
}
void main(){
	float depth = texture(gbuffer_depth, texcoord).x;
	vec4 gbuffer1 = texture(gbuffer_diffuse, texcoord).rgba;
	vec3 base_color = gbuffer1.rgb;
	vec3 N=getNormal2(texcoord);
	vec3 P_world=getPosition(texcoord);



	vec3 V = normalize(eyepos - Position);


	if (1.0==depth) {
		image_out = textureLod(env_map, Position.xyz, 0.0).rgb;
		return;
	}

	float roughness=0.8;
	float metalness=1;
	vec3 diffuse=base_color* prefilter_diffuse(Normal);
	vec3 specular=SpecularIBL(vec3(1.0),roughness,Normal,V);
	image_out=diffuse;
	//image_out=base_color;
	//image_out=SpecularIBL(vec(1.0),)
}