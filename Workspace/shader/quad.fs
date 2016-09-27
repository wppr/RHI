

in vec2 texcoord;
uniform sampler2D test_tex;
uniform sampler2D test_tex2;
uniform samplerCube test_tex_cube;
out vec4 color;

void main() {
  vec4 q = texture(test_tex, texcoord);
  // vec4 p = texture(test_tex2, texcoord);
  // vec4 g=texture(test_tex_cube, vec3(texcoord.xy,1.0));
  color=vec4(q.xyz,1.0);
 // color=vec4(1-1*(1-q.x));
}