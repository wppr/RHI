
in vec2 texcoord;

out float frag_color;

uniform sampler2D image_hdr;

uniform sampler2D image;

vec4 EncodeFloat4Byte(float v)
{
   vec4 enc = vec4(1.0f, 255.0f, 65025.0f, 16581375.0f) * v;
   enc = fract(enc);
   enc -= enc.yzww * vec4(1.0 / 255.0f, 1.0 / 255.0f, 1.0 / 255.0f, 0);
   return enc;
}

void main() {
	vec3 color = texture(image, texcoord).rgb;
	float luminance = (dot(color, vec3(0.2126, 0.7152, 0.0722)) + 0.001);
	
	//frag_color=vec4(vec3(luminance),1.0);
	frag_color=luminance;
}