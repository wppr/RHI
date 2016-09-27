
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexcoord;
layout (location = 3) in vec3 vTangent;						
layout (location = 4) in vec3 vBiTangent;		

out vec2 texcoord;
out vec3 o_view;
out vec3 o_normal;
out vec3 o_tangent;
out vec4 worldPosition;


uniform mat4 world;
uniform mat4 currWorldViewProj;
uniform vec3 cameraPosition;


void main( ) 
{
	texcoord = vTexcoord;
	mat4 worldInverseTranspose = transpose(inverse(world));
	worldPosition = ( world * vec4(vPosition,1.0));
	o_view = cameraPosition - worldPosition.xyz;
	o_normal =  (mat3(worldInverseTranspose) * vNormal);
	o_tangent = (mat3(worldInverseTranspose) * vTangent);
	gl_Position = 	currWorldViewProj * vec4(vPosition,1.0);
	
}