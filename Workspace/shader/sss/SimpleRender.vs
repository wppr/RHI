
uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform mat4 WorldMatrix;						



layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexcoord;
layout (location = 3) in vec3 vTangent;						
layout (location = 4) in vec3 vBiTangent;		

out vec3 normal;
out vec3 o_world_pos;
out vec2 texcoord;
out vec3 tangent;
out vec3 bitangent;
void main()
{
	texcoord=vTexcoord;

	vec4 world_pos=WorldMatrix*vec4(vPosition,1.0);
	o_world_pos=world_pos.xyz/world_pos.w;

	mat4 mvp=Projection*ViewMatrix*WorldMatrix;

	mat3 wm2=transpose(inverse(mat3(WorldMatrix)));

	normal=normalize(wm2*vNormal);
	tangent=normalize(wm2*vTangent);
	bitangent=normalize(wm2*vBiTangent);
	// vec4 t_normal=transpose(inverse(WorldMatrix))*vec4(vNormal,1.0);
	// normal=t_normal.xyz/t_normal.w;

	// vec4 t_tangent=WorldMatrix*vec4(vTangent,1.0);
	// tangent=t_tangent.xyz/t_tangent.w;

	gl_Position = mvp* vec4(vPosition,1.0);
}