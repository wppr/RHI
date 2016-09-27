
in vec2 texcoord;

uniform sampler2D colortex;
uniform sampler2D depthtex;
uniform vec2 BlurDirection;
uniform float sssWidth;
out vec4 out_color;
/**
 * SSSS_FOV must be set to the value used to render the scene.
 */

#define SSSS_FOVY 45.0

/**
 * Light diffusion should occur on the surface of the object, not in a screen 
 * oriented plane. Setting SSSS_FOLLOW_SURFACE to 1 will ensure that diffusion
 * is more accurately calculated, at the expense of more memory accesses.
 */
#define SSSS_FOLLOW_SURFACE 0

/**
 * This define allows to specify a different source for the SSS strength
 * (instead of using the alpha channel of the color framebuffer). This is
 * useful when the alpha channel of the mian color buffer is used for something
 * else.
 */



#define SSSS_N_SAMPLES 17
vec4 kernel[] = vec4[](
    vec4(0.536343, 0.624624, 0.748867, 0),
    vec4(0.00317394, 0.000134823, 3.77269e-005, -2),
    vec4(0.0100386, 0.000914679, 0.000275702, -1.53125),
    vec4(0.0144609, 0.00317269, 0.00106399, -1.125),
    vec4(0.0216301, 0.00794618, 0.00376991, -0.78125),
    vec4(0.0347317, 0.0151085, 0.00871983, -0.5),
    vec4(0.0571056, 0.0287432, 0.0172844, -0.28125),
    vec4(0.0582416, 0.0659959, 0.0411329, -0.125),
    vec4(0.0324462, 0.0656718, 0.0532821, -0.03125),
    vec4(0.0324462, 0.0656718, 0.0532821, 0.03125),
    vec4(0.0582416, 0.0659959, 0.0411329, 0.125),
    vec4(0.0571056, 0.0287432, 0.0172844, 0.28125),
    vec4(0.0347317, 0.0151085, 0.00871983, 0.5),
    vec4(0.0216301, 0.00794618, 0.00376991, 0.78125),
    vec4(0.0144609, 0.00317269, 0.00106399, 1.125),
    vec4(0.0100386, 0.000914679, 0.000275702, 1.53125),
    vec4(0.00317394, 0.000134823, 3.77269e-005, 2)
);

vec4 SSSSBlurPS(
        /**
         * The usual quad texture coordinates.
         */
        vec2 texcoord,

        /**
         * This is a SRGB or HDR color input buffer, which should be the final
         * color frame, resolved in case of using multisampling. The desired
         * SSS strength should be stored in the alpha channel (1 for full
         * strength, 0 for disabling SSS). If this is not possible, you an
         * customize the source of this value using SSSS_STREGTH_SOURCE.
         *
         * When using non-SRGB buffers, you
         * should convert to linear before processing, and back again to gamma
         * space before storing the pixels (see Chapter 24 of GPU Gems 3 for
         * more info)
         *
         * IMPORTANT: WORKING IN A NON-LINEAR SPACE WILL TOTALLY RUIN SSS!
         */
        sampler2D colorTex,

        /**
         * The linear depth buffer of the scene, resolved in case of using
         * multisampling. The resolve should be a simple average to avoid
         * artifacts in the silhouette of objects.
         */
        sampler2D depthTex,

        /**
         * This parameter specifies the global level of subsurface scattering
         * or, in other words, the width of the filter. It's specified in
         * world space units.
         */
        float sssWidth,

        /**
         * Direction of the blur:
         *   - First pass:   vec2(1.0, 0.0)
         *   - Second pass:  vec2(0.0, 1.0)
         */
        vec2 dir,

        /**
         * This parameter indicates whether the stencil buffer should be
         * initialized. Should be set to 'true' for the first pass if not
         * previously initialized, to enable optimization of the second
         * pass.
         */
        bool initStencil) {

    // Fetch color of current pixel:
    vec4 colorM = texture(colorTex, texcoord);

    // Initialize the stencil buffer in case it was not already available:
    if (initStencil) // (Checked in compile time, it's optimized away)
        if (colorM.a == 0.0) discard;

    // Fetch linear depth of current pixel:
    float depthM = texture(depthTex, texcoord).r;

    // Calculate the sssWidth scale (1.0 for a unit plane sitting on the
    // projection window):
    float distanceToProjectionWindow = 1.0 / tan(0.5 * radians(SSSS_FOVY));
    float scale = distanceToProjectionWindow / depthM;

    // Calculate the final step to fetch the surrounding pixels:
    vec2 finalStep = sssWidth * scale * dir;
    finalStep *= colorM.a; // Modulate it using the alpha channel.
    finalStep *= 1.0 / 3.0; // Divide by 3 as the kernels range from -3 to 3.

    // Accumulate the center sample:
    vec4 colorBlurred = colorM;
    colorBlurred.rgb *= kernel[0].rgb;

    // Accumulate the other samples:

    for (int i = 1; i < SSSS_N_SAMPLES; i++) {
        // Fetch color and depth for current sample:
        vec2 offset = texcoord + kernel[i].a * finalStep;
        vec4 color = texture(colorTex, offset);

        #if SSSS_FOLLOW_SURFACE == 1
        // If the difference in depth is huge, we lerp color back to "colorM":
        float depth = texture(depthTex, offset).r;
        float s = clamp(300.0f * distanceToProjectionWindow *
                               sssWidth * abs(depthM - depth),0.0,1.0);
        color.rgb = mix(color.rgb, colorM.rgb, s);
        #endif

        // Accumulate:
        colorBlurred.rgb += kernel[i].rgb * color.rgb;
    }

    return colorBlurred;
}
void main(){
	out_color=SSSSBlurPS(texcoord,colortex,depthtex,sssWidth,BlurDirection,false);

}