
in vec2 texcoord;
uniform samplerCube env_map;
uniform sampler2D depth;
uniform sampler2D front;
uniform mat4 view_proj_inv_mat;
uniform int isHdrSky;
out vec3 color;


vec3 getPosition(vec2 uv){
  float depth = texture(depth, texcoord).x;
  vec3 ndc = 2.0 * vec3(texcoord, depth) - 1.0;
  vec4 P = view_proj_inv_mat * vec4(ndc, 1.0); 
  P.xyz /= P.w;
  return P.xyz;
}


void main() {
  vec4 d = texture(depth, texcoord);
  vec4 f = texture(front, texcoord);
  vec3 position_world=getPosition(texcoord);
  vec3 env = textureLod(env_map, position_world.xyz, 0.0).rgb;
  if(isHdrSky==0){
    env=pow(env,vec3(2.2));
  }
  color=f.rgb;
  if (d==1.0) {
	 color = env;
	}

}