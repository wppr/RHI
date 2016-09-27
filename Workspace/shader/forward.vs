
uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform mat4 WorldMatrix;						



layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexcoord;
layout(location = 3) in vec4 vertex_bone_weight;
layout(location = 4) in ivec4 vertex_bone_id;								

out vec3 position_eye;
out vec3 normal_eye;
out vec2 texcoord;
out vec3 o_world_pos;

#define MAX_BONES 100

uniform mat4 bone[MAX_BONES];

void main()
{
	texcoord=vTexcoord;

	// vec4 skinned_position=vec4(0);
	
	// vec4 weight=vertex_bone_weight;
	
	// skinned_position+=bone[vertex_bone_id.x]*vec4(vPosition,1.0)*weight.x;
	// skinned_position+=bone[vertex_bone_id.y]*vec4(vPosition,1.0)*weight.y;
	// skinned_position+=bone[vertex_bone_id.z]*vec4(vPosition,1.0)*weight.z;
	// skinned_position+=bone[vertex_bone_id.w]*vec4(vPosition,1.0)*(weight.w);

	// vec4 skinned_normal=vec4(0);
	// skinned_normal+=bone[vertex_bone_id.x]*vec4(vNormal,1.0)*(weight.x);
	// skinned_normal+=bone[vertex_bone_id.y]*vec4(vNormal,1.0)*weight.y;
	// skinned_normal+=bone[vertex_bone_id.z]*vec4(vNormal,1.0)*weight.z;
	// skinned_normal+=bone[vertex_bone_id.w]*vec4(vNormal,1.0)*(weight.w);
	
	//vec4 world_pos = WorldMatrix * skinned_position;
	vec4 world_pos = WorldMatrix * vec4(vPosition,1.0);
	o_world_pos=world_pos.xyz/world_pos.w;

	vec4 view_pos=ViewMatrix * world_pos;
	position_eye = view_pos.xyz/view_pos.w;

	vec4 view_normal=ViewMatrix * WorldMatrix * vec4(vNormal,1.0);
	normal_eye = view_normal.xyz/view_normal.w;

	//mat4 world_view_mat=ViewMatrix * WorldMatrix;
	//normal_eye=transpose(inverse(mat3(world_view_mat)))*(vNormal);
	normal_eye=vNormal;
	gl_Position = Projection* view_pos;
}