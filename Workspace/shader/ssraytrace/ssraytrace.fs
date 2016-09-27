#version 300 es
precision high float;

in vec3 msPosition;
in vec3 csNormal;
in vec3 csPosition;
in vec2 texcoord;
out vec4 frag;

uniform sampler2D color;
uniform sampler2D depth;
uniform vec3 clipInfo;
uniform mat4 ProjMatrix;
uniform mat4 ViewMatrix;
uniform mat4 WorldMatrix;	
uniform samplerCube	env_map_sampler;
float reconstructCSZ(float depthBufferValue, vec3 c) {
      return c[0] / (depthBufferValue * c[1] + c[2]);
}
void swap(inout float a, inout float b) {
     float temp = a;
     a = b;
     b = temp;
}
float distanceSquared(vec2 a, vec2 b) {
    a -= b;
    return dot(a, a);
}
vec2 getFragcoord(vec2 size){
	return gl_FragCoord.xy/size;
}
/**
    \param csOrigin Camera-space ray origin, which must be 
    within the view volume and must have z < -0.01 and project within the valid screen rectangle

    \param csDirection Unit length camera-space ray direction

    \param projectToPixelMatrix A projection matrix that maps to pixel coordinates (not [-1, +1] normalized device coordinates)

    \param csZBuffer The depth or camera-space Z buffer, depending on the value of \a csZBufferIsHyperbolic

    \param csZBufferSize Dimensions of csZBuffer

    \param csZThickness Camera space thickness to ascribe to each pixel in the depth buffer
    
    \param csZBufferIsHyperbolic True if csZBuffer is an OpenGL depth buffer, false (faster) if
     csZBuffer contains (negative) "linear" camera space z values. Const so that the compiler can evaluate the branch based on it at compile time

    \param clipInfo See G3D::Camera documentation

    \param nearPlaneZ Negative number

    \param stride Step in horizontal or vertical pixels between samples. This is a float
     because integer math is slow on GPUs, but should be set to an integer >= 1

    \param jitterFraction  Number between 0 and 1 for how far to bump the ray in stride units
      to conceal banding artifacts

    \param maxSteps Maximum number of iterations. Higher gives better images but may be slow

    \param maxRayTraceDistance Maximum camera-space distance to trace before returning a miss

    \param hitPixel Pixel coordinates of the first intersection with the scene

    \param csHitPoint Camera space location of the ray hit

    Single-layer

 */
vec4 out_test;

bool traceScreenSpaceRay1
   (vec3          csOrigin, 
    vec3         csDirection,
    mat4          projectToPixelMatrix,
    sampler2D       csZBuffer,
    vec2          csZBufferSize,
    float           csZThickness,
    bool   		csZBufferIsHyperbolic,
    vec3          clipInfo,
    float           nearPlaneZ,
    float			stride,
    float           jitterFraction,
    float           maxSteps,
    float        	maxRayTraceDistance,
    out vec2      hitPixel,
    out int         which,
	out vec3		csHitPoint) {
    
    // Clip ray to a near plane in 3D (doesn't have to be *the* near plane, although that would be a good idea)
    float rayLength = ((csOrigin.z + csDirection.z * maxRayTraceDistance) > nearPlaneZ) ?
                        (nearPlaneZ - csOrigin.z) / csDirection.z :
                        maxRayTraceDistance;
	vec3 csEndPoint = csDirection * rayLength + csOrigin;

    // Project into screen space
    vec4 H0 = projectToPixelMatrix * vec4(csOrigin, 1.0);
    vec4 H1 = projectToPixelMatrix * vec4(csEndPoint, 1.0);

    // There are a lot of divisions by w that can be turned into multiplications
    // at some minor precision loss...and we need to interpolate these 1/w values
    // anyway.
    //
    // Because the caller was required to clip to the near plane,
    // this homogeneous division (projecting from 4D to 2D) is guaranteed 
    // to succeed. 
    float k0 = 1.0 / H0.w;
    float k1 = 1.0 / H1.w;

    // Switch the original points to values that interpolate linearly in 2D
    vec3 Q0 = csOrigin * k0; 
    vec3 Q1 = csEndPoint * k1;

	// Screen-space endpoints
    vec2 P0 = H0.xy * k0;
    vec2 P1 = H1.xy * k1;
    // [Optional clipping to frustum sides here]

    // Initialize to off screen
    hitPixel = vec2(-1.0, -1.0);
    which = 0; // Only one layer

    // If the line is degenerate, make it cover at least one pixel
    // to avoid handling zero-pixel extent as a special case later
    P1 += vec2((distanceSquared(P0, P1) < 0.0001) ? 0.01 : 0.0);

    vec2 delta = P1 - P0;
// out_test=vec4(delta.xy*0.5+0.5,0,1);
    // out_test=texture(color,P0/csZBufferSize);
    // out_test=vec4(rayLength/2000);
    
     vec2 tmp=(P1.xy)/csZBufferSize;
     out_test=vec4(0);
     if(P1.x>500)
     	out_test=vec4(1.0,0,0.0,1.0);

     // out_test=vec4(tmp.xxx,1);
	//out_test=vec4(abs(csDirection.xxx),1);

    // Permute so that the primary iteration is in x to reduce
    // large branches later
    bool permute = false;
	if (abs(delta.x) < abs(delta.y)) {
		// More-vertical line. Create a permutation that swaps x and y in the output
		permute = true;

        // Directly swizzle the inputs
		delta = delta.yx;
		P1 = P1.yx;
		P0 = P0.yx;        
	}
    
	// From now on, "x" is the primary iteration direction and "y" is the secondary one

    float stepDirection = sign(delta.x);
    float invdx = stepDirection / delta.x;
    vec2 dP = vec2(stepDirection, invdx * delta.y);
    // Track the derivatives of Q and k
    vec3 dQ = (Q1 - Q0) * invdx;
    float   dk = (k1 - k0) * invdx;

    // Scale derivatives by the desired pixel stride
	dP *= stride; dQ *= stride; dk *= stride;

    // Offset the starting values by the jitter fraction
	P0 += dP * jitterFraction; Q0 += dQ * jitterFraction; k0 += dk * jitterFraction;

	// Slide P from P0 to P1, (now-homogeneous) Q from Q0 to Q1, and k from k0 to k1
    vec3 Q = Q0;
    float  k = k0;

	// We track the ray depth at +/- 1/2 pixel to treat pixels as clip-space solid 
	// voxels. Because the depth at -1/2 for a given pixel will be the same as at 
	// +1/2 for the previous iteration, we actually only have to compute one value 
	// per iteration.
	float prevZMaxEstimate = csOrigin.z;
    float stepCount = 0.0;
    float rayZMax = prevZMaxEstimate, rayZMin = prevZMaxEstimate;
    float sceneZMax = rayZMax + 1e4;

    // P1.x is never modified after this point, so pre-scale it by 
    // the step direction for a signed comparison
    float end = P1.x * stepDirection;

    // We only advance the z field of Q in the inner loop, since
    // Q.xy is never used until after the loop terminates.

	for (vec2 P = P0;
        ((P.x * stepDirection) <= end) && 
        (stepCount < maxSteps) &&
        ((rayZMax < sceneZMax - csZThickness) ||
            (rayZMin > sceneZMax)) &&
        (sceneZMax != 0.0);
        P += dP, Q.z += dQ.z, k += dk, stepCount += 1.0) {
                
		hitPixel = permute ? P.yx : P;
		//out_test=vec4(stepCount/maxSteps,0,0,1);
        // The depth range that the ray covers within this loop
        // iteration.  Assume that the ray is moving in increasing z
        // and swap if backwards.  Because one end of the interval is
        // shared between adjacent iterations, we track the previous
        // value and then swap as needed to ensure correct ordering
        rayZMin = prevZMaxEstimate;

        // Compute the value at 1/2 pixel into the future
        rayZMax = (dQ.z * 0.5 + Q.z) / (dk * 0.5 + k);
		prevZMaxEstimate = rayZMax;
        if (rayZMin > rayZMax) { swap(rayZMin, rayZMax); }

        // Camera-space z of the background
        sceneZMax = texelFetch(csZBuffer, ivec2(hitPixel), 0).r;

        // This compiles away when csZBufferIsHyperbolic = false
        if (csZBufferIsHyperbolic) {
            sceneZMax = reconstructCSZ(sceneZMax, clipInfo);
        }
    } // pixel on ray

    Q.xy += dQ.xy * stepCount;
	csHitPoint = Q * (1.0 / k);

    // Matches the new loop condition:
    return (rayZMax >= sceneZMax - csZThickness) && (rayZMin <= sceneZMax);
}

