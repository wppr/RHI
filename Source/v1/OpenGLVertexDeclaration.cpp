// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	OpenGLVertexDeclaration.cpp: OpenGL vertex declaration RHI implementation.
=============================================================================*/

#include "OpenGLRHI.h"
#include "unreal_defination.h"
#include "RHIDefinitions.h"
#include "RHI.h"
#include "OPENGL_Include.h"
#include "OpenGLResources.h"

static FORCEINLINE void SetupGLElement(FOpenGLVertexElement& GLElement, GLenum Type, GLuint Size, bool bNormalized, bool bShouldConvertToFloat)
{
	GLElement.Type = Type;
	GLElement.Size = Size;
	GLElement.bNormalized = bNormalized;
	GLElement.bShouldConvertToFloat = bShouldConvertToFloat;
	GLElement.bValid = true;
}

/**
 * Key used to look up vertex declarations in the cache.
 */
struct FOpenGLVertexDeclarationKey
{
	/** Vertex elements in the declaration. */
	FOpenGLVertexElements VertexElements;
	/** Hash of the vertex elements. */
	uint32 Hash;

	/** Initialization constructor. */
	explicit FOpenGLVertexDeclarationKey(const FVertexDeclarationElementList& InElements)
	{
		for(int32 ElementIndex = 0;ElementIndex < InElements.size();ElementIndex++)
		{
			const FVertexElement& Element = InElements[ElementIndex];
			FOpenGLVertexElement GLElement;
			GLElement.StreamIndex = Element.StreamIndex;
			GLElement.Offset = Element.Offset;
			GLElement.Divisor = Element.bUseInstanceIndex ? 1 : 0;
			GLElement.AttributeIndex = Element.AttributeIndex;
			switch(Element.Type)
			{
				case VET_Float1:		SetupGLElement(GLElement, GL_FLOAT,			1,			false,	true); break;
				case VET_Float2:		SetupGLElement(GLElement, GL_FLOAT,			2,			false,	true); break;
				case VET_Float3:		SetupGLElement(GLElement, GL_FLOAT,			3,			false,	true); break;
				case VET_Float4:		SetupGLElement(GLElement, GL_FLOAT,			4,			false,	true); break;
				case VET_PackedNormal:	SetupGLElement(GLElement, GL_UNSIGNED_BYTE,	4,			true,	true); break;
				case VET_UByte4:		SetupGLElement(GLElement, GL_UNSIGNED_BYTE,	4,			false,	false); break;
				case VET_UByte4N:		SetupGLElement(GLElement, GL_UNSIGNED_BYTE,	4,			true,	true); break;
				//case VET_Color:	
				//	if (FOpenGL::SupportsVertexArrayBGRA())
				//	{
				//		SetupGLElement(GLElement, GL_UNSIGNED_BYTE,	GL_BGRA,	true,	true);
				//	}
				//	else
				//	{
				//		SetupGLElement(GLElement, GL_UNSIGNED_BYTE,	4,	true,	true);
				//	}
				//	break;
				case VET_Short2:		SetupGLElement(GLElement, GL_SHORT,			2,			false,	false); break;
				case VET_Short4:		SetupGLElement(GLElement, GL_SHORT,			4,			false,	false); break;
				case VET_Short2N:		SetupGLElement(GLElement, GL_SHORT,			2,			true,	true); break;
				//case VET_Half2:
				//	if (FOpenGL::SupportsVertexHalfFloat())
				//	{
				//		SetupGLElement(GLElement, FOpenGL::GetVertexHalfFloatFormat(), 2, false, true);
				//	}
				//	else
				//	{
				//		// @todo-mobile: Use shorts?
				//		SetupGLElement(GLElement, GL_SHORT, 2, false, true);
				//	}
				//	break;
				//case VET_Half4:
				//	if (FOpenGL::SupportsVertexHalfFloat())
				//	{
				//		SetupGLElement(GLElement, FOpenGL::GetVertexHalfFloatFormat(), 4, false, true);
				//	}
				//	else
				//	{
				//		// @todo-mobile: Use shorts?
				//		SetupGLElement(GLElement, GL_SHORT, 4, false, true);
				//	}
				//	break;
				case VET_INT:			SetupGLElement(GLElement, GL_INT, 4, false, false); break;
				case VET_UINT:			SetupGLElement(GLElement, GL_UNSIGNED_INT, 4, false, false); break;
				case VET_Short4N:		SetupGLElement(GLElement, GL_SHORT,			4,			true,	true); break;
				case VET_UShort2:		SetupGLElement(GLElement, GL_UNSIGNED_SHORT, 2, false, false); break;
				case VET_UShort4:		SetupGLElement(GLElement, GL_UNSIGNED_SHORT, 4, false, false); break;
				case VET_UShort2N:		SetupGLElement(GLElement, GL_UNSIGNED_SHORT, 2, true, true); break;
				case VET_UShort4N:		SetupGLElement(GLElement, GL_UNSIGNED_SHORT, 4, true, true); break;
				default:;// UE_LOG(LogRHI, Fatal, TEXT("Unknown RHI vertex element type %u"), (uint8)InElements[ElementIndex].Type);
			};
			VertexElements[ElementIndex]=(GLElement);
		}
	}
};

FVertexDeclarationRHIRef FOpenGLDynamicRHI::RHICreateVertexDeclaration(const FVertexDeclarationElementList& Elements)
{
	// Construct a key from the elements.
	FOpenGLVertexDeclarationKey Key(Elements);
	FVertexDeclarationRHIRef  v(new FOpenGLVertexDeclaration(Key.VertexElements));


	return v;
}
