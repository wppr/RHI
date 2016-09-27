#version 300 es
precision highp float;

in vec3 v_normal;
in vec2 v_texcoord;

uniform vec4 diffuse_color;
uniform sampler2D diffuse_texture;					

uniform vec4 specular_color;
uniform sampler2D specular_texture;

uniform vec4 ambient_color;
uniform sampler2D ambient_texture;

layout (location = 0) out vec4 DiffuseOut; 
layout (location = 1) out vec4 NormalOut; 
layout (location = 2) out vec4 SpeculerOut; 
layout (location = 3) out vec4 NormalOut2; 


vec2 EncodeFloat2Byte(float v)
{
   vec2 enc = vec2(1.0f, 255.0f) * v;
   enc=fract(enc);
   enc-=enc.yy*vec2(1.0/255.0f,0.0);
   return enc;
}
vec4 EncodeFloat4Byte(float v)
{
   vec4 enc = vec4(1.0f, 255.0f, 65025.0f, 16581375.0f) * v;
   enc = fract(enc);
   enc -= enc.yzww * vec4(1.0 / 255.0f, 1.0 / 255.0f, 1.0 / 255.0f, 0);
   return enc;
}
void main() 
{ 

    //diffuse out
    vec4 color = texture(diffuse_texture, v_texcoord).bgra;
    if(dot(color.rgb, vec3(0.2126, 0.7152, 0.0722)) < 0.001) color = diffuse_color;
    DiffuseOut = color;

    //specular out
    color = texture(specular_texture, v_texcoord).bgra;
    if(dot(color.rgb, vec3(0.2126, 0.7152, 0.0722)) < 0.001) color = specular_color;
    SpeculerOut=specular_color;

    //ambient out
    SpeculerOut.w=dot(ambient_color.rgb, vec3(0.2126, 0.7152, 0.0722));

    //normal out
    vec3 N=normalize(v_normal); 
    vec2 n1=EncodeFloat2Byte(N.x/2.0+0.5);
    vec2 n2=EncodeFloat2Byte(N.y/2.0+0.5);
    vec2 n3=EncodeFloat2Byte(N.z/2.0+0.5);
    NormalOut=vec4(n1.x,n1.y,n2.x,n2.y);
    NormalOut2=vec4(n3.x,n3.y,1.0,1.0);




    

}
	

     