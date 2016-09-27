
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
	//mat4 World=WorldMatrix;
	Position = (World * vec4(vPosition,1.0)).xyz;	
	Texcoord=vTexcoord;
	Normal=transpose(inverse(mat3(World)))*(vNormal);

	gl_Position = Projection* ViewMatrix*World * vec4(vPosition,1.0);

}

