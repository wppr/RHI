// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "RHIDefinitions.h"
#include <memory>
#include "PixelFormat.h" // for EPixelFormat
#include "Color.h"
#include <assert.h>
#include <vector>
#include "unreal_defination.h"
using std::shared_ptr;
using std::make_shared;
class FRHIResource
{
public:
	FRHIResource()
	{
	}
	virtual ~FRHIResource() 
	{ 
	}
	int AddRef()
	{
		int NewValue = ++NumRefs;
		return NewValue;
	}
	int Release()
	{
		int NewValue = --NumRefs;
		if (NewValue == 0)
		{
			delete this;
		}
		return NewValue;
	}
	int GetRefCount()
	{
		return NumRefs;
	}
private:
	int NumRefs=0;
	//static std::vector<FRHIResource*> PendingDeletes;
	//static FRHIResource* CurrentlyDeleting;
};


//
// State blocks
//

class FRHISamplerState : public FRHIResource {};
class FRHIRasterizerState : public FRHIResource {};
class FRHIDepthStencilState : public FRHIResource {};
class FRHIBlendState : public FRHIResource {};

//
// Shader bindings
//

class FRHIVertexDeclaration : public FRHIResource {};
class FRHIBoundShaderState : public FRHIResource {};
class FRHIBoundBufferState : public FRHIResource {};
//
// Shader
//

class FRHIShader : public FRHIResource
{
public:
	FRHIShader() : FRHIResource() {}

};

class FRHIVertexShader : public FRHIShader {};
class FRHIHullShader : public FRHIShader {};
class FRHIDomainShader : public FRHIShader {};
class FRHIPixelShader : public FRHIShader {};
class FRHIGeometryShader : public FRHIShader {};
class FRHIComputeShader : public FRHIShader {};

//
// Buffers
//

/** The layout of a uniform buffer in memory. */
struct FRHIUniformBufferLayout
{
	/** The size of the constant buffer in bytes. */
	uint32 ConstantBufferSize;
	/** The offset to the beginning of the resource table. */
	uint32 ResourceOffset;
	/** The type of each resource (EUniformBufferBaseType). */

	//change by me: use self defined EUniformBUfferType
	std::vector<uint8> Resources;

	FRHIUniformBufferLayout() :
		ConstantBufferSize(0),
		ResourceOffset(0)
	{
	}

};

/** Compare two uniform buffer layouts. */
inline bool operator==(const FRHIUniformBufferLayout& A, const FRHIUniformBufferLayout& B)
{
	return A.ConstantBufferSize == B.ConstantBufferSize
		&& A.ResourceOffset == B.ResourceOffset
		&& A.Resources == B.Resources;
}

class FRHIUniformBuffer : public FRHIResource
{
public:

	/** Initialization constructor. */
	FRHIUniformBuffer(const FRHIUniformBufferLayout& InLayout)
	: Layout(&InLayout)
	{}

	/** @return The number of bytes in the uniform buffer. */
	uint32 GetSize() const { return Layout->ConstantBufferSize; }
	const FRHIUniformBufferLayout& GetLayout() const { return *Layout; }

private:
	/** Layout of the uniform buffer. */
	const FRHIUniformBufferLayout* Layout;
};

class FRHIIndexBuffer : public FRHIResource
{
public:

	/** Initialization constructor. */
	FRHIIndexBuffer(uint32 InStride,uint32 InSize,uint32 InUsage)
	: Stride(InStride)
	, Size(InSize)
	, Usage(InUsage)
	{}
	/** @return The stride in bytes of the index buffer; must be 2 or 4. */
	uint32 GetStride() const { return Stride; }

	/** @return The number of bytes in the index buffer. */
	uint32 GetSize() const { return Size; }

	/** @return The usage flags used to create the index buffer. */
	uint32 GetUsage() const { return Usage; }
private:
	uint32 Stride;
	uint32 Size;
	uint32 Usage;
};

class FRHIVertexBuffer : public FRHIResource
{
public:

