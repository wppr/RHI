// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	OpenGLDrv.h: Public OpenGL RHI definitions.
=============================================================================*/

#pragma once

// Dependencies.
//#include "Core.h"
#include "RHI.h"
#include "DynamicRHI.h"
#include "OPENGL_Include.h"
// OpenGL RHI public headers.
#include "OpenGLUtil.h"
#include "OpenGLState.h"
#include "OpenGLResources.h"

class EGLService;

/** The interface which is implemented by the dynamically bound RHI. */
class  FOpenGLDynamicRHI : public FDynamicRHI, public IRHICommandContext
{
public:

	friend class FOpenGLViewport;

	/** Initialization constructor. */
	FOpenGLDynamicRHI();

	/** Destructor */
	~FOpenGLDynamicRHI() {}

	// FDynamicRHI interface.
	virtual void Init();
	virtual void Shutdown();
	template<typename TRHIType>
	static FORCEINLINE typename TOpenGLResourceTraits<TRHIType>::TConcreteType* ResourceCast(TRHIType* Resource)
	{
		return static_cast<typename TOpenGLResourceTraits<TRHIType>::TConcreteType*>(Resource);
	}
	virtual FSamplerStateRHIRef RHICreateSamplerState(const FSamplerStateInitializerRHI& Initializer) final override;
	virtual FRasterizerStateRHIRef RHICreateRasterizerState(const FRasterizerStateInitializerRHI& Initializer) final override;
	virtual FDepthStencilStateRHIRef RHICreateDepthStencilState(const FDepthStencilStateInitializerRHI& Initializer) final override;
	virtual FBlendStateRHIRef RHICreateBlendState(const FBlendStateInitializerRHI& Initializer) final override;
	virtual FVertexDeclarationRHIRef RHICreateVertexDeclaration(const FVertexDeclarationElementList& Elements) final override;
	virtual FPixelShaderRHIRef RHICreatePixelShader(const char* Code) final override;
	virtual FVertexShaderRHIRef RHICreateVertexShader(const char* Code) final override;
	virtual FBoundShaderStateRHIRef RHICreateBoundShaderState(FVertexDeclarationRHIParamRef VertexDeclaration, FVertexShaderRHIParamRef VertexShader, FPixelShaderRHIParamRef PixelShader) final override;
	virtual FBoundBufferStateRHIRef RHICreateBoundBufferState(const FBoundBufferDesc& Desc,  FRHIVertexDeclaration* VertexDeclaration)  final override;
	virtual FUniformBufferRHIRef RHICreateUniformBuffer(const void* Contents, const FRHIUniformBufferLayout& Layout, EUniformBufferUsage Usage) final override;
	virtual FIndexBufferRHIRef RHICreateIndexBuffer(uint32 Stride, uint32 Size, uint32 InUsage, FRHIResourceCreateInfo& CreateInfo) final override;
	virtual FVertexBufferRHIRef RHICreateVertexBuffer(uint32 Size, uint32 InUsage, FRHIResourceCreateInfo& CreateInfo) final override;
	virtual FTexture2DRHIRef RHICreateTexture2D(uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, uint32 NumSamples, uint32 Flags, FRHIResourceCreateInfo& CreateInfo) final override;
	//virtual FShaderResourceViewRHIRef RHICreateShaderResourceView(FTexture2DRHIParamRef Texture2DRHI, uint8 MipLevel) final override;
	//virtual FShaderResourceViewRHIRef RHICreateShaderResourceView(FTexture2DRHIParamRef Texture2DRHI, uint8 MipLevel, uint8 NumMipLevels, uint8 Format) final override;
	//virtual FShaderResourceViewRHIRef RHICreateShaderResourceView(FTexture3DRHIParamRef Texture3DRHI, uint8 MipLevel) final override;
	//virtual FShaderResourceViewRHIRef RHICreateShaderResourceView(FTexture2DArrayRHIParamRef Texture2DArrayRHI, uint8 MipLevel) final override;
	//virtual FShaderResourceViewRHIRef RHICreateShaderResourceView(FTextureCubeRHIParamRef TextureCubeRHI, uint8 MipLevel) final override;
	virtual void RHIGenerateMips(FTextureRHIParamRef Texture) final override;
	virtual FTextureCubeRHIRef RHICreateTextureCube(uint32 Size, uint8 Format, uint32 NumMips, uint32 Flags, FRHIResourceCreateInfo& CreateInfo) final override;
	//virtual FViewportRHIRef RHICreateViewport(void* WindowHandle, uint32 SizeX, uint32 SizeY, bool bIsFullscreen, EPixelFormat PreferredPixelFormat) final override;
	//virtual void RHIResizeViewport(FViewportRHIParamRef Viewport, uint32 SizeX, uint32 SizeY, bool bIsFullscreen) final override;
	virtual FRHIRenderTarget* RHICreateRenderTarget(const FRHISetRenderTargetsInfo& Info) final override;

