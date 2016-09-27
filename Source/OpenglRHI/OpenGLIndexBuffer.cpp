// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	OpenGLIndexBuffer.cpp: OpenGL Index buffer RHI implementation.
=============================================================================*/

#include "OpenGLDrvPrivate.h"

FIndexBufferRHIRef FOpenGLDynamicRHI::RHICreateIndexBuffer(uint32 Stride,uint32 Size, uint32 InUsage, FRHIResourceCreateInfo& CreateInfo)
{
	VERIFY_GL_SCOPE();

	const void *Data = NULL;

	// If a resource array was provided for the resource, create the resource pre-populated
	if(CreateInfo.ResourceArray)
	{
		Data = CreateInfo.ResourceArray;
	}

	FIndexBufferRHIRef IndexBuffer(new FOpenGLIndexBuffer(Stride, Size, InUsage, Data));
	return IndexBuffer;
}

