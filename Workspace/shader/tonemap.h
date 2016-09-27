
//need to multiply by exposure before the tone mapping and do the gamma correction after.


vec3 ACESFilm( vec3 x )
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e),0.0,1.0);
}


float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;
vec3 Uncharted2Tonemap(vec3 x)
{
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 Uncharted2TonemapFull(vec3 texColor,float ExposureBias)
{
   texColor *= 16; 
   vec3 curr = Uncharted2Tonemap(ExposureBias*texColor);

   vec3 whiteScale = vec3(1.0)/Uncharted2Tonemap(vec3(W));
   vec3 color = curr*whiteScale;

   vec3 retColor = pow(color,vec3(1/2.2));
   return retColor;

}