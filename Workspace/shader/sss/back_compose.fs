

in vec2 texcoord;
uniform sampler2D back;
uniform sampler2D front;
out vec4 color;

void main() {
  vec4 b = texture(back, texcoord);
  vec4 f = texture(front, texcoord);
  float l=length(f);
  if(l==0)
  	color=b;
  else 
  	color=f;
  //color=vec4(1.0);
  //
 // color=vec4(1-1*(1-q.x));
}