	virtual void RHISetRasterizerState(FRasterizerStateRHIParamRef NewState) final override;
	virtual void RHISetViewport(uint32 MinX, uint32 MinY, float MinZ, uint32 MaxX, uint32 MaxY, float MaxZ) final override;
	virtual void RHISetScissorRect(bool bEnable, uint32 MinX, uint32 MinY, uint32 MaxX, uint32 MaxY) final override;
	virtual void RHISetBoundShaderState(FBoundShaderStateRHIParamRef BoundShaderState) final override;
	virtual void RHISetBoundBufferState(FBoundBufferStateRHIParamRef BoundBufferState) final override;
	virtual void RHISetShaderTexture(FVertexShaderRHIParamRef VertexShader, uint32 TextureIndex, FTextureRHIParamRef NewTexture) final override;
	virtual void RHISetShaderTexture(FPixelShaderRHIParamRef PixelShader, uint32 TextureIndex, FTextureRHIParamRef NewTexture) final override;
	virtual void RHISetShaderSampler(FVertexShaderRHIParamRef VertexShader, uint32 SamplerIndex, FSamplerStateRHIParamRef NewState) final override;
	virtual void RHISetShaderSampler(FPixelShaderRHIParamRef PixelShader, uint32 SamplerIndex, FSamplerStateRHIParamRef NewState) final override;
	//virtual void RHISetShaderUniformBuffer(FVertexShaderRHIParamRef VertexShader, uint32 BufferIndex, FUniformBufferRHIParamRef Buffer) final override;
	//virtual void RHISetShaderUniformBuffer(FPixelShaderRHIParamRef PixelShader, uint32 BufferIndex, FUniformBufferRHIParamRef Buffer) final override;
	//virtual void RHISetShaderParameter(FVertexShaderRHIParamRef VertexShader, uint32 BufferIndex, uint32 BaseIndex, uint32 NumBytes, const void* NewValue) final override;
	//virtual void RHISetShaderParameter(FPixelShaderRHIParamRef PixelShader, uint32 BufferIndex, uint32 BaseIndex, uint32 NumBytes, const void* NewValue) final override;
	virtual void RHISetDepthStencilState(FDepthStencilStateRHIParamRef NewState, uint32 StencilRef) final override;
	//virtual void RHISetBlendState(FBlendStateRHIParamRef NewState, const FLinearColor& BlendFactor) final override;
	//virtual void RHISetRenderTargets(uint32 NumSimultaneousRenderTargets, const FRHIRenderTargetView* NewRenderTargets, const FRHIDepthRenderTargetView* NewDepthStencilTarget, uint32 NumUAVs, const FUnorderedAccessViewRHIParamRef* UAVs) final override;
	//virtual void RHISetRenderTargetsAndClear(const FRHISetRenderTargetsInfo& RenderTargetsInfo) final override;
	virtual void RHISetRenderTarget(FRHIRenderTarget* RenderTarget) final override;
	virtual void RHIDrawPrimitive(uint32 PrimitiveType, uint32 BaseVertexIndex, uint32 NumPrimitives, uint32 NumInstances) final override;
	virtual void RHIDrawIndexedPrimitive(FIndexBufferRHIParamRef IndexBuffer, uint32 PrimitiveType, int32 BaseVertexIndex, uint32 FirstInstance, uint32 NumVertices, uint32 StartIndex, uint32 NumPrimitives, uint32 NumInstances) final override;
	virtual void RHIClear(bool bClearColor, const FLinearColor& Color, bool bClearDepth, float Depth, bool bClearStencil, uint32 Stencil, FIntRect ExcludeRect) final override;
	virtual void RHIClearMRT(bool bClearColor, int32 NumClearColors, const FLinearColor* ColorArray, bool bClearDepth, float Depth, bool bClearStencil, uint32 Stencil, FIntRect ExcludeRect) final override;
//	virtual void RHIEnableDepthBoundsTest(bool bEnable, float MinDepth, float MaxDepth) final override;
	void CachedSetupTextureStage(FOpenGLContextState& ContextState, GLint TextureIndex, GLenum Target, GLuint Resource, GLint BaseMip, GLint NumMips);