void main(){

	// vec4 view_pos=ViewMatrix*WorldMatrix*vec4(msPosition,1.0);
	// vec3 csPosition = view_pos.xyz/view_pos.w;

	vec3 ReflectDirection = normalize(reflect(normalize(csPosition),normalize(csNormal)));

	vec2 ZBufferSize=textureSize(depth,0);

	mat4 p=mat4(0.5*ZBufferSize.x,0,0,0,
		0,0.5*ZBufferSize.y,0,0,
		0,0,0,0,
		0.5*ZBufferSize.x,0.5*ZBufferSize.y,0,1);

	mat4 projectToPixelMatrix=p*ProjMatrix;
	
	float csZThickness=10;
	bool csZBufferIsHyperbolic=true;
	float           nearPlaneZ=-1;
    float			stride=1;
    float           jitterFraction=0.5;
    float           maxSteps=400;
    float        	maxRayTraceDistance=1000;
    vec2      hitPixel;
    int         which=0;
	vec3		csHitPoint;
	bool ok=traceScreenSpaceRay1(csPosition,
		ReflectDirection,
		projectToPixelMatrix,
		depth,
    	ZBufferSize,
    	csZThickness,
	    csZBufferIsHyperbolic,
	    clipInfo,
		nearPlaneZ,
		stride,
		jitterFraction,
		maxSteps,
		maxRayTraceDistance,
		hitPixel,
		which,
		csHitPoint);

	vec4 hitpoint=ProjMatrix*vec4(csHitPoint,1.0);
	frag=textureLod(env_map_sampler, ReflectDirection, 0.0);
	if(ok)
		frag=vec4(1.0,0,0,1);
		//frag=texelFetch(color,ivec2(hitPixel),0);
	else{
		vec4 t=texture(color,getFragcoord(ZBufferSize));
		if(length(t)>0)
			frag=t;
	}

	

	//vec4 pos=projectToPixelMatrix*vec4(csPosition,1.0);
	//frag=texture(color,getFragcoord(ZBufferSize)); //验证投影矩阵

	frag=out_test;
	//frag=vec4(abs(normalize(ReflectDirection)),1.0); //验证反射方向
	//frag=hitpoint/hitpoint.w;
	// vec4 proj_pos=ProjMatrix* vec4(csPosition,1.0);
	// vec3 Pos= proj_pos.xyz/proj_pos.w;
	// vec2 texcoord2;
	// texcoord2.x = 0.5 * (1.0 + Pos.x);
	// texcoord2.y = 0.5 * (1.0 + Pos.y);
	// //vec4 pos=ProjMatrix*vec4(csPosition,1.0);
	// frag=texture(color,texcoord2);
	

}