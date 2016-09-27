
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

  vec4 color = texture(diffuse_texture, texcoord)*diffuse_color;

  vec3 N=vec3(normalize(normal_eye));

  //fragment_colour =vec4(color.xyz*result,1.0);
  //fragment_colour= vec4(normalize(normal_eye),1.0);
  //fragment_colour=texture(diffuse_texture, texcoord).bgra;
  fragment_colour= vec4(N*0.5+0.5,1.0);
  //fragment_colour=vec4(normalize(normal_eye),1.0);
  // vec3 c=vec3(gl_FragCoord.x/1200,gl_FragCoord.y/900,gl_FragCoord.z);
  // vec3 ndc = 2.0 * c - 1.0;
  // vec4 P = proj_inv * vec4(ndc, 1.0); 
  // P.xyz /= P.w;
  // fragment_colour=vec4(position_eye,1.0);
}

	
