
in vec3 normal,o_world_pos;
in vec2 texcoord;
in vec3 tangent;
in vec3 bitangent;

uniform mat4 ViewMatrix;
uniform vec3 eyepos;
vec3 light_position_world  = vec3 (1.0, 20.0, 30.0);
vec3 lightIntensity=vec3(2.0);

uniform vec4 diffuse_color;
uniform sampler2D diffuse_texture;          

uniform vec4 specular_color;
uniform sampler2D specular_texture;

uniform vec4 ambient_color;
uniform sampler2D ambient_texture;

uniform sampler2D NormalMap;
out vec4 fragment_colour;

float bumpiness=0.9;

vec3 BumpMap(sampler2D normalTex, vec2 texcoord) {
    vec3 bump;
    bump.xy = -1.0 + 2.0 * texture(normalTex, vec2(texcoord.x,texcoord.y)).xy;
    bump.z = sqrt(1.0 - bump.x * bump.x - bump.y * bump.y);
    return normalize(bump);
}

void main()
{

  //vec3 bitangent = cross(normal, tangent);
  mat3 tbn = transpose(mat3(-tangent, bitangent, normal));

    // Transform bumped normal to world space, in order to use IBL for ambient lighting:
  //vec3 tangentNormal = BumpMap(NormalMap, texcoord);//mix(vec3(0.0, 0.0, 1.0), , bumpiness);
  vec3 tangentNormal = BumpMap(NormalMap,texcoord);
  vec3 normal_bumped = tbn*tangentNormal;


  vec3 norm = normalize(normal_bumped);

  vec3 lightVec = light_position_world - o_world_pos;
  vec3 L = normalize(lightVec);
  vec3 viewVec=eyepos- o_world_pos;
  vec3 V = normalize(viewVec);
  vec3 halfAngle = normalize(L + V);
  float NdotL = dot(L, norm);
  float NdotH = clamp(dot(halfAngle, norm), 0.0, 1.0);


  float diffuse  = 0.5 * NdotL + 0.5;
  float specular = pow(NdotH, 64.0)*0.2;

  float result = diffuse + specular;

  vec4 color = texture(diffuse_texture, texcoord)*diffuse_color;

  //vec3 N=vec3(texcoord,0.0);
  vec3 N=normal_bumped;
  fragment_colour =vec4(lightIntensity*color.xyz*result,1.0);
  //fragment_colour =texture(NormalMap, texcoord);
//fragment_colour=color;
 // fragment_colour =vec4(specular);
  //fragment_colour= vec4(normalize(normal_eye),1.0);

  //fragment_colour=vec4(N*0.5+0.5,1.0);
  //fragment_colour=texture(diffuse_texture,gl_FragCoord.xy);
   //fragment_colour= vec4(tangentNormal,1.0);

  // fragment_colour=vec4(position_eye,1.0);
}

	
