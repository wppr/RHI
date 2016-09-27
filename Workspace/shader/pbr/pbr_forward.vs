
uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform mat4 WorldMatrix;						
uniform vec3 eyepos;

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexcoord;



out vec2 Texcoord;
out vec3 Position;
out vec3 Normal;
void main()
{

	Position = (WorldMatrix * vec4(vPosition,1.0)).xyz;	
	Texcoord=vTexcoord;
	Normal=transpose(inverse(mat3(WorldMatrix)))*(vNormal);

	gl_Position = Projection* ViewMatrix*WorldMatrix * vec4(vPosition,1.0);

}