	/** Initialization constructor. */
	FRHIVertexBuffer(uint32 InSize,uint32 InUsage)
	: Size(InSize)
	, Usage(InUsage)
	{}

	/** @return The number of bytes in the index buffer. */
	uint32 GetSize() const { return Size; }

	/** @return The usage flags used to create the index buffer. */
	uint32 GetUsage() const { return Usage; }
private:
	uint32 Size;
	uint32 Usage;
};
class FRHIStructuredBuffer : public FRHIResource
{
public:

	/** Initialization constructor. */
	FRHIStructuredBuffer(uint32 InStride,uint32 InSize,uint32 InUsage)
	: Stride(InStride)
	, Size(InSize)
	, Usage(InUsage)
	{}

	uint32 Stride;
	uint32 Size;
	uint32 Usage;
};

class FClearValueBinding;
class  FRHITexture : public FRHIResource
{
public:
	
	/** Initialization constructor. */
	FRHITexture(uint32 InNumMips, uint32 InNumSamples, EPixelFormat InFormat, uint32 InFlags, const FClearValueBinding& InClearValue)
	:// ClearValue(InClearValue),
	 NumMips(InNumMips)
	, NumSamples(InNumSamples)
	, Format(InFormat)
	, Flags(InFlags)
	{}

	// Dynamic cast methods.
	virtual class FRHITexture2D* GetTexture2D() { return NULL; }
	virtual class FRHITexture2DArray* GetTexture2DArray() { return NULL; }
	virtual class FRHITexture3D* GetTexture3D() { return NULL; }
	virtual class FRHITextureCube* GetTextureCube() { return NULL; }
	virtual class FRHITextureReference* GetTextureReference() { return NULL; }
	
	/**
	 * Returns access to the platform-specific native resource pointer.  This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all.
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason
	 */
	virtual void* GetNativeResource() const
	{
		// Override this in derived classes to expose access to the native texture resource
		return nullptr;
	}

	/**
	 * Returns access to the platform-specific native shader resource view pointer.  This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all.
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason
	 */
	virtual void* GetNativeShaderResourceView() const
	{
		// Override this in derived classes to expose access to the native texture resource
		return nullptr;
	}

	/** @return The number of mip-maps in the texture. */
	uint32 GetNumMips() const { return NumMips; }

	/** @return The format of the pixels in the texture. */
	EPixelFormat GetFormat() const { return Format; }

	/** @return The flags used to create the texture. */
	uint32 GetFlags() const { return Flags; }

	/* @return the number of samples for multi-sampling. */
	uint32 GetNumSamples() const { return NumSamples; }

	/** @return Whether the texture is multi sampled. */
	bool IsMultisampled() const { return NumSamples > 1; }		

	//FRHIResourceInfo ResourceInfo;


	//void SetName(FName& InName)
	//{
	//	TextureName = InName;
	//}

	//FName GetName() const
	//{
	//	return TextureName;
	//}

	//bool HasClearValue() const
	//{
	//	return ClearValue.ColorBinding != EClearBinding::ENoneBound;
	//}

	//FLinearColor GetClearColor() const
	//{
	//	return ClearValue.GetClearColor();
	//}

	//void GetDepthStencilClearValue(float& OutDepth, uint32& OutStencil) const
	//{
	//	return ClearValue.GetDepthStencil(OutDepth, OutStencil);
	//}

	//float GetDepthClearValue() const
	//{
	//	float Depth;
	//	uint32 Stencil;
	//	ClearValue.GetDepthStencil(Depth, Stencil);
	//	return Depth;
	//}

	//uint32 GetStencilClearValue() const
	//{
	//	float Depth;
	//	uint32 Stencil;
	//	ClearValue.GetDepthStencil(Depth, Stencil);
	//	return Stencil;
	//}

	//const FClearValueBinding GetClearBinding() const
	//{
	//	return ClearValue;
	//}

private:
	//FClearValueBinding ClearValue;
	uint32 NumMips;
	uint32 NumSamples;
	EPixelFormat Format;
	uint32 Flags;
	FName TextureName;
};


