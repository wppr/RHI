
in vec2 Texcoord;
in vec3 Position;
in vec3 Normal;

uniform vec4 diffuse_color;
uniform sampler2D diffuse_texture;					

uniform vec4 specular_color;
uniform sampler2D specular_texture;

uniform vec4 ambient_color;
uniform sampler2D ambient_texture;

uniform float near;
uniform float far;


layout (location = 0) out vec4 DiffuseOut; 
//layout (location = 1) out vec4 PositionOut; 
//layout (location = 2) out uvec2 NormalOut; 
layout (location = 3) out vec4 NormalOut2; 

vec3 GetBaseColor(vec4 color,sampler2D tex){
    vec3 base_color=color.xyz;
    vec3 tex_color=texture(tex,Texcoord).rgb;
    if(color.w==1.0)    
        base_color=tex_color;
    return base_color;

}

void main() 
{ 

    DiffuseOut = vec4(GetBaseColor(diffuse_color,diffuse_texture),1.0);
    NormalOut2=vec4(normalize(Normal),1.0);



   //  //specular out
   // SpeculerOut = vec4(GetBaseColor(specular_color,specular_texture),1.0);

   //  //ambient out
   //  SpeculerOut.w=dot(ambient_color.rgb, vec3(0.2126, 0.7152, 0.0722));



    //normal out
    // vec3 N=normalize(v_normal)*0.5+0.5; 
    // vec2 n1=EncodeFloat2Byte(N.x);
    // vec2 n2=EncodeFloat2Byte(N.y);
    // vec2 n3=EncodeFloat2Byte(N.z);
    
    //NormalOut=vec4(n1.x,n1.y,n2.x,n2.y);

 //   NormalOut2=vec4(n3.x,n3.y,1.0,1.0);

   // NormalOut=uvec2(packHalf2x16(Normal.xy), packHalf2x16(vec2(Normal.z, 0.0)));

    // NormalOut2=vec4(decode(n1),decode(n2),decode(n3),1.0);
    // //float
    // NormalOut=vec4(normalize(v_normal),1.0);
    // NormalOut2=vec4(pos_view,1.0);
}
	

     