	FOpenGLContextState& GetContextStateForCurrentContext();

	void CachedBindArrayBuffer( FOpenGLContextState& ContextState, GLuint Buffer )
	{
		VERIFY_GL_SCOPE();
		if( ContextState.ArrayBufferBound != Buffer )
		{
			glBindBuffer( GL_ARRAY_BUFFER, Buffer );
			ContextState.ArrayBufferBound = Buffer;
		}
	}

	void CachedBindElementArrayBuffer( FOpenGLContextState& ContextState, GLuint Buffer )
	{
		VERIFY_GL_SCOPE();
		if( ContextState.ElementArrayBufferBound != Buffer )
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, Buffer );
			ContextState.ElementArrayBufferBound = Buffer;
		}
	}

	void CachedBindPixelUnpackBuffer( FOpenGLContextState& ContextState, GLuint Buffer )
	{
		VERIFY_GL_SCOPE();

		if( ContextState.PixelUnpackBufferBound != Buffer )
		{
			glBindBuffer( GL_PIXEL_UNPACK_BUFFER, Buffer );
			ContextState.PixelUnpackBufferBound = Buffer;
		}
	}

	void CachedBindUniformBuffer( FOpenGLContextState& ContextState, GLuint Buffer )
	{
		VERIFY_GL_SCOPE();
		if( ContextState.UniformBufferBound != Buffer )
		{
			glBindBuffer( GL_UNIFORM_BUFFER, Buffer );
			ContextState.UniformBufferBound = Buffer;
		}
	}

	bool IsUniformBufferBound( FOpenGLContextState& ContextState, GLuint Buffer ) const
	{
		return ( ContextState.UniformBufferBound == Buffer );
	}

	/** Add query to Queries list upon its creation. */
	//void RegisterQuery( FOpenGLRenderQuery* Query );

	///** Remove query from Queries list upon its deletion. */
	//void UnregisterQuery( FOpenGLRenderQuery* Query );

	/** Inform all queries about the need to recreate themselves after OpenGL context they're in gets deleted. */
	void InvalidateQueries();

	FOpenGLSamplerState* GetPointSamplerState() const { return (FOpenGLSamplerState*)PointSamplerState.get(); }

	FRHITexture* CreateOpenGLTexture(uint32 SizeX, uint32 SizeY, bool CubeTexture, bool ArrayTexture, uint8 Format, uint32 NumMips, uint32 NumSamples, uint32 ArraySize, uint32 Flags, const FClearValueBinding& InClearValue, vector<void*> BulkData);

	void SetCustomPresent(class FRHICustomPresent* InCustomPresent);
private:


	/** Counter incremented each time RHIBeginScene is called. */
	uint32 SceneFrameCounter;

	/** Value used to detect when resource tables need to be recached. INDEX_NONE means always recache. */
	uint32 ResourceTableFrameCounter;

	/** RHI device state, independent of underlying OpenGL context used */
	FOpenGLRHIState						PendingState;
	FOpenGLStreamedVertexBufferArray	DynamicVertexBuffers;
