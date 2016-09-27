#version 300 es
precision highp float;
uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform mat4 WorldMatrix;						


in vec4 vPosition;							


void main()
{
	vec4 world_pos = WorldMatrix * vPosition;
	vec4 view_pos = ViewMatrix*vec4(world_pos.x,world_pos.y,world_pos.z,1.0);

	gl_Position = Projection* view_pos;
}