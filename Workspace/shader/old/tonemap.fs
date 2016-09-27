#version 300 es
precision highp float;

in vec2 texcoord;

out vec3 color;

uniform sampler2D hdr_image_sampler;

uniform sampler2D luminance_sampler;


int level=11;

vec3 filmic_tone_map_aux(vec3 hdr) {
	const float A = 0.15; const float B = 0.50; const float C = 0.10;
    const float D = 0.20; const float E = 0.02; const float F = 0.30;
    return ((hdr * ( A * hdr + C * B) + D * E) / (hdr * ( A * hdr + B) + D * F)) - E / F;	
}

vec3 filmic_tone_map(vec3 hdr, float white) {
	return filmic_tone_map_aux(hdr) / filmic_tone_map_aux(vec3(white));
}

float magicTone(float LinearColor){
	float x=max(0,LinearColor-0.004);
	return (x*(6.2*x+0.5))/(x*(6.2*x+1.7)+0.06);
}
vec3 magTone(vec3 color){
	vec3 c;
	c.r=magicTone(color.r);
	c.g=magicTone(color.g);
	c.b=magicTone(color.b);
	return c;
}
void main() {
	const float log10inv = 1.0 / log(10.0);
	const float whitepoint = 20.0;
	vec3 image = texture(hdr_image_sampler, texcoord).rgb;
	float lumavg = (textureLod(luminance_sampler, texcoord, 10).r);
	lumavg=0.02;
	float luminance = dot(image.rgb, vec3(0.2126, 0.7152, 0.0722));
	float key = 1.03 - 2.0 / (2.0 + log10inv * log(lumavg + 1.0));
	float lumscaled = key * luminance / lumavg;
	float lumcompress = lumscaled * (1.0 + lumscaled / (whitepoint * whitepoint)) / (1.0 + lumscaled);


	color = filmic_tone_map(image * key / lumavg, 5.0);

	// color = pow(color, vec3(0.45));
	// color=magTone(image);

	//color=vec3(lumavg);
	//color=texture(luminance_sampler, texcoord).rgb;
}