//	FOpenGLStreamedIndexBufferArray		DynamicIndexBuffers;
	FSamplerStateRHIRef					PointSamplerState;

	/** A list of all viewport RHIs that have been created. */
	vector<FOpenGLViewport*> Viewports;
	shared_ptr<FOpenGLViewport>		DrawingViewport;
	bool								bRevertToSharedContextAfterDrawingViewport;

	bool								bIsRenderingContextAcquired;


	/** Per-context state caching */
	FOpenGLContextState	SharedContextState;
	FOpenGLContextState	RenderingContextState;

	/** Underlying platform-specific data */
	struct FPlatformOpenGLDevice* PlatformDevice;


	GLuint GetOpenGLFramebuffer(uint32 NumSimultaneousRenderTargets, FOpenGLTextureBase** RenderTargets, uint32* ArrayIndices, uint32* MipmapLevels, FOpenGLTextureBase* DepthStencilTarget);

	void InitializeStateResources();

	/** needs to be called before each dispatch call */
	

	void EnableVertexElementCached(FOpenGLContextState& ContextCache, const FOpenGLVertexElement &VertexElement, GLsizei Stride, void *Pointer, GLuint Buffer);
	void EnableVertexElementCachedZeroStride(FOpenGLContextState& ContextCache, const FOpenGLVertexElement &VertexElement, uint32 NumVertices, FOpenGLVertexBuffer* VertexBuffer);
	void SetupVertexArrays(FOpenGLContextState& ContextCache, uint32 BaseVertexIndex, FOpenGLStream* Streams, uint32 NumStreams, uint32 MaxVertices);
	void SetupVertexArraysVAB(FOpenGLContextState& ContextCache, uint32 BaseVertexIndex, FOpenGLStream* Streams, uint32 NumStreams, uint32 MaxVertices);
	void SetupVertexArraysUP(FOpenGLContextState& ContextState, void* Buffer, uint32 Stride);

//	void SetupBindlessTextures( FOpenGLContextState& ContextState, const TArray<FOpenGLBindlessSamplerInfo> &Samplers );

	/** needs to be called before each draw call */
	void BindPendingFramebuffer( FOpenGLContextState& ContextState );
	void BindPendingShaderState( FOpenGLContextState& ContextState );
	void BindPendingBufferState(FOpenGLContextState& ContextState);
	void BindPendingComputeShaderState( FOpenGLContextState& ContextState, FComputeShaderRHIParamRef ComputeShaderRHI);
	void UpdateRasterizerStateInOpenGLContext( FOpenGLContextState& ContextState );
	void UpdateDepthStencilStateInOpenGLContext( FOpenGLContextState& ContextState );
	void UpdateScissorRectInOpenGLContext( FOpenGLContextState& ContextState );
	void UpdateViewportInOpenGLContext( FOpenGLContextState& ContextState );
	
	template <class ShaderType> void SetResourcesFromTables(const ShaderType* RESTRICT);
	void CommitGraphicsResourceTables();
	void CommitComputeResourceTables(class FOpenGLComputeShader* ComputeShader);
	void CommitNonComputeShaderConstants();
	void CommitComputeShaderConstants(FComputeShaderRHIParamRef ComputeShaderRHI);
	void SetPendingBlendStateForActiveRenderTargets( FOpenGLContextState& ContextState );
	
	void SetupTexturesForDraw( FOpenGLContextState& ContextState);
	template <typename StateType>
	void SetupTexturesForDraw( FOpenGLContextState& ContextState, const StateType ShaderState, int32 MaxTexturesNeeded);


public:
	/** Remember what RHI user wants set on a specific OpenGL texture stage, translating from Stage and TextureIndex for stage pair. */
	void InternalSetShaderTexture(FOpenGLTextureBase* Texture, FOpenGLShaderResourceView* SRV, GLint TextureIndex, GLenum Target, GLuint Resource, int NumMips, int LimitMip);
	void InternalSetShaderUAV(GLint UAVIndex, GLenum Format, GLuint Resource);
	void InternalSetSamplerStates(GLint TextureIndex, FOpenGLSamplerState* SamplerState);

private:
	void ApplyTextureStage(FOpenGLContextState& ContextState, GLint TextureIndex, const FTextureStage& TextureStage, FOpenGLSamplerState* SamplerState);

//	void ReadSurfaceDataRaw(FOpenGLContextState& ContextState, FTextureRHIParamRef TextureRHI,FIntRect Rect,TArray<uint8>& OutData, FReadSurfaceDataFlags InFlags);

	void BindUniformBufferBase(FOpenGLContextState& ContextState, int32 NumUniformBuffers, FUniformBufferRHIRef* BoundUniformBuffers, uint32 FirstUniformBuffer, bool ForceUpdate);

	void ClearCurrentFramebufferWithCurrentScissor(FOpenGLContextState& ContextState, int8 ClearType, int32 NumClearColors, const FLinearColor* ClearColorArray, float Depth, uint32 Stencil);

	void FreeZeroStrideBuffers();

	/** Consumes about 100ms of GPU time (depending on resolution and GPU), useful for making sure we're not CPU bound when GPU profiling. */
	void IssueLongGPUTask();

};