class  FRHITexture2D : public FRHITexture
{
public:

	/** Initialization constructor. */
	FRHITexture2D(uint32 InSizeX, uint32 InSizeY, uint32 InNumMips, uint32 InNumSamples, EPixelFormat InFormat, uint32 InFlags, const FClearValueBinding& InClearValue)
		: FRHITexture(InNumMips, InNumSamples, InFormat, InFlags, InClearValue)
		, SizeX(InSizeX)
		, SizeY(InSizeY)
	{}

	// Dynamic cast methods.
	virtual FRHITexture2D* GetTexture2D() { return this; }

	/** @return The width of the texture. */
	uint32 GetSizeX() const { return SizeX; }

	/** @return The height of the texture. */
	uint32 GetSizeY() const { return SizeY; }

private:

	uint32 SizeX;
	uint32 SizeY;
};

class FRHITextureCube : public FRHITexture
{
public:

	/** Initialization constructor. */
	FRHITextureCube(uint32 InSize, uint32 InNumMips, EPixelFormat InFormat, uint32 InFlags, const FClearValueBinding& InClearValue)
		: FRHITexture(InNumMips, 1, InFormat, InFlags, InClearValue)
		, Size(InSize)
	{}

	// Dynamic cast methods.
	virtual FRHITextureCube* GetTextureCube() { return this; }

	/** @return The width and height of each face of the cubemap. */
	uint32 GetSize() const { return Size; }

private:

	uint32 Size;
};

class  FRHITexture2DArray : public FRHITexture
{
public:

	/** Initialization constructor. */
	FRHITexture2DArray(uint32 InSizeX, uint32 InSizeY, uint32 InSizeZ, uint32 InNumMips, EPixelFormat InFormat, uint32 InFlags, const FClearValueBinding& InClearValue)
		: FRHITexture(InNumMips, 1, InFormat, InFlags, InClearValue)
		, SizeX(InSizeX)
		, SizeY(InSizeY)
		, SizeZ(InSizeZ)
	{}

	// Dynamic cast methods.
	virtual FRHITexture2DArray* GetTexture2DArray() { return this; }

	/** @return The width of the textures in the array. */
	uint32 GetSizeX() const { return SizeX; }

	/** @return The height of the texture in the array. */
	uint32 GetSizeY() const { return SizeY; }

	/** @return The number of textures in the array. */
	uint32 GetSizeZ() const { return SizeZ; }

private:

	uint32 SizeX;
	uint32 SizeY;
	uint32 SizeZ;
};

class  FRHITexture3D : public FRHITexture
{
public:

	/** Initialization constructor. */
	FRHITexture3D(uint32 InSizeX, uint32 InSizeY, uint32 InSizeZ, uint32 InNumMips, EPixelFormat InFormat, uint32 InFlags, const FClearValueBinding& InClearValue)
		: FRHITexture(InNumMips, 1, InFormat, InFlags, InClearValue)
		, SizeX(InSizeX)
		, SizeY(InSizeY)
		, SizeZ(InSizeZ)
	{}

	// Dynamic cast methods.
	virtual FRHITexture3D* GetTexture3D() { return this; }

	/** @return The width of the texture. */
	uint32 GetSizeX() const { return SizeX; }

	/** @return The height of the texture. */
	uint32 GetSizeY() const { return SizeY; }

	/** @return The depth of the texture. */
	uint32 GetSizeZ() const { return SizeZ; }

private:

	uint32 SizeX;
	uint32 SizeY;
	uint32 SizeZ;
};

class FRHIViewport : public FRHIResource 
{
public:
	FRHIViewport()
		: FRHIResource()
	{
	}
	/**
	 * Returns access to the platform-specific native resource pointer.  This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all.
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason
	 */
	virtual void* GetNativeSwapChain() const { return nullptr; }
	/**
	 * Returns access to the platform-specific native resource pointer to a backbuffer texture.  This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all.
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason
	 */
	virtual void* GetNativeBackBufferTexture() const { return nullptr; }
	/**
	 * Returns access to the platform-specific native resource pointer to a backbuffer rendertarget. This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all.
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason
	 */
	virtual void* GetNativeBackBufferRT() const { return nullptr; }

