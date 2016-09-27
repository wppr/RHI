#include "OpenGLRHI.h"
#include "OpenGLES31.h"
#include "OpenGLDrvPrivate.h"
FOpenGLDynamicRHI::FOpenGLDynamicRHI()
{

}



FBoundBufferStateRHIRef FOpenGLDynamicRHI::RHICreateBoundBufferState(const FBoundBufferDesc& Desc, FRHIVertexDeclaration* VertexDeclaration) 
{
	shared_ptr<FOpenGLBoundBufferState> BoundBufferState(new FOpenGLBoundBufferState(Desc, VertexDeclaration));
	return BoundBufferState;
}

FUniformBufferRHIRef FOpenGLDynamicRHI::RHICreateUniformBuffer(const void* Contents, const FRHIUniformBufferLayout& Layout, EUniformBufferUsage Usage)
{
	return NULL;
}

FIndexBufferRHIRef FOpenGLDynamicRHI::RHICreateIndexBuffer(uint32 Stride, uint32 Size, uint32 InUsage, FRHIResourceCreateInfo& CreateInfo)
{
	auto Data = CreateInfo.ResourceArray;
	shared_ptr<FOpenGLIndexBuffer> IndexBuffer(new FOpenGLIndexBuffer(0, Size, InUsage, Data));
	return IndexBuffer;
}

FVertexBufferRHIRef FOpenGLDynamicRHI::RHICreateVertexBuffer(uint32 Size, uint32 InUsage, FRHIResourceCreateInfo& CreateInfo)
{
	auto Data = CreateInfo.ResourceArray;
	shared_ptr<FOpenGLVertexBuffer> VertexBuffer(new FOpenGLVertexBuffer(0, Size, InUsage, Data));
	return VertexBuffer;
}





void FOpenGLDynamicRHI::RHISetRasterizerState(FRasterizerStateRHIParamRef NewState)
{

}

void FOpenGLDynamicRHI::RHISetViewport(uint32 MinX, uint32 MinY, float MinZ, uint32 MaxX, uint32 MaxY, float MaxZ)
{

}

void FOpenGLDynamicRHI::RHISetScissorRect(bool bEnable, uint32 MinX, uint32 MinY, uint32 MaxX, uint32 MaxY)
{

}



void FOpenGLDynamicRHI::InitializeStateResources()
{
	SharedContextState.InitializeResources(FOpenGL::GetMaxCombinedTextureImageUnits(), OGL_MAX_COMPUTE_STAGE_UAV_UNITS);
	RenderingContextState.InitializeResources(FOpenGL::GetMaxCombinedTextureImageUnits(), OGL_MAX_COMPUTE_STAGE_UAV_UNITS);
	PendingState.InitializeResources(FOpenGL::GetMaxCombinedTextureImageUnits(), OGL_MAX_COMPUTE_STAGE_UAV_UNITS);
}

void FOpenGLDynamicRHI::RHIDrawIndexedPrimitive(FIndexBufferRHIParamRef IndexBuffer, uint32 PrimitiveType, int32 BaseVertexIndex, uint32 FirstInstance, uint32 NumVertices, uint32 StartIndex, uint32 NumPrimitives, uint32 NumInstances)
{

}

void FOpenGLDynamicRHI::RHIClear(bool bClearColor, const FLinearColor& Color, bool bClearDepth, float Depth, bool bClearStencil, uint32 Stencil, FIntRect ExcludeRect)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FOpenGLDynamicRHI::RHIClearMRT(bool bClearColor, int32 NumClearColors, const FLinearColor* ColorArray, bool bClearDepth, float Depth, bool bClearStencil, uint32 Stencil, FIntRect ExcludeRect)
{

}
