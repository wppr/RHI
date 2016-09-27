#version 300 es
precision highp float;
uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform mat4 WorldMatrix;						
uniform vec3 eyepos;

in vec4 vPosition;							
in vec2 vTexcoord ;							
in vec4 vNormal;									


out vec3 v_normal;
out vec2 v_texcoord;


void main()
{
	vec4 world_pos = WorldMatrix * vPosition;
	vec4 view_pos = ViewMatrix*vec4(world_pos.x,world_pos.y,world_pos.z,1.0);

	v_texcoord=vTexcoord;

	v_normal = (WorldMatrix * vec4(vNormal.x,vNormal.y,vNormal.z,0.0)).xyz; 
	gl_Position = Projection* view_pos;
}