
in vec2 texcoord;
out vec2 color;

#include "GGXSample.h"

vec3 fresnel_schlick(vec3 f0, float VdotH) {
	return f0 + (1.0 - f0) * pow(1.0 - VdotH, 5.0);
}

float G_Smith(float roughness, float ndotv,float ndotl)
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

vec2 IntegrateBRDF( float Roughness, float NoV )
{
	vec3 V;
	V.x = sqrt( 1.0f - NoV * NoV ); // sin
	V.y = 0;
	V.z = NoV; // cos
	float A = 0;
	float B = 0;
	const uint NumSamples = 1024;
	for( uint i = 0; i < NumSamples; i++ )
	{
		vec2 Xi = Hammersley( i, NumSamples );
		vec3 H = ImportanceSampleGGX( Xi, Roughness, vec3(0,0,1) );
		vec3 L = 2 * dot( V, H ) * H - V;
		float NoL = clamp( L.z,0.0,1.0 );
		float NoH = clamp( H.z ,0.0,1.0);
		float VoH = clamp( dot( V, H ),0.0,1.0 );
		if( NoL > 0 )
		{
			float G = G_Smith( Roughness, NoV, NoL );
			float G_Vis = G * VoH / (NoH * NoV);
			float Fc = pow( 1 - VoH, 5 );
			A += (1 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}
	return vec2( A, B ) / NumSamples;
}

void main(){
	color=IntegrateBRDF(texcoord.x,texcoord.y);
}