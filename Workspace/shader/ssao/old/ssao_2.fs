

in vec2 texcoord;

out vec4 ao_color;
uniform sampler2D zbuffer;
uniform sampler2D normal;
uniform sampler2D g_random;
uniform vec3 samples[32];

uniform mat4 proj;
uniform mat4 proj_inv;

//float acc_image;
//float radius=0.2;
//int num_samples=32;

float random_size=64;
float g_sample_rad=1.0;
float g_intensity=3;
float g_scale=0.5;
float g_bias=0.1;
float decode(vec2 rg)
{
   return dot(rg, vec2(1.0, 1.0 / 255.0f));
}
vec3 getPosition(vec2 uv){
  float depth = texture(zbuffer, texcoord).x;
  vec3 ndc = 2.0 * vec3(texcoord, depth) - 1.0;
  vec4 P = proj_inv * vec4(ndc, 1.0); 
  P.xyz /= P.w;
  return P.xyz;
}
float getRandom2(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
vec3 getNormal(vec2 uv){
  vec4 Normalraw = texture(normal, texcoord);
  vec3 N=vec3(decode(Normalraw.xy),decode(Normalraw.zw),0.0);
  N=N*2-1;
  N.z=0;
  N.z = sqrt(1.0 - dot(N, N));
  return N;
}
vec2 getRandom(vec2 uv)
{
  vec2 g_screen_size=vec2(1200,900);
 return normalize(texture(g_random, g_screen_size * uv / random_size).xy * 2.0f - 1.0f);
}
float doAmbientOcclusion(vec2 tcoord,vec2 uv, vec3 p, vec3 cnorm)
{
 vec3 diff = getPosition(tcoord + uv).xyz - p;
 const vec3 v = normalize(diff);
 const float d = length(diff)*g_scale;
 return max(0.0,dot(cnorm,v)-g_bias)*(1.0/(1.0+d))*g_intensity;
}

void main() {
 const vec2 vecs[4] = {vec2(1,0),vec2(-1,0),
            vec2(0,1),vec2(0,-1)};
 vec3 p = getPosition(texcoord);
 vec3 n = getNormal(texcoord);
 //vec2 rand = getRandom(texcoord);
 vec2 rand=normalize(vec2(getRandom2(p.xy), getRandom2(n.xy)));
 float ao = 0.0f;
 float rad = g_sample_rad/p.z;
 //**SSAO Calculation**//
 int iterations = 4;
 for (int j = 0; j < iterations; j++)
 {
  vec2 coord1 = reflect(vecs[j],rand)*rad;
  vec2 coord2 = vec2(coord1.x*0.707 - coord1.y*0.707, coord1.x*0.707 + coord1.y*0.707);
  
  ao += doAmbientOcclusion(texcoord,coord1*0.25, p, n);
  ao += doAmbientOcclusion(texcoord,coord2*0.5, p, n);
  ao += doAmbientOcclusion(texcoord,coord1*0.75, p, n);
  ao += doAmbientOcclusion(texcoord,coord2, p, n);
 } 
 ao=1-ao/float(iterations)/4.0;
 ao_color=vec4(vec3(ao),1.0);
}