	/**
	 * Returns access to the platform-specific native window. This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all. 
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason.
	 * AddParam could represent any additional platform-specific data (could be null).
	 */
	virtual void* GetNativeWindow(void** AddParam = nullptr) const { return nullptr; }

	/**
	 * Sets custom Present handler on the viewport
	 */
	virtual void SetCustomPresent(class FRHICustomPresent*) {}

	/**
	 * Returns currently set custom present handler.
	 */
	virtual class FRHICustomPresent* GetCustomPresent() const { return nullptr; }
};

//
// Views
//

class FRHIUnorderedAccessView : public FRHIResource {};
class FRHIShaderResourceView : public FRHIResource {};



typedef FRHISamplerState*              FSamplerStateRHIParamRef;
typedef std::shared_ptr<FRHISamplerState> FSamplerStateRHIRef;

typedef FRHIRasterizerState*              FRasterizerStateRHIParamRef;
typedef std::shared_ptr<FRHIRasterizerState> FRasterizerStateRHIRef;

typedef FRHIDepthStencilState*              FDepthStencilStateRHIParamRef;
typedef std::shared_ptr<FRHIDepthStencilState> FDepthStencilStateRHIRef;

typedef FRHIBlendState*              FBlendStateRHIParamRef;
typedef std::shared_ptr<FRHIBlendState> FBlendStateRHIRef;

typedef FRHIVertexDeclaration*              FVertexDeclarationRHIParamRef;
typedef std::shared_ptr<FRHIVertexDeclaration> FVertexDeclarationRHIRef;

typedef FRHIVertexShader*              FVertexShaderRHIParamRef;
typedef std::shared_ptr<FRHIVertexShader> FVertexShaderRHIRef;

typedef FRHIHullShader*              FHullShaderRHIParamRef;
typedef std::shared_ptr<FRHIHullShader> FHullShaderRHIRef;

typedef FRHIDomainShader*              FDomainShaderRHIParamRef;
typedef std::shared_ptr<FRHIDomainShader> FDomainShaderRHIRef;

typedef FRHIPixelShader*              FPixelShaderRHIParamRef;
typedef std::shared_ptr<FRHIPixelShader> FPixelShaderRHIRef;

typedef FRHIGeometryShader*              FGeometryShaderRHIParamRef;
typedef std::shared_ptr<FRHIGeometryShader> FGeometryShaderRHIRef;

typedef FRHIComputeShader*              FComputeShaderRHIParamRef;
typedef std::shared_ptr<FRHIComputeShader> FComputeShaderRHIRef;

typedef FRHIBoundShaderState*              FBoundShaderStateRHIParamRef;
typedef std::shared_ptr<FRHIBoundShaderState> FBoundShaderStateRHIRef;

typedef FRHIBoundBufferState*              FBoundBufferStateRHIParamRef;
typedef std::shared_ptr<FRHIBoundBufferState> FBoundBufferStateRHIRef;

typedef FRHIUniformBuffer*              FUniformBufferRHIParamRef;
typedef std::shared_ptr<FRHIUniformBuffer> FUniformBufferRHIRef;

typedef FRHIIndexBuffer*              FIndexBufferRHIParamRef;
typedef std::shared_ptr<FRHIIndexBuffer> FIndexBufferRHIRef;

typedef FRHIVertexBuffer*              FVertexBufferRHIParamRef;
typedef std::shared_ptr<FRHIVertexBuffer> FVertexBufferRHIRef;

typedef FRHIStructuredBuffer*              FStructuredBufferRHIParamRef;
typedef std::shared_ptr<FRHIStructuredBuffer> FStructuredBufferRHIRef;

