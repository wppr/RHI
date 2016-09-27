
in vec2 texcoord;
out vec4 image_acc_blur;

uniform sampler2D image_acc;


void main() {
	vec2 unit = 0.666666667 / vec2(textureSize(image_acc, 0));

	float sum = 0.0;
	
	sum += texture(image_acc, texcoord + vec2(-unit.x, -unit.y)).x;
	sum += texture(image_acc, texcoord + vec2( unit.x, -unit.y)).x;
	sum += texture(image_acc, texcoord + vec2(-unit.x,  unit.y)).x;
	sum += texture(image_acc, texcoord + vec2( unit.x,  unit.y)).x;


	image_acc_blur = vec4(vec3(0.25 * sum),1.0);
}