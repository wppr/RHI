
in vec2 texcoord;

out vec4 ao_color;
uniform sampler2D zbuffer;
uniform sampler2D normal;
uniform sampler2D g_random;
uniform vec3 samples[32];

uniform mat4 proj;
uniform mat4 proj_inv;


float g_sample_rad=1.0;
    float scale = 0.5;
    float bias = 0.1;
    float intensity = 3; 

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

float doAmbientOcclusion(vec2 tcoord, vec2 uv, vec3 p, vec3 cnorm)
{
    vec3 diff = getPosition(tcoord+uv).xyz-p;
    vec3 v = normalize(diff);
    float d = length(diff) * scale;
    return max(0.0,dot(cnorm,v)-bias)*(1.0/(1.0+d))* intensity;

}

float ambientOcclusion(){
    vec3 p = getPosition(texcoord.xy);
    vec3 n = getNormal(texcoord.xy);
    vec2 rnd = normalize(vec2(getRandom2(p.xy), getRandom2(n.xy)));

    float ao = 0.0f;
    float rad = 1.0/p.z;
    vec2 vec[4]; 
    vec[0] = vec2(1.0,0.0); 
    vec[1] = vec2(-1.0,0.0); 
    vec[2] = vec2(0.0,1.0); 
    vec[3] = vec2(0.0,-1.0);

    int iterations = 4;
    for (int j = 0; j < iterations; ++j)
    {
      vec2 coord1 = reflect(vec[j],rnd)*rad;
      vec2 coord2 = vec2(coord1.x*0.707-coord1.y*0.707,
                  coord1.x*0.707 + coord1.y*0.707);
      
      ao += doAmbientOcclusion(texcoord,coord1*0.25, p, n);
      ao += doAmbientOcclusion(texcoord,coord2*0.5, p, n);
      ao += doAmbientOcclusion(texcoord,coord1*0.75, p, n);
      ao += doAmbientOcclusion(texcoord,coord2, p, n);
    }

    ao= ao/(float(iterations)*4.0);
    return 1.0 -ao; 
}

void main(){
    vec3 p = getPosition(texcoord.xy);
    vec3 n = getNormal(texcoord.xy);
    vec2 rnd = normalize(vec2(getRandom2(p.xy), getRandom2(n.xy)));
  ao_color=vec4(ambientOcclusion());
  ao_color=vec4(n,1.0);
}