typedef FRHITexture*              FTextureRHIParamRef;
typedef std::shared_ptr<FRHITexture> FTextureRHIRef;

typedef FRHITexture2D*              FTexture2DRHIParamRef;
typedef std::shared_ptr<FRHITexture2D> FTexture2DRHIRef;

typedef FRHITexture2DArray*              FTexture2DArrayRHIParamRef;
typedef std::shared_ptr<FRHITexture2DArray> FTexture2DArrayRHIRef;

typedef FRHITexture3D*              FTexture3DRHIParamRef;
typedef std::shared_ptr<FRHITexture3D> FTexture3DRHIRef;

typedef FRHITextureCube*              FTextureCubeRHIParamRef;
typedef std::shared_ptr<FRHITextureCube> FTextureCubeRHIRef;
//
//typedef FRHITextureReference*              FTextureReferenceRHIParamRef;
//typedef std::shared_ptr<FRHITextureReference> FTextureReferenceRHIRef;

typedef FRHIViewport*              FViewportRHIParamRef;
typedef std::shared_ptr<FRHIViewport> FViewportRHIRef;

typedef FRHIUnorderedAccessView*              FUnorderedAccessViewRHIParamRef;
typedef std::shared_ptr<FRHIUnorderedAccessView> FUnorderedAccessViewRHIRef;

typedef FRHIShaderResourceView*              FShaderResourceViewRHIParamRef;
typedef std::shared_ptr<FRHIShaderResourceView> FShaderResourceViewRHIRef;




class FRHIRenderTargetView
{
public:
	FTextureRHIParamRef Texture;
	uint32 MipIndex;

	/** Array slice or texture cube face.  Only valid if texture resource was created with TexCreate_TargetArraySlicesIndependently! */
	uint32 ArraySliceIndex;
	
	ERenderTargetLoadAction LoadAction;
	ERenderTargetStoreAction StoreAction;

	FRHIRenderTargetView() : 
		Texture(NULL),
		MipIndex(0),
		ArraySliceIndex(-1),
		LoadAction(ERenderTargetLoadAction::ELoad),
		StoreAction(ERenderTargetStoreAction::EStore)
	{}

	FRHIRenderTargetView(const FRHIRenderTargetView& Other) :
		Texture(Other.Texture),
		MipIndex(Other.MipIndex),
		ArraySliceIndex(Other.ArraySliceIndex),
		LoadAction(Other.LoadAction),
		StoreAction(Other.StoreAction)
	{}

	FRHIRenderTargetView(FTextureRHIParamRef InTexture) :
		Texture(InTexture),
		MipIndex(0),
		ArraySliceIndex(-1),
		LoadAction(ERenderTargetLoadAction::ELoad),
		StoreAction(ERenderTargetStoreAction::EStore)
	{}

	FRHIRenderTargetView(FTextureRHIParamRef InTexture, uint32 InMipIndex, uint32 InArraySliceIndex) :
		Texture(InTexture),
		MipIndex(InMipIndex),
		ArraySliceIndex(InArraySliceIndex),
		LoadAction(ERenderTargetLoadAction::ELoad),
		StoreAction(ERenderTargetStoreAction::EStore)
	{}
	
	FRHIRenderTargetView(FTextureRHIParamRef InTexture, uint32 InMipIndex, uint32 InArraySliceIndex, ERenderTargetLoadAction InLoadAction, ERenderTargetStoreAction InStoreAction) :
		Texture(InTexture),
		MipIndex(InMipIndex),
		ArraySliceIndex(InArraySliceIndex),
		LoadAction(InLoadAction),
		StoreAction(InStoreAction)
	{}

	bool operator==(const FRHIRenderTargetView& Other) const
	{
		return 
			Texture == Other.Texture &&
			MipIndex == Other.MipIndex &&
			ArraySliceIndex == Other.ArraySliceIndex &&
			LoadAction == Other.LoadAction &&
			StoreAction == Other.StoreAction;
	}
};

