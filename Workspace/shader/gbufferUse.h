

//uniform sampler2D normal2;
//uniform highp usampler2D  normal;
uniform sampler2D gbufferDepth;
uniform sampler2D gbufferNormal;

vec3 getViewPositionFromDepth(vec2 uv,mat4 proj_inv_mat){
  float depth = texture(gbufferDepth, uv).x;
  vec3 ndc = 2.0 * vec3(uv, depth) - 1.0;
  vec4 P = proj_inv_mat * vec4(ndc, 1.0); 
  P.xyz /= P.w;
  return P.xyz;
}

vec3 getWorldNormal(vec2 uv){
  vec4 Normalraw = texture(gbufferNormal, uv);
  return normalize(Normalraw.xyz);
}




float getGbufferDepth(vec2 uv){
	float depth = texture(gbufferDepth, uv).x;
	return depth;
}


// vec3 getNormal3(vec2 uv){
//   uvec2 image_packed = texelFetch(normal, ivec2(gl_FragCoord.xy), 0).xy;
//   vec4 image_unpacked = vec4(unpackHalf2x16(image_packed.x), unpackHalf2x16(image_packed.y));
//   return image_unpacked.xyz;
// }