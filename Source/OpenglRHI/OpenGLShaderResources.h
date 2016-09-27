// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	OpenGLShaderResources.h: OpenGL shader resource RHI definitions.
=============================================================================*/
#pragma once
#include "OPENGL_Include.h"
#include "DynamicRHI.h"
#include <string>
using std::string;
/**
 * OpenGL shader resource.
 */
template <typename RHIResourceType, GLenum GLTypeEnum, EShaderFrequency FrequencyT>
class TOpenGLShader : public RHIResourceType
{
public:
	enum
	{
		StaticFrequency = FrequencyT
	};
	static const GLenum TypeEnum = GLTypeEnum;

	/** The OpenGL resource ID. */
	GLuint Resource;
	/** true if the shader has compiled successfully. */
	bool bSuccessfullyCompiled;

	string GlslCode;
	TOpenGLShader()
		: Resource(0)
		, bSuccessfullyCompiled(false)
	{

}

	~TOpenGLShader()
	{
	}
};


typedef TOpenGLShader<FRHIVertexShader, GL_VERTEX_SHADER, SF_Vertex> FOpenGLVertexShader;
typedef TOpenGLShader<FRHIPixelShader, GL_FRAGMENT_SHADER, SF_Pixel> FOpenGLPixelShader;
enum
{
	OGL_MAX_UNIFORM_BUFFER_BINDINGS = 12,	// @todo-mobile: Remove me
	OGL_FIRST_UNIFORM_BUFFER = 0,			// @todo-mobile: Remove me
	OGL_MAX_COMPUTE_STAGE_UAV_UNITS = 8,	// @todo-mobile: Remove me
	OGL_UAV_NOT_SUPPORTED_FOR_GRAPHICS_UNIT = -1, // for now, only CS supports UAVs/ images
};