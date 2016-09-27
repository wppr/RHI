#version 300 es
precision highp float;

in vec3 position_eye, normal_eye;
in vec2 texcoord;
in vec3 o_world_pos;
uniform mat4 ViewMatrix;

vec3 light_position_world  = vec3 (1.0, 20.0, 30.0);

uniform vec3 eyepos;
       
uniform vec4 diffuse_color;
uniform sampler2D diffuse_texture;          

uniform vec4 specular_color;
uniform sampler2D specular_texture;

uniform vec4 ambient_color;
uniform sampler2D ambient_texture;
uniform mat4 proj_inv;

uniform highp sampler2D shadow_map_sampler;
uniform mat4        shadow_mat;

out vec4 fragment_colour;


void main()
{
  vec3 norm = normalize(normal_eye);
  vec3 light_position_eye = vec3 (ViewMatrix * vec4 (light_position_world, 1.0));
  vec3 lightVec = light_position_eye - position_eye;
  vec3 L = normalize(lightVec);
  vec3 viewVec=-position_eye;
  vec3 V = normalize(viewVec);
  vec3 halfAngle = normalize(L + V);
  float NdotL = dot(L, norm);
  float NdotH = clamp(dot(halfAngle, norm), 0.0, 1.0);


  float diffuse  = 0.5 * NdotL + 0.5;
  float specular = pow(NdotH, 64.0);

  float result = diffuse + specular*0.3;

  vec4 color = texture(diffuse_texture, texcoord).bgra*diffuse_color;

  vec3 N=vec3(normalize(normal_eye));

//shadow map
  vec4 shadow_map_coord = shadow_mat * vec4(o_world_pos,1.0);
 // float shadow_term = textureProj(shadow_map_sampler, shadow_map_coord);
  float depthmap = texture(shadow_map_sampler, shadow_map_coord.xy / shadow_map_coord.w).x;
  float shadow_term = 1.0;
  if (0.003+depthmap<shadow_map_coord.z / shadow_map_coord.w)
    shadow_term = 0.4;


  fragment_colour =vec4(color.xyz*result,1.0)*shadow_term;

 //  mat4 inv_view=inverse(ViewMatrix);
 //  vec4 position_world = inv_view * vec4(position_eye,1.0);
 //  position_world /= position_world.w;
 // // fragment_colour=ViewMatrix*vec4(1.0);
 //  fragment_colour=vec4(position_eye.xyz,1.0);

  //fragment_colour=vec4(normalize(position_eye)*0.5+0.5,1.0);
}

  
