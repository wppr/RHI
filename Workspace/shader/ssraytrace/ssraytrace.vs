#version 300 es
precision highp float;
uniform mat4 ProjMatrix;
uniform mat4 ViewMatrix;
uniform mat4 WorldMatrix;						


layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
					
out vec3 msPosition;
out vec3 csPosition;
out vec3 csNormal;
out vec2 texcoord;
void main()
{
	
	vec4 world_pos = WorldMatrix * vec4(vPosition,1.0);
	vec4 view_pos=ViewMatrix * world_pos;
	csPosition = view_pos.xyz/view_pos.w;
	msPosition=vPosition;
	csNormal = (ViewMatrix*WorldMatrix*vec4(vNormal,0)).xyz;

	vec4 proj_pos=ProjMatrix* view_pos;
	vec3 Pos= proj_pos.xyz/proj_pos.w;
	texcoord.x = 0.5 * (1.0 + Pos.x);
	texcoord.y = 0.5 * (1.0 + Pos.y);
	gl_Position = proj_pos;
}