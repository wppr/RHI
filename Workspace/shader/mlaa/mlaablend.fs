
in vec2 texcoord;
out vec3 image_out;

uniform sampler2D image_in_sampler;
uniform sampler2D weight_sampler;
uniform float scale;
void main() {
	vec3 image_color = texture(image_in_sampler, texcoord).rgb;
	image_out=texture(weight_sampler, texcoord).rgb;
	//return;
	vec2 c = texelFetch(weight_sampler, ivec2(gl_FragCoord.xy), 0).rb;
	float r = texelFetchOffset(weight_sampler, ivec2(gl_FragCoord.xy), 0, ivec2(1.0, 0.0)).a;
	float b = texelFetchOffset(weight_sampler, ivec2(gl_FragCoord.xy), 0, ivec2(0.0, -1.0)).g;
	vec4 a = vec4(c.r, b, c.g, r);
	vec4 w = a * a * a;
	float tot = dot(w, vec4(1.0));
	if (tot > 0.0) {
		vec4 o = a / vec2(textureSize(image_in_sampler, 0)).yyxx;
		vec3 color = vec3(0.0);
		color = texture(image_in_sampler, texcoord + vec2(0.0, o.r)).rgb*vec3(w.r)+color;
		color = texture(image_in_sampler, texcoord + vec2(0.0, -o.g)).rgb*vec3(w.g)+color;
		color = texture(image_in_sampler, texcoord + vec2(-o.b, 0.0)).rgb*vec3(w.b)+color;
		color = texture(image_in_sampler, texcoord + vec2(o.a, 0.0)).rgb*vec3(w.a)+color;
		image_color = color / tot;
	}
	image_out = image_color;
}

