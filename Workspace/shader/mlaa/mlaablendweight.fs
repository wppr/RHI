
in vec2 texcoord;

out vec4 weight;

uniform sampler2D edge_sampler;
uniform sampler2D area_sampler;
uniform sampler2D edge_sampler_N;

uniform int max_search_steps;
uniform int max_distance;

float search_x_left(vec2 o, vec2 texel_size) {
	o -= vec2(1.5, 0.0) * texel_size;
	int i = 0;
	float e = 0.0;
	for (; i < max_search_steps; ++i) {
		e = texture(edge_sampler, o).g;
		if (e < 0.9) break;
		o -= vec2(2.0, 0.0) * texel_size;
	}
	return -2.0 * min(float(i) + e, float(max_search_steps));
}

float search_x_right(vec2 o, vec2 texel_size) {
	o += vec2(1.5, 0.0) * texel_size;
	int i = 0;
	float e = 0.0;
	for (; i < max_search_steps; ++i) {
		e = texture(edge_sampler, o).g;
		if (e < 0.9) break;
		o += vec2(2.0, 0.0) * texel_size;
	}
	return 2.0 * min(float(i) + e, float(max_search_steps));
}

float search_y_up(vec2 o, vec2 texel_size) {
	o += vec2(0.0, 1.5) * texel_size;
	int i = 0;
	float e = 0.0;
	for (; i < max_search_steps; ++i) {
		e = texture(edge_sampler, o).r;
		if (e < 0.9) break;
		o += vec2(0.0, 2.0) * texel_size;
	}
	return 2.0 * min(float(i) + e, float(max_search_steps));
}

float search_y_down(vec2 o, vec2 texel_size) {
	o -= vec2(0.0, 1.5) * texel_size;
	int i = 0;
	float e = 0.0;
	for (; i < max_search_steps; ++i) {
		e = texture(edge_sampler, o).r;
		if (e < 0.9) break;
		o -= vec2(0.0, 2.0) * texel_size;
	}
	return -2.0 * min(float(i) + e, float(max_search_steps));
}

vec2 area(vec2 d, vec2 ce) {
	float texel_size = 0.2 / (float(max_distance) + 1.0);
	vec2 base = round(4.0 * ce) * (float(max_distance) + 1.0);
	vec2 pos = (base + d + vec2(0.5)) * texel_size;
	return texture(area_sampler, vec2(pos.x, 1.0 - pos.y)).rg;
}

// vec2 area2(vec2 d,vec2 ce){
// 	float areaSize = float(max_distance) * 5.0;
//     vec2 pixcoord = float(max_distance) * round(4.0 * ce) + d;
//     vec2 texcoord = pixcoord / (areaSize - 1.0);
//     return textureLod(area_sampler,vec2(texcoord.x, 1.0 - texcoord.y), 0.0).rg;
// }
void main() {
	vec2 e = texture(edge_sampler_N, texcoord).rg;
	vec2 texel_size = 1.0 / vec2(textureSize(edge_sampler, 0));
	weight = vec4(0.0);
	if (e.g > 0.0) {
		vec2 d = vec2(search_x_left(texcoord, texel_size), search_x_right(texcoord, texel_size));
		vec4 pos = vec4(d.x, 0.25, d.y + 1.0, 0.25)*texel_size.xyxy+texcoord.xyxy;
		float ce0 = texture(edge_sampler, pos.xy).r;
		float ce1 = texture(edge_sampler, pos.zw).r;
		weight.rg = area(abs(d), vec2(ce0, ce1));
	}
	if (e.r > 0.0) {
		vec2 d = vec2(search_y_down(texcoord, texel_size), search_y_up(texcoord, texel_size)).yx;
		vec4 pos = vec4(-0.25, d.x, -0.25, d.y - 1.0)*texel_size.xyxy+texcoord.xyxy;
		float ce0 = texture(edge_sampler, pos.xy).g;
		float ce1 = texture(edge_sampler, pos.zw).g;
		weight.ba = area(abs(d), vec2(ce0, ce1));

	}

	//weight=texture(edge_sampler_N, texcoord);
}