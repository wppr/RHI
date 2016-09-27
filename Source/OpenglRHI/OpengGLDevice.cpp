#include "OPENGL_Include.h"
#include "OpenGLDrvPrivate.h"
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;
#include "OpenGLES31.h"
static void printGLString(const char *name, GLenum s) {
	const char *v = (const char *)glGetString(s);
#ifdef ANDROID
	LOGI("%s", v);
#else
	cout <<endl<<name<<" "<< v <<endl;
#endif
}


static void printGLExtension() {
	const char *v = (const char *)glGetString(GL_EXTENSIONS);
	string s(v);
	replace(s.begin(), s.end(), ' ', '\n');
#ifdef ANDROID
	LOGI("%s", s.c_str());
#else
	cout <<endl<<"Extensions: "<<endl<< s << endl;
#endif
}
static void LogOpenglInfo() {
	printGLString("Version", GL_VERSION);
	printGLString("Vendor", GL_VENDOR);
	printGLString("Renderer", GL_RENDERER);
	printGLExtension();
}
static void InitGLFormat();
void FOpenGLDynamicRHI::Init()
{

	LogOpenglInfo();
	InitGLFormat();
	
	const string e;
	FOpenGL::ProcessExtensions(e);

	InitializeStateResources();
	//init use GLFW
}

void FOpenGLDynamicRHI::Shutdown()
{
	//not implement
}

FOpenGLContextState& FOpenGLDynamicRHI::GetContextStateForCurrentContext()
{
	return RenderingContextState;

}

FOpenGLTextureFormat GOpenGLTextureFormats[PF_MAX];
extern FPixelFormatInfo	GPixelFormats[PF_MAX];
bool GDisableOpenGLDebugOutput = false;
static inline void SetupTextureFormat(EPixelFormat Format, const FOpenGLTextureFormat& GLFormat)
{
	GOpenGLTextureFormats[Format] = GLFormat;
	GPixelFormats[Format].Supported = (GLFormat.Format != GL_NONE && (GLFormat.InternalFormat[0] != GL_NONE || GLFormat.InternalFormat[1] != GL_NONE));
}

