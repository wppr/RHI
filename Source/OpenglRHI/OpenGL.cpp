#include "OpenGL.h"
#include "OpenGLDrvPrivate.h"

GLint FOpenGLBase::MaxTextureImageUnits = -1;
GLint FOpenGLBase::MaxCombinedTextureImageUnits = -1;
GLint FOpenGLBase::MaxVertexTextureImageUnits = -1;
GLint FOpenGLBase::MaxGeometryTextureImageUnits = -1;
GLint FOpenGLBase::MaxHullTextureImageUnits = -1;
GLint FOpenGLBase::MaxDomainTextureImageUnits = -1;
GLint FOpenGLBase::MaxVaryingVectors = -1;
GLint FOpenGLBase::MaxVertexUniformComponents = -1;
GLint FOpenGLBase::MaxPixelUniformComponents = -1;
GLint FOpenGLBase::MaxGeometryUniformComponents = -1;
GLint FOpenGLBase::MaxHullUniformComponents = -1;
GLint FOpenGLBase::MaxDomainUniformComponents = -1;
bool  FOpenGLBase::bSupportsASTC = false;
bool  FOpenGLBase::bSupportsCopyImage = false;
bool  FOpenGLBase::bSupportsSeamlessCubemap = false;
bool  FOpenGLBase::bSupportsVolumeTextureRendering = false;
bool  FOpenGLBase::bSupportsTextureFilterAnisotropic = false;
bool  FOpenGLBase::bSupportsDrawBuffersBlend = false;
bool  FOpenGLBase::bAmdWorkaround = false;
void FOpenGLBase::ProcessQueryGLInt()
{
	GET_GL_INT(GL_MAX_TEXTURE_IMAGE_UNITS, 0, MaxTextureImageUnits);
	GET_GL_INT(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, 0, MaxVertexTextureImageUnits);
	GET_GL_INT(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0, MaxCombinedTextureImageUnits);
}
void FOpenGLBase::ProcessExtensions(const string& ExtensionsString)
{
	ProcessQueryGLInt();

	// For now, just allocate additional units if available and advertise no tessellation units for HW that can't handle more
	if (MaxCombinedTextureImageUnits < 48)
	{
		// To work around AMD driver limitation of 32 GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
		// Going to hard code this for now (16 units in PS, 8 units in VS, 8 units in GS).
		// This is going to be a problem for tessellation.
		MaxTextureImageUnits = MaxTextureImageUnits > 16 ? 16 : MaxTextureImageUnits;
		MaxVertexTextureImageUnits = MaxVertexTextureImageUnits > 8 ? 8 : MaxVertexTextureImageUnits;
		MaxGeometryTextureImageUnits = MaxGeometryTextureImageUnits > 8 ? 8 : MaxGeometryTextureImageUnits;
		MaxHullTextureImageUnits = 0;
		MaxDomainTextureImageUnits = 0;
		MaxCombinedTextureImageUnits = MaxCombinedTextureImageUnits > 32 ? 32 : MaxCombinedTextureImageUnits;
	}
}