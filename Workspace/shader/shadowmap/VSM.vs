
layout (location = 0) in vec3 vPosition;

uniform mat4 proj_light;
uniform	mat4 view_light;
uniform mat4 WorldMatrix;
uniform int  LinearDepth;

out vec4 PosView;
uniform mat4 InstanceMatrix[50];

void main() {
	mat4 World=InstanceMatrix[gl_InstanceID];
	PosView=view_light *World*vec4(vPosition, 1.0);
	gl_Position=proj_light*view_light *World*vec4(vPosition, 1.0);
}