static void InitGLFormat() {
	for (int32 PF = 0; PF < PF_MAX; ++PF)
	{
		SetupTextureFormat(EPixelFormat(PF), FOpenGLTextureFormat());
	}

	GLenum DepthFormat = FOpenGL::GetDepthFormat();
	GLenum ShadowDepthFormat = FOpenGL::GetShadowDepthFormat();

	// Initialize the platform pixel format map.					InternalFormat				InternalFormatSRGB		Format				Type							bCompressed		bBGRA
	SetupTextureFormat(PF_Unknown, FOpenGLTextureFormat());
	SetupTextureFormat(PF_A32B32G32R32F, FOpenGLTextureFormat(GL_RGBA32F, GL_NONE, GL_RGBA, GL_FLOAT, false, false));
	SetupTextureFormat(PF_UYVY, FOpenGLTextureFormat());
	//@todo: ES2 requires GL_OES_depth_texture extension to support depth textures of any kind.
	SetupTextureFormat(PF_ShadowDepth, FOpenGLTextureFormat(ShadowDepthFormat, GL_NONE, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, false, false));
	SetupTextureFormat(PF_D24, FOpenGLTextureFormat(DepthFormat, GL_NONE, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, false, false));
	SetupTextureFormat(PF_A16B16G16R16, FOpenGLTextureFormat(GL_RGBA16, GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT, false, false));
	SetupTextureFormat(PF_A1, FOpenGLTextureFormat());
	SetupTextureFormat(PF_R16G16B16A16_UINT, FOpenGLTextureFormat(GL_RGBA16UI, GL_NONE, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, false, false));
	SetupTextureFormat(PF_R16G16B16A16_SINT, FOpenGLTextureFormat(GL_RGBA16I, GL_NONE, GL_RGBA_INTEGER, GL_SHORT, false, false));
	SetupTextureFormat(PF_R5G6B5_UNORM, FOpenGLTextureFormat());


	{
		// Not supported for rendering:
		SetupTextureFormat(PF_G16, FOpenGLTextureFormat(GL_R16, GL_R16, GL_RED, GL_UNSIGNED_SHORT, false, false));
		SetupTextureFormat(PF_R32_FLOAT, FOpenGLTextureFormat(GL_R32F, GL_R32F, GL_RED, GL_FLOAT, false, false));
		SetupTextureFormat(PF_G16R16F, FOpenGLTextureFormat(GL_RG16F, GL_RG16F, GL_RG, GL_HALF_FLOAT, false, false));
		SetupTextureFormat(PF_G16R16F_FILTER, FOpenGLTextureFormat(GL_RG16F, GL_RG16F, GL_RG, GL_HALF_FLOAT, false, false));
		SetupTextureFormat(PF_G32R32F, FOpenGLTextureFormat(GL_RG32F, GL_RG32F, GL_RG, GL_FLOAT, false, false));
		SetupTextureFormat(PF_A2B10G10R10, FOpenGLTextureFormat(GL_RGB10_A2, GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV, false, false));
		SetupTextureFormat(PF_R16F, FOpenGLTextureFormat(GL_R16F, GL_R16F, GL_RED, GL_HALF_FLOAT, false, false));
		SetupTextureFormat(PF_R16F_FILTER, FOpenGLTextureFormat(GL_R16F, GL_R16F, GL_RED, GL_HALF_FLOAT, false, false));
		if (FOpenGL::SupportsR11G11B10F())
		{
			// Note: Also needs to include support for compute shaders to be defined here (e.g. glBindImageTexture)
			SetupTextureFormat(PF_FloatRGB, FOpenGLTextureFormat(GL_R11F_G11F_B10F, GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, false, false));
			SetupTextureFormat(PF_FloatR11G11B10, FOpenGLTextureFormat(GL_RGBA16F, GL_RGBA16F, GL_RGB, GL_HALF_FLOAT, false, false));
		}
		else
		{
			SetupTextureFormat(PF_FloatRGB, FOpenGLTextureFormat(GL_RGBA16F, GL_RGBA16F, GL_RGB, GL_HALF_FLOAT, false, false));
			SetupTextureFormat(PF_FloatR11G11B10, FOpenGLTextureFormat(GL_R11F_G11F_B10F, GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, false, false));
		}
		SetupTextureFormat(PF_V8U8, FOpenGLTextureFormat(GL_RG8_SNORM, GL_NONE, GL_RG, GL_BYTE, false, false));
		SetupTextureFormat(PF_R8G8, FOpenGLTextureFormat(GL_RG8, GL_NONE, GL_RG, GL_UNSIGNED_BYTE, false, false));
		SetupTextureFormat(PF_BC5, FOpenGLTextureFormat(GL_COMPRESSED_RG_RGTC2, GL_COMPRESSED_RG_RGTC2, GL_RG, GL_UNSIGNED_BYTE, true, false));
		SetupTextureFormat(PF_BC4, FOpenGLTextureFormat(GL_COMPRESSED_RED_RGTC1, GL_COMPRESSED_RED_RGTC1, GL_RED, GL_UNSIGNED_BYTE, true, false));
		SetupTextureFormat(PF_A8, FOpenGLTextureFormat(GL_R8, GL_NONE, GL_RED, GL_UNSIGNED_BYTE, false, false));
		SetupTextureFormat(PF_R32_UINT, FOpenGLTextureFormat(GL_R32UI, GL_NONE, GL_RED_INTEGER, GL_UNSIGNED_INT, false, false));
		SetupTextureFormat(PF_R32_SINT, FOpenGLTextureFormat(GL_R32I, GL_NONE, GL_RED_INTEGER, GL_INT, false, false));
		SetupTextureFormat(PF_R16_UINT, FOpenGLTextureFormat(GL_R16UI, GL_NONE, GL_RED_INTEGER, GL_UNSIGNED_SHORT, false, false));
		SetupTextureFormat(PF_R16_SINT, FOpenGLTextureFormat(GL_R16I, GL_NONE, GL_RED_INTEGER, GL_SHORT, false, false));
		SetupTextureFormat(PF_FloatRGBA, FOpenGLTextureFormat(GL_RGBA16F, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, false, false));

		SetupTextureFormat(PF_G8, FOpenGLTextureFormat(GL_R8, GL_R8, GL_RED, GL_UNSIGNED_BYTE, false, false));
		SetupTextureFormat(PF_B8G8R8A8, FOpenGLTextureFormat(GL_RGBA8, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE, false, true));
		SetupTextureFormat(PF_R8G8B8A8, FOpenGLTextureFormat(GL_RGBA8, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE, false, false));
		SetupTextureFormat(PF_R8G8B8, FOpenGLTextureFormat(GL_RGB8, GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE, false, false));
		if (FOpenGL::SupportsRG16UI())
		{
			// The user should check for support for PF_G16R16 and implement a fallback if it's not supported!
			SetupTextureFormat(PF_G16R16, FOpenGLTextureFormat(GL_RG16, GL_RG16, GL_RG, GL_UNSIGNED_SHORT, false, false));
		}

		{
			SetupTextureFormat(PF_G8, FOpenGLTextureFormat(GL_R8, GL_SRGB8, GL_RED, GL_UNSIGNED_BYTE, false, false));
			//SetupTextureFormat(PF_B8G8R8A8, FOpenGLTextureFormat(GL_RGBA8, GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, false, false));
//			SetupTextureFormat(PF_R8G8B8A8, FOpenGLTextureFormat(GL_RGBA8, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, false, false));
			SetupTextureFormat(PF_G16R16, FOpenGLTextureFormat(GL_RG16, GL_RG16, GL_RG, GL_UNSIGNED_SHORT, false, false));
		}
		if (FOpenGL::SupportsPackedDepthStencil())
		{
			SetupTextureFormat(PF_DepthStencil, FOpenGLTextureFormat(GL_DEPTH24_STENCIL8, GL_NONE, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, false, false));
		}
		else
		{
			// @todo android: This is cheating by not setting a stencil anywhere, need that! And Shield is still rendering black scene
			SetupTextureFormat(PF_DepthStencil, FOpenGLTextureFormat(DepthFormat, GL_NONE, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, false, false));
		}
	}

	{

		// ES2-based cases
		GLuint BGRA8888 =  GL_RGBA;
		GLuint RGBA8 =  GL_RGBA;


		//SetupTextureFormat(PF_B8G8R8A8, FOpenGLTextureFormat(GL_BGRA, FOpenGL::SupportsSRGB() ? GL_SRGB_ALPHA_EXT : GL_BGRA, BGRA8888, GL_UNSIGNED_BYTE, false, false));

		//SetupTextureFormat(PF_B8G8R8A8, FOpenGLTextureFormat(GL_RGBA, FOpenGL::SupportsSRGB() ? GL_SRGB_ALPHA_EXT : GL_RGBA, GL_BGRA8_EXT, FOpenGL::SupportsSRGB() ? GL_SRGB8_ALPHA8_EXT : GL_BGRA8_EXT, BGRA8888, GL_UNSIGNED_BYTE, false, false));

		SetupTextureFormat(PF_R8G8B8A8, FOpenGLTextureFormat(RGBA8, FOpenGL::SupportsSRGB() ? GL_SRGB_ALPHA_EXT : RGBA8, GL_RGBA8, FOpenGL::SupportsSRGB() ? GL_SRGB8_ALPHA8_EXT : GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, false, false));

		SetupTextureFormat(PF_G8, FOpenGLTextureFormat(GL_LUMINANCE, GL_LUMINANCE, GL_LUMINANCE, GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE, false, false));
		SetupTextureFormat(PF_A8, FOpenGLTextureFormat(GL_ALPHA, GL_ALPHA, GL_ALPHA, GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE, false, false));
		{

			SetupTextureFormat(PF_FloatRGBA, FOpenGLTextureFormat(GL_RGBA, GL_RGBA, GL_RGBA, GL_HALF_FLOAT, false, false));

		}
		{
			SetupTextureFormat(PF_FloatRGBA, FOpenGLTextureFormat(GL_RGBA, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, false, false));
		}

		{
			// @todo android: This is cheating by not setting a stencil anywhere, need that! And Shield is still rendering black scene
			SetupTextureFormat(PF_DepthStencil, FOpenGLTextureFormat(GL_DEPTH_COMPONENT, GL_NONE, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, false, false));
		}

	}

}