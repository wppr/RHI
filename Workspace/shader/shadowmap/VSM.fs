
in vec4 PosView;
out vec2 FragOut;

void main() {
	//float depth = gl_FragCoord.z;
	float depth=length(PosView.xyz);
	float dx = dFdx(depth);
	float dy = dFdy(depth);
	float moment2 = depth * depth + 0.25 * (dx * dx + dy * dy);

	FragOut = vec2(depth, moment2);

}