
in vec2 texcoord;

out vec3 edges;
uniform sampler2D image_sampler;
uniform sampler2D depth_sampler;

void main() {
	const vec3 lum = vec3(0.2126, 0.7152, 0.0722);
	const float image_threshold = 0.1;
	//const float depth_threshold = 0.000003;
	float depth_threshold=0.01;
	float c = texture(depth_sampler, texcoord).r;
	float l = textureOffset(depth_sampler, texcoord, ivec2(-1, 0)).r;
	float t = textureOffset(depth_sampler, texcoord, ivec2(0, 1)).r;
	vec2 edge = step(depth_threshold, abs(vec2(l, t) - vec2(c)));

	//if (dot(edge, vec2(1.0)) <depth_threshold) {
		c = dot(texture(image_sampler, texcoord).rgb, lum);
		l = dot(textureOffset(image_sampler, texcoord, ivec2(-1, 0)).rgb, lum);
		t = dot(textureOffset(image_sampler, texcoord, ivec2(0, 1)).rgb, lum);
		edge = step(vec2(image_threshold), abs(vec2(l, t) - vec2(c)));
	//}

	edges = vec3(edge,0.0);

}

