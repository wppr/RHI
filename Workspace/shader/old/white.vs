#version 300 es
uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform mat4 WorldMatrix;						
uniform vec3 eyepos;

						
in vec2 aPosition;
in vec3 alightposition;
in vec3 alightdir;
in vec3 alightcolor;
in vec3 alightparam;

out vec2 v_texCoord;
out vec3 lightposition;
out vec3 lightdir;
out vec3 lightcolor;
out vec3 lightparam;

void main(){
	vec4 Pos = vec4(aPosition.x,aPosition.y,0.99,1.0);
	v_texCoord.x = 0.5 * (1.0 + Pos.x);
	v_texCoord.y = 0.5 * (1.0 + Pos.y);
	lightposition=alightposition;
	lightdir=alightdir;
	lightcolor=alightcolor;
	lightparam=alightparam;
	gl_Position = Pos;
}