class FExclusiveDepthStencil
{
public:
	enum Type
	{
		// don't use those directly, use the combined versions below
		// 4 bits are used for depth and 4 for stencil to make the hex value readable and non overlapping
		DepthNop =		0x00,
		DepthRead =		0x01,
		DepthWrite =	0x02,
		DepthMask =		0x0f,
		StencilNop =	0x00,
		StencilRead =	0x10,
		StencilWrite =	0x20,
		StencilMask =	0xf0,

		// use those:
		DepthNop_StencilNop = DepthNop + StencilNop,
		DepthRead_StencilNop = DepthRead + StencilNop,
		DepthWrite_StencilNop = DepthWrite + StencilNop,
		DepthNop_StencilRead = DepthNop + StencilRead,
		DepthRead_StencilRead = DepthRead + StencilRead,
		DepthWrite_StencilRead = DepthWrite + StencilRead,
		DepthNop_StencilWrite = DepthNop + StencilWrite,
		DepthRead_StencilWrite = DepthRead + StencilWrite,
		DepthWrite_StencilWrite = DepthWrite + StencilWrite,
	};

private:
	Type Value;

public:
	// constructor
	FExclusiveDepthStencil(Type InValue = DepthNop_StencilNop)
		: Value(InValue)
	{
	}

	inline bool IsUsingDepthStencil() const
	{
		return Value != DepthNop_StencilNop;
	}
	inline bool IsDepthWrite() const
	{
		return ExtractDepth() == DepthWrite;
	}
	inline bool IsStencilWrite() const
	{
		return ExtractStencil() == StencilWrite;
	}
	inline void SetDepthWrite()
	{
		Value = (Type)(ExtractStencil() | DepthWrite);
	}
	inline void SetStencilWrite()
	{
		Value = (Type)(ExtractDepth() | StencilWrite);
	}
	inline void SetDepthStencilWrite(bool bDepth, bool bStencil)
	{
		Value = DepthNop_StencilNop;

		if (bDepth)
		{
			SetDepthWrite();
		}
		if (bStencil)
		{
			SetStencilWrite();
		}
	}
	bool operator==(const FExclusiveDepthStencil& rhs) const
	{
		return Value == rhs.Value;
	}
	inline bool IsValid(FExclusiveDepthStencil& Current) const
	{
		Type Depth = ExtractDepth();

		if (Depth != DepthNop && Depth != Current.ExtractDepth())
		{
			return false;
		}

		Type Stencil = ExtractStencil();

		if (Stencil != StencilNop && Stencil != Current.ExtractStencil())
		{
			return false;
		}

		return true;
	}

	uint32 GetIndex() const
	{
		// Note: The array to index has views created in that specific order.

		// we don't care about the Nop versions so less views are needed
		// we combine Nop and Write
		switch (Value)
		{
			case DepthWrite_StencilNop:
			case DepthNop_StencilWrite:
			case DepthWrite_StencilWrite:
			case DepthNop_StencilNop:
				return 0; // old DSAT_Writable
		
			case DepthRead_StencilNop:
			case DepthRead_StencilWrite:
				return 1; // old DSAT_ReadOnlyDepth

			case DepthNop_StencilRead:
			case DepthWrite_StencilRead:
				return 2; // old DSAT_ReadOnlyStencil

			case DepthRead_StencilRead:
				return 3; // old DSAT_ReadOnlyDepthAndStencil
		}
		// should never happen
		return -1;
	}
	static const uint32 MaxIndex = 4;

private:
	inline Type ExtractDepth() const
	{
		return (Type)(Value & DepthMask);
	}
	inline Type ExtractStencil() const
	{
		return (Type)(Value & StencilMask);
	}
};

class FRHIDepthRenderTargetView
{
public:
	FTextureRHIParamRef Texture;

	ERenderTargetLoadAction		DepthLoadAction;
	ERenderTargetStoreAction	DepthStoreAction;
	ERenderTargetLoadAction		StencilLoadAction;

private:
	ERenderTargetStoreAction	StencilStoreAction;
	FExclusiveDepthStencil		DepthStencilAccess;
public:

