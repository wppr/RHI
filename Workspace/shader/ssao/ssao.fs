
in vec2 texcoord;
out vec4 ao_color;

uniform int num_samples;
uniform vec3 samples[64];


uniform sampler2D noise;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 proj_inv_mat;
uniform mat4 proj_view_inv_mat;
float acc_image;
uniform float radius;
uniform float near;
uniform float far;
uniform int w;
uniform int h;

#include "gbufferUse.h"

float LinearizeDepth(float depth,float near,float far)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main() {
  vec2 image_scale = 0.25* vec2(w,h);

  vec3 viewPosition=getViewPositionFromDepth(texcoord,proj_inv_mat);
  vec3 worldNormal=getWorldNormal(texcoord);
  vec3 viewNormal=mat3(view)*worldNormal; 
  vec3 N=normalize(viewNormal.xyz);

  //random vec
  vec3 D =texture(noise, texcoord * image_scale).xyz;
  vec3 T = normalize(D - dot(N, D) * N);
  vec3 B = cross(N, T);
  mat3 TBN=mat3(T,B,N);

  float occlusion=0.0;
  for(int i = 0; i < num_samples; ++i)
  {
        // get sample position
      vec3 sample_point = TBN * samples[i]; // From tangent to view-space
      sample_point = viewPosition + sample_point * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
      vec4 offset = vec4(sample_point, 1.0);
      offset = proj * offset; // from view to clip-space
      offset.xyz /= offset.w; // perspective divide
      offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
      float rawDepth=getGbufferDepth(offset.xy);
      float sampleDepth =-LinearizeDepth(rawDepth,near,far); // Get depth value of kernel sample
        
        // range check & accumulate
      float rangeCheck = smoothstep(0.0, 1.0, radius / abs(viewPosition.z - sampleDepth ));
      occlusion += (sampleDepth >= sample_point.z ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / num_samples);
    
  ao_color = vec4(occlusion);  
}