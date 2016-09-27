





float ShadowLinear(sampler2D shadowMap,mat4 shadowMat,  vec3 worldPosition, float bias,float n,float f) {
	vec4 shadowPosition = shadowMat* vec4(worldPosition, 1.0);
	shadowPosition.xy /= shadowPosition.w;
	shadowPosition.z += bias;
	float z=shadowPosition.z*2-shadowPosition.w;//range -near ..far
	float z2=(z+n)/(f+n);//range 0..1
	float z1=texture(shadowMap, shadowPosition.xy).r;
	if(z2>z1)
		return 0.0;
	else 
		return 1.0;
}


float Shadow(sampler2D shadowMap,mat4 shadowMat,  vec3 worldPosition, float bias) {
	vec4 shadowPosition = shadowMat* vec4(worldPosition, 1.0);
	shadowPosition.xy /= shadowPosition.w;
	shadowPosition.z += bias;
	float z2=shadowPosition.z/shadowPosition.w;
	float z1=texture(shadowMap, shadowPosition.xy).r;
	if(z2>z1)
		return 0.0;
	else 
		return 1.0;
}


// VSM

float g_MinVariance=0.0002;
uniform float ReduceBleeding;
float linstep(float low, float high, float v)
{
	return clamp((v-low)/(high-low), 0.0, 1.0);
}
float ReduceLightBleeding(float p_max, float Amount)  
{  
   return linstep(Amount, 1, p_max);  
}  
float ChebyshevUpperBound(vec2 Moments, float t)  
{  
  // One-tailed inequality valid if t > Moments.x  
   float p=step(t,Moments.x);
  // Compute variance.  
   float Variance = Moments.y-(Moments.x*Moments.x);  
  Variance = max(Variance, g_MinVariance);  
  // Compute probabilistic upper bound.  
   float d = t-Moments.x;  
  float p_max = linstep(ReduceBleeding,1.0,Variance/(Variance+d*d)); 
  return max(p,p_max);  

}


float ShadowVSM(sampler2D shadowMap,mat4 shadowMat, vec3 worldPosition,vec3 Lightpos)  
{  
	float SurfaceDistToLight = length(Lightpos-worldPosition);
	vec4 shadowPosition = shadowMat* vec4(worldPosition, 1.0);
	vec3 shadowCoord=shadowPosition.xyz/shadowPosition.w;
   vec2 Moments = texture(shadowMap, shadowCoord.xy).xy;  
   Moments=vec2(Moments.x,Moments.y);
   return ChebyshevUpperBound(Moments, SurfaceDistToLight);  
}  