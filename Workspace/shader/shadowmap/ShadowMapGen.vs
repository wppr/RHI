
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexcoord;
layout(location = 3) in vec4 vertex_bone_weight;
layout(location = 4) in ivec4 vertex_bone_id;								


uniform mat4 proj_light;
uniform	mat4 view_light;
uniform mat4 WorldMatrix;
uniform float Far;
uniform float Near;
uniform int  LinearDepth;

void main() {
	
	
	vec4 pos=proj_light*view_light *WorldMatrix*vec4(vPosition, 1.0);

	if(LinearDepth==1){
		vec4 view_pos=view_light *WorldMatrix*vec4(vPosition, 1.0);
		float z=view_pos.z/view_pos.w;
		float z1=(-z-Near)/(Far-Near);//the z  that we want ,linear in  0..1
		pos.z=pos.w*(2*z1-1);
	}
	gl_Position=pos;
}