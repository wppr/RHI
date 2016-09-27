
in vec3 texcoord;
uniform samplerCube env_map;
uniform sampler2D depth;
uniform sampler2D front;
uniform int isHdrSky;
uniform float w;
uniform float h;
out vec3 color;




void main() {
  vec2 screencoord=gl_FragCoord.xy/vec2(w,h);
  float d = texture(depth, screencoord).x;
  vec4 f = texture(front, screencoord);
  vec3 env = textureLod(env_map, texcoord, 0.0).rgb;
  if(isHdrSky==0){
    env=pow(env,vec3(2.2));
  }
  color=f.rgb;
  if (d>0.99999) {
	 color = env;
	}

}