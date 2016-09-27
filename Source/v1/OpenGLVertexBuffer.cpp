// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	OpenGLVertexBuffer.cpp: OpenGL vertex buffer RHI implementation.
=============================================================================*/

#include "OpenGLDrvPrivate.h"



FVertexBufferRHIRef FOpenGLDynamicRHI::RHICreateVertexBuffer(uint32 Size, uint32 InUsage, FRHIResourceCreateInfo& CreateInfo)
{
	VERIFY_GL_SCOPE();

	const void *Data = NULL;

	// If a resource array was provided for the resource, create the resource pre-populated
	if(CreateInfo.ResourceArray)
	{
		Data = CreateInfo.ResourceArray;
	}

	FVertexBufferRHIRef VertexBuffer(new FOpenGLVertexBuffer(0, Size, InUsage, Data));
	return VertexBuffer;
}
