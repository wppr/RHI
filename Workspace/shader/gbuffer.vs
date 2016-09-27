
uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform mat4 WorldMatrix;						

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexcoord;

uniform mat4 InstanceMatrix[50];


out vec2 Texcoord;
out vec3 Position;
out vec3 Normal;
void main()
{
	mat4 World=InstanceMatrix[gl_InstanceID];
	Position = (World * vec4(vPosition,1.0)).xyz;	
	Texcoord=vTexcoord;
	Normal=transpose(inverse(mat3(World)))*(vNormal);

	gl_Position = Projection* ViewMatrix*World * vec4(vPosition,1.0);

}


// uniform mat4 Projection;
// uniform mat4 ViewMatrix;
// uniform mat4 WorldMatrix;						
// uniform vec3 eyepos;

// layout (location = 0) in vec3 vPosition;
// layout (location = 1) in vec3 vNormal;
// layout (location = 2) in vec2 vTexcoord;
// layout (location = 3) in vec4 vertex_bone_weight;
// layout (location = 4) in ivec4 vertex_bone_id;


// out vec3 v_normal;
// out vec2 v_texcoord;
// out vec3 pos_view;

// #define MAX_BONES 100

// uniform mat4 bone[MAX_BONES];


// void main()
// {


// 	vec4 skinned_position=vec4(0);
	
// 	vec4 weight=vertex_bone_weight;
	
// 	skinned_position+=bone[vertex_bone_id.x]*vec4(vPosition,1.0)*weight.x;
// 	skinned_position+=bone[vertex_bone_id.y]*vec4(vPosition,1.0)*weight.y;
// 	skinned_position+=bone[vertex_bone_id.z]*vec4(vPosition,1.0)*weight.z;
// 	skinned_position+=bone[vertex_bone_id.w]*vec4(vPosition,1.0)*(weight.w);

// 	vec4 skinned_normal=vec4(0);
// 	skinned_normal+=bone[vertex_bone_id.x]*vec4(vNormal,1.0)*(weight.x);
// 	skinned_normal+=bone[vertex_bone_id.y]*vec4(vNormal,1.0)*weight.y;
// 	skinned_normal+=bone[vertex_bone_id.z]*vec4(vNormal,1.0)*weight.z;
// 	skinned_normal+=bone[vertex_bone_id.w]*vec4(vNormal,1.0)*(weight.w);

// 	vec4 world_pos = WorldMatrix * skinned_position;
	
// 	v_texcoord=vTexcoord;
// 	vec4 view_pos=ViewMatrix * world_pos;
// 	pos_view=view_pos.xyz/view_pos.w;

// 	mat4 world_view_mat=ViewMatrix * WorldMatrix;
	
// 	v_normal=transpose(inverse(mat3(WorldMatrix)))*(vNormal);
// 	//v_normal=transpose(inverse(mat3(WorldMatrix)))*(vNormal);


// 	gl_Position = Projection* view_pos;

// }