	// accessor to prevent write access to StencilStoreAction
	ERenderTargetStoreAction GetStencilStoreAction() const { return StencilStoreAction; }
	// accessor to prevent write access to DepthStencilAccess
	FExclusiveDepthStencil GetDepthStencilAccess() const { return DepthStencilAccess; }

	FRHIDepthRenderTargetView() :
		Texture(nullptr),
		DepthLoadAction(ERenderTargetLoadAction::EClear),
		DepthStoreAction(ERenderTargetStoreAction::EStore),
		StencilLoadAction(ERenderTargetLoadAction::EClear),
		StencilStoreAction(ERenderTargetStoreAction::EStore),
		DepthStencilAccess(FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		Validate();
	}

	FRHIDepthRenderTargetView(FTextureRHIParamRef InTexture) :
		Texture(InTexture),
		DepthLoadAction(ERenderTargetLoadAction::EClear),
		DepthStoreAction(ERenderTargetStoreAction::EStore),
		StencilLoadAction(ERenderTargetLoadAction::EClear),
		StencilStoreAction(ERenderTargetStoreAction::EStore),
		DepthStencilAccess(FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		Validate();
	}

	FRHIDepthRenderTargetView(FTextureRHIParamRef InTexture, ERenderTargetLoadAction InLoadAction, ERenderTargetStoreAction InStoreAction) :
		Texture(InTexture),
		DepthLoadAction(InLoadAction),
		DepthStoreAction(InStoreAction),
		StencilLoadAction(InLoadAction),
		StencilStoreAction(InStoreAction),
		DepthStencilAccess(FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		Validate();
	}

	FRHIDepthRenderTargetView(FTextureRHIParamRef InTexture, ERenderTargetLoadAction InLoadAction, ERenderTargetStoreAction InStoreAction, FExclusiveDepthStencil InDepthStencilAccess) :
		Texture(InTexture),
		DepthLoadAction(InLoadAction),
		DepthStoreAction(InStoreAction),
		StencilLoadAction(InLoadAction),
		StencilStoreAction(InStoreAction),
		DepthStencilAccess(InDepthStencilAccess)
	{
		Validate();
	}

	FRHIDepthRenderTargetView(FTextureRHIParamRef InTexture, ERenderTargetLoadAction InDepthLoadAction, ERenderTargetStoreAction InDepthStoreAction, ERenderTargetLoadAction InStencilLoadAction, ERenderTargetStoreAction InStencilStoreAction) :
		Texture(InTexture),
		DepthLoadAction(InDepthLoadAction),
		DepthStoreAction(InDepthStoreAction),
		StencilLoadAction(InStencilLoadAction),
		StencilStoreAction(InStencilStoreAction),
		DepthStencilAccess(FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		Validate();
	}

	FRHIDepthRenderTargetView(FTextureRHIParamRef InTexture, ERenderTargetLoadAction InDepthLoadAction, ERenderTargetStoreAction InDepthStoreAction, ERenderTargetLoadAction InStencilLoadAction, ERenderTargetStoreAction InStencilStoreAction, FExclusiveDepthStencil InDepthStencilAccess) :
		Texture(InTexture),
		DepthLoadAction(InDepthLoadAction),
		DepthStoreAction(InDepthStoreAction),
		StencilLoadAction(InStencilLoadAction),
		StencilStoreAction(InStencilStoreAction),
		DepthStencilAccess(InDepthStencilAccess)
	{
		Validate();
	}

	void Validate() const
	{		
	}

	bool operator==(const FRHIDepthRenderTargetView& Other)
	{
		return
			Texture == Other.Texture &&
			DepthLoadAction == Other.DepthLoadAction &&
			DepthStoreAction == Other.DepthStoreAction &&
			StencilLoadAction == Other.StencilLoadAction &&
			StencilStoreAction == Other.StencilStoreAction &&
			DepthStencilAccess == Other.DepthStencilAccess;
	}
};




