


layout (location = 0) in vec3 vPositon;
out vec3 texcoord;
 
uniform mat4 ProjMatrix;
uniform mat4 ViewMatrix;
uniform mat4 WorldMatrix;
 
void main()
{
    gl_Position =   ProjMatrix * ViewMatrix *WorldMatrix*vec4(vPositon, 1.0);  
    texcoord = vPositon;
}