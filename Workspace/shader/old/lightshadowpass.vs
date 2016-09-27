
uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform mat4 WorldMatrix;						
uniform vec3 eyepos;

						
in vec4 vPosition;	
out vec2 v_texCoord;

void main(){
	vec4 Pos = vec4(vPosition.x,vPosition.y,0.99,1.0);
	v_texCoord.x = 0.5 * (1.0 + Pos.x);
	v_texCoord.y = 0.5 * (1.0 + Pos.y);
	gl_Position = Pos;
}