class FRHISetRenderTargetsInfo
{
public:
	// Color Render Targets Info
	FRHIRenderTargetView ColorRenderTarget[MaxSimultaneousRenderTargets];	
	int32 NumColorRenderTargets;
	bool bClearColor;

	// Depth/Stencil Render Target Info
	FRHIDepthRenderTargetView DepthStencilRenderTarget;	
	bool bClearDepth;
	bool bClearStencil;

	FRHISetRenderTargetsInfo() :
		NumColorRenderTargets(0),
		bClearColor(false),		
		bClearDepth(false),
		bClearStencil(false)
	{}

	FRHISetRenderTargetsInfo(int32 InNumColorRenderTargets, const FRHIRenderTargetView* InColorRenderTargets, const FRHIDepthRenderTargetView& InDepthStencilRenderTarget) :
		NumColorRenderTargets(InNumColorRenderTargets),
		bClearColor(InNumColorRenderTargets > 0 && InColorRenderTargets[0].LoadAction == ERenderTargetLoadAction::EClear),
		DepthStencilRenderTarget(InDepthStencilRenderTarget),		
		bClearDepth(InDepthStencilRenderTarget.Texture && InDepthStencilRenderTarget.DepthLoadAction == ERenderTargetLoadAction::EClear),
		bClearStencil(InDepthStencilRenderTarget.Texture && InDepthStencilRenderTarget.StencilLoadAction == ERenderTargetLoadAction::EClear)
	{
		for (int32 Index = 0; Index < InNumColorRenderTargets; ++Index)
		{
			ColorRenderTarget[Index] = InColorRenderTargets[Index];			
		}
	}
	// @todo metal mrt: This can go away after all the cleanup is done
	void SetClearDepthStencil(bool bInClearDepth, bool bInClearStencil = false)
	{
		if (bInClearDepth)
		{
			DepthStencilRenderTarget.DepthLoadAction = ERenderTargetLoadAction::EClear;
		}
		if (bInClearStencil)
		{
			DepthStencilRenderTarget.StencilLoadAction = ERenderTargetLoadAction::EClear;
		}
		bClearDepth = bInClearDepth;		
		bClearStencil = bInClearStencil;		
	}
};


class FRHIRenderTarget {
public:
	FRHISetRenderTargetsInfo Info;

};

class FRHICustomPresent : public FRHIResource
{
public:
	explicit FRHICustomPresent(FRHIViewport* InViewport) 
		: FRHIResource()
		, ViewportRHI(InViewport) 
	{
	}
	
	virtual ~FRHICustomPresent() {} // should release any references to D3D resources.

	virtual void OnBackBufferResize() = 0;

	// @param InOutSyncInterval - in out param, indicates if vsync is on (>0) or off (==0).
	// @return	true if normal Present should be performed; false otherwise. If it returns
	// true, then InOutSyncInterval could be modified to switch between VSync/NoVSync for the normal Present.
	virtual bool Present(int32& InOutSyncInterval) = 0;

	// Called when rendering thread is acquired
	virtual void OnAcquireThreadOwnership() {}
	// Called when rendering thread is released
	virtual void OnReleaseThreadOwnership() {}

protected:
	// Weak reference, don't create a circular dependency that would prevent the viewport from being destroyed.
	FRHIViewport* ViewportRHI;
};


typedef FRHICustomPresent*              FCustomPresentRHIParamRef;
typedef std::shared_ptr<FRHICustomPresent> FCustomPresentRHIRef;

struct FBoundBufferDesc
{

	FRHIIndexBuffer* IndexBuffer;
	FRHIVertexBuffer* VertexBuffers[MaxVertexElementCount];
	uint32 Offsets[MaxVertexElementCount];
	uint32 Strides[MaxVertexElementCount];
	FBoundBufferDesc() {
		IndexBuffer = NULL;
		for (int i = 0; i < MaxVertexElementCount; i++) {
			VertexBuffers[i] = NULL;
			Offsets[i] = 0;
			Strides[i]=0;
		}

	}
};
