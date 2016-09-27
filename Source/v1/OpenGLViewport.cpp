// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	OpenGLViewport.cpp: OpenGL viewport RHI implementation.
=============================================================================*/

#include "OpenGLDrvPrivate.h"
#include <algorithm>

/*=============================================================================
 *	The following RHI functions must be called from the main thread.
 *=============================================================================*/
FViewportRHIRef FOpenGLDynamicRHI::RHICreateViewport(void* WindowHandle,uint32 SizeX,uint32 SizeY,bool bIsFullscreen,EPixelFormat PreferredPixelFormat)
{

	// Use a default pixel format if none was specified	
	if (PreferredPixelFormat == EPixelFormat::PF_Unknown)
	{
		PreferredPixelFormat = EPixelFormat::PF_B8G8R8A8;
	}

	return FViewportRHIRef(new FOpenGLViewport(this,WindowHandle,SizeX,SizeY,bIsFullscreen,PreferredPixelFormat));
}

void FOpenGLDynamicRHI::RHIResizeViewport(FViewportRHIParamRef ViewportRHI,uint32 SizeX,uint32 SizeY,bool bIsFullscreen)
{
	FOpenGLViewport* Viewport = ResourceCast(ViewportRHI);

	Viewport->Resize(SizeX,SizeY,bIsFullscreen);
}



FOpenGLViewport::FOpenGLViewport(FOpenGLDynamicRHI* InOpenGLRHI,void* InWindowHandle,uint32 InSizeX,uint32 InSizeY,bool bInIsFullscreen,EPixelFormat PreferredPixelFormat)
	: OpenGLRHI(InOpenGLRHI)
	, OpenGLContext(NULL)
	, SizeX(0)
	, SizeY(0)
	, bIsFullscreen(false)
	, PixelFormat(PreferredPixelFormat)
	, bIsValid(true)
	, FrameSyncEvent(InOpenGLRHI)
{

	OpenGLRHI->Viewports.push_back(this);
	OpenGLContext = PlatformCreateOpenGLContext(OpenGLRHI->PlatformDevice, InWindowHandle);
	Resize(InSizeX, InSizeY, bInIsFullscreen);
}

FOpenGLViewport::~FOpenGLViewport()
{

	if (bIsFullscreen)
	{
		PlatformRestoreDesktopDisplayMode();
	}


	// Release back buffer, before OpenGL context becomes invalid, making it impossible
	BackBuffer.SafeRelease();

	PlatformDestroyOpenGLContext(OpenGLRHI->PlatformDevice,OpenGLContext);
	OpenGLContext = NULL;
	std::remove(OpenGLRHI->Viewports.begin(), OpenGLRHI->Viewports.end(), this);
}

void FOpenGLViewport::Resize(uint32 InSizeX,uint32 InSizeY,bool bInIsFullscreen)
{
	if ((InSizeX == SizeX) && (InSizeY == SizeY) && (bInIsFullscreen == bIsFullscreen))
	{
		return;
	}

	VERIFY_GL_SCOPE();

	////if (IsValidRef(CustomPresent))
	////{
	////	CustomPresent->OnBackBufferResize();
	////}

	//BackBuffer.SafeRelease();	// when the rest of the engine releases it, its framebuffers will be released too (those the engine knows about)

	//BackBuffer = (FOpenGLTexture2D*)PlatformCreateBuiltinBackBuffer(OpenGLRHI, InSizeX, InSizeY);
	//if (!BackBuffer)
	//{
	//	BackBuffer = (FOpenGLTexture2D*)OpenGLRHI->CreateOpenGLTexture(InSizeX, InSizeY, false, false, PixelFormat, 1, 1, 1, TexCreate_RenderTargetable, FClearValueBinding::Transparent);
	//}

	PlatformResizeGLContext(OpenGLRHI->PlatformDevice, OpenGLContext, InSizeX, InSizeY, bInIsFullscreen, bIsFullscreen, BackBuffer->Target, BackBuffer->Resource);

	SizeX = InSizeX;
	SizeY = InSizeY;
	bIsFullscreen = bInIsFullscreen;
}

void* FOpenGLViewport::GetNativeWindow(void** AddParam) const
{
	return PlatformGetWindow(OpenGLContext, AddParam);
}

