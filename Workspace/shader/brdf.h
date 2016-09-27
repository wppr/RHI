#define float3 vec3
#define saturate(a) clamp(a, 0.0, 1.0)
#define PI										3.14159265358979323846



float Square( float x )
{
	return x*x;
}

float3 Square( float3 x )
{
	return x*x;
}
/*previously used 

vec3 fresnel_schlick(vec3 f0, float VdotH) {
	return f0 + (1.0 - f0) * pow(1.0 - VdotH, 5.0);
}


float G_Smith_Env(float roughness, float ndotv,float ndotl)
{
	// = G_Schlick / (4 * ndotv * ndotl)

	//for analytical light
	//float a = roughness + 1.0;
	//float k = a * a * 0.125;
	float k = roughness*roughness*0.5;
	float Vis_SchlickV = ndotv/(ndotv * (1 - k) + k);
	float Vis_SchlickL = ndotl/(ndotl * (1 - k) + k);

	return (Vis_SchlickV * Vis_SchlickL);
}
float G_Smith_Analytical(float roughness, float ndotv,float ndotl)
{
	// = G_Schlick / (4 * ndotv * ndotl)

	//for analytical light
	//float a = roughness + 1.0;
	float k = a * a * 0.125;
	//float k = roughness*roughness*0.5;
	float Vis_SchlickV = ndotv/(ndotv * (1 - k) + k);
	float Vis_SchlickL = ndotl/(ndotl * (1 - k) + k);

	return (Vis_SchlickV * Vis_SchlickL);
}

*/

//unreal

float3 Diffuse_Lambert( float3 DiffuseColor )
{
	return DiffuseColor * (1 / PI);
}


float Vis_Schlick( float Roughness, float NoV, float NoL )
{
	float k = Square( Roughness ) * 0.5;
	float Vis_SchlickV = NoV * (1 - k) + k;
	float Vis_SchlickL = NoL * (1 - k) + k;
	return 0.25 / ( Vis_SchlickV * Vis_SchlickL );
}
float Vis_Schlick_Analytic( float Roughness, float NoV, float NoL )
{
	Roughness=(Roughness+1)*0.5;
	float k = Square( Roughness ) * 0.5;
	float Vis_SchlickV = NoV * (1 - k) + k;
	float Vis_SchlickL = NoL * (1 - k) + k;
	return 0.25 / ( Vis_SchlickV * Vis_SchlickL );
}
float D_GGX( float Roughness, float NoH )
{
	float a = Roughness * Roughness;
	float a2 = a * a;
	float d = ( NoH * a2 - NoH ) * NoH + 1;	
	return a2 / ( PI*d*d );					
}
float3 F_Schlick( float3 SpecularColor, float VoH )
{
	float Fc = pow( 1 - VoH,5.0 );					
	//return Fc + (1 - Fc) * SpecularColor;		
	
	// Anything less than 2% is physically impossible and is instead considered to be shadowing
	return saturate( 50.0 * SpecularColor.g ) * Fc + (1 - Fc) * SpecularColor;
	
}

float3 StandardShading(float3 DiffuseColor,float3 SpecularColor, float Roughness, float3 LightColor, float3 L, float3 V, float3 N)
{

	float3 H = normalize(V + L);
	float NoL = saturate( dot(N, L) );
	if(NoL<=0) return vec3(0.0);
	//float NoV = saturate( dot(N, V) );
	float NoV = max( dot(N, V), 1e-5 );	// constant to prevent NaN
	float NoH = saturate( dot(N, H) );
	float VoH = saturate( dot(V, H) );
	
	// Generalized microfacet specular
	float D = D_GGX( Roughness, NoH );
	float Vis = Vis_Schlick_Analytic( Roughness, NoV, NoL );
	float3 F = F_Schlick( SpecularColor, VoH );

	float3 fd = Diffuse_Lambert( DiffuseColor );
	float3 fr= D*F*Vis;

	return (fr+fd)*LightColor*NoL;
	// f(l,v)*light * cos 
	//return (Diffuse+Specular)*LightEnergy*NoL;
	//return Diffuse * (LobeEnergy[2] * DiffSpecMask.r) + (D * Vis * DiffSpecMask.g) * F;
}


vec3 prefilter_irradiance(samplerCube irradianceMap,vec3 N_world){
	return textureLod(irradianceMap, N_world, 0.0).rgb;
}

vec3 PrefilterEnvMap(samplerCube envMap,float Roughness,vec3 R){
	return textureLod(envMap,R,Roughness*5).rgb;
}
vec2 IntegrateBRDF(sampler2D BRDFlut,float Roughness,float NoV ){
	return textureLod(IntegrateBRDFSampler,vec2(Roughness,NoV),0).xy;
}
vec3 ApproximateSpecularIBL(sampler2D BRDFlut,samplerCube envMap, vec3 SpecularColor, float Roughness, vec3 N, vec3 V )
{
	float NoV = clamp( dot( N, V ),1e-5,1.0 );
	vec3 R = 2 * dot( V, N ) * N - V;
	vec3 PrefilteredColor = PrefilterEnvMap(envMap,Roughness, R );
	vec2 EnvBRDF = IntegrateBRDF(BRDFlut,Roughness, NoV );
	return PrefilteredColor*( SpecularColor * EnvBRDF.x + EnvBRDF.y );
}

float3 EnvShading(float3 DiffuseColor,float3 SpecularColor, float Roughness,samplerCube envMap,samplerCube irradianceMap,sampler2D BRDFlut, float3 V, float3 N){
	float3 diffuse= Diffuse_Lambert(DiffuseColor)*prefilter_irradiance(irradianceMap,N);
	float3 specular=ApproximateSpecularIBL(BRDFlut,envMap,SpecularColor,Roughness,N,V);
	return diffuse+specular;
}