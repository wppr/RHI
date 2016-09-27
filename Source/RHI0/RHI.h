#pragma once
#include "PreDefine.h"
#include "unreal_defination.h"
#include "RHIResources.h"
#include "Color.h"
#include<vector>
#include <array>
using std::array;
using std::vector;

struct FVertexElement
{
	uint8 StreamIndex;
	uint8 Offset;
	EVertexElementType Type;
	uint8 AttributeIndex;
	uint16 Stride;
	uint16 bUseInstanceIndex;
	FVertexElement(uint8 InAttributeIndex, EVertexElementType InType) :
		StreamIndex(0),
		Offset(0),
		Type(InType),
		AttributeIndex(InAttributeIndex),
		Stride(0),
		bUseInstanceIndex(0)
	{}

	FVertexElement() {}
	FVertexElement(uint8 InStreamIndex, uint8 InOffset, EVertexElementType InType, uint8 InAttributeIndex, uint16 InStride, bool bInUseInstanceIndex = false) :
		StreamIndex(InStreamIndex),
		Offset(InOffset),
		Type(InType),
		AttributeIndex(InAttributeIndex),
		Stride(InStride),
		bUseInstanceIndex(bInUseInstanceIndex)
	{}
};
typedef vector<FVertexElement> FVertexDeclarationElementList;

struct FSamplerStateInitializerRHI
{
	FSamplerStateInitializerRHI() {}
	FSamplerStateInitializerRHI(
		ESamplerFilter InFilter,
		ESamplerAddressMode InAddressU = AM_Wrap,
		ESamplerAddressMode InAddressV = AM_Wrap,
		ESamplerAddressMode InAddressW = AM_Wrap,
		int32 InMipBias = 0,
		int32 InMaxAnisotropy = 0,
		float InMinMipLevel = 0,
		float InMaxMipLevel = FLT_MAX,
		uint32 InBorderColor = 0,
		/** Only supported in D3D11 */
		ESamplerCompareFunction InSamplerComparisonFunction = SCF_Never
		)
		: Filter(InFilter)
		, AddressU(InAddressU)
		, AddressV(InAddressV)
		, AddressW(InAddressW)
		, MipBias(InMipBias)
		, MinMipLevel(InMinMipLevel)
		, MaxMipLevel(InMaxMipLevel)
		, MaxAnisotropy(InMaxAnisotropy)
		, BorderColor(InBorderColor)
		, SamplerComparisonFunction(InSamplerComparisonFunction)
	{
	}
	ESamplerFilter Filter;
	ESamplerAddressMode AddressU;
	ESamplerAddressMode AddressV;
	ESamplerAddressMode AddressW;
	int32 MipBias;
	/** Smallest mip map level that will be used, where 0 is the highest resolution mip level. */
	float MinMipLevel;
	/** Largest mip map level that will be used, where 0 is the highest resolution mip level. */
	float MaxMipLevel;
	int32 MaxAnisotropy;
	uint32 BorderColor;
	ESamplerCompareFunction SamplerComparisonFunction;


};
struct FRasterizerStateInitializerRHI
{
	ERasterizerFillMode FillMode;
	ERasterizerCullMode CullMode;
	float DepthBias;
	float SlopeScaleDepthBias;
	bool bAllowMSAA;
	bool bEnableLineAA;

};
struct FDepthStencilStateInitializerRHI
{
	bool bEnableDepthWrite;
	ECompareFunction DepthTest;

	bool bEnableFrontFaceStencil;
	ECompareFunction  FrontFaceStencilTest;
	EStencilOp  FrontFaceStencilFailStencilOp;
	EStencilOp  FrontFaceDepthFailStencilOp;
	EStencilOp  FrontFacePassStencilOp;
	bool bEnableBackFaceStencil;
	ECompareFunction  BackFaceStencilTest;
	EStencilOp  BackFaceStencilFailStencilOp;
	EStencilOp  BackFaceDepthFailStencilOp;
	EStencilOp  BackFacePassStencilOp;
	uint8 StencilReadMask;
	uint8 StencilWriteMask;

	FDepthStencilStateInitializerRHI(
		bool bInEnableDepthWrite = true,
		ECompareFunction InDepthTest = CF_LessEqual,
		bool bInEnableFrontFaceStencil = false,
		ECompareFunction InFrontFaceStencilTest = CF_Always,
		EStencilOp InFrontFaceStencilFailStencilOp = SO_Keep,
		EStencilOp InFrontFaceDepthFailStencilOp = SO_Keep,
		EStencilOp InFrontFacePassStencilOp = SO_Keep,
		bool bInEnableBackFaceStencil = false,
		ECompareFunction InBackFaceStencilTest = CF_Always,
		EStencilOp InBackFaceStencilFailStencilOp = SO_Keep,
		EStencilOp InBackFaceDepthFailStencilOp = SO_Keep,
		EStencilOp InBackFacePassStencilOp = SO_Keep,
		uint8 InStencilReadMask = 0xFF,
		uint8 InStencilWriteMask = 0xFF
		)
		: bEnableDepthWrite(bInEnableDepthWrite)
		, DepthTest(InDepthTest)
		, bEnableFrontFaceStencil(bInEnableFrontFaceStencil)
		, FrontFaceStencilTest(InFrontFaceStencilTest)
		, FrontFaceStencilFailStencilOp(InFrontFaceStencilFailStencilOp)
		, FrontFaceDepthFailStencilOp(InFrontFaceDepthFailStencilOp)
		, FrontFacePassStencilOp(InFrontFacePassStencilOp)
		, bEnableBackFaceStencil(bInEnableBackFaceStencil)
		, BackFaceStencilTest(InBackFaceStencilTest)
		, BackFaceStencilFailStencilOp(InBackFaceStencilFailStencilOp)
		, BackFaceDepthFailStencilOp(InBackFaceDepthFailStencilOp)
		, BackFacePassStencilOp(InBackFacePassStencilOp)
		, StencilReadMask(InStencilReadMask)
		, StencilWriteMask(InStencilWriteMask)
	{}
};
class FBlendStateInitializerRHI
{
public:

	struct FRenderTarget
	{
		EBlendOperation  ColorBlendOp;
		EBlendFactor  ColorSrcBlend;
		EBlendFactor  ColorDestBlend;
		EBlendOperation  AlphaBlendOp;
		EBlendFactor  AlphaSrcBlend;
		EBlendFactor  AlphaDestBlend;
		EColorWriteMask  ColorWriteMask;

		FRenderTarget(
			EBlendOperation InColorBlendOp = BO_Add,
			EBlendFactor InColorSrcBlend = BF_One,
			EBlendFactor InColorDestBlend = BF_Zero,
			EBlendOperation InAlphaBlendOp = BO_Add,
			EBlendFactor InAlphaSrcBlend = BF_One,
			EBlendFactor InAlphaDestBlend = BF_Zero,
			EColorWriteMask InColorWriteMask = CW_RGBA
			)
			: ColorBlendOp(InColorBlendOp)
			, ColorSrcBlend(InColorSrcBlend)
			, ColorDestBlend(InColorDestBlend)
			, AlphaBlendOp(InAlphaBlendOp)
			, AlphaSrcBlend(InAlphaSrcBlend)
			, AlphaDestBlend(InAlphaDestBlend)
			, ColorWriteMask(InColorWriteMask)
		{}

	};

	FBlendStateInitializerRHI() {}

	FBlendStateInitializerRHI(const FRenderTarget& InRenderTargetBlendState)
		: bUseIndependentRenderTargetBlendStates(false)
	{
		RenderTargets[0] = InRenderTargetBlendState;
	}

	//template<uint32 NumRenderTargets>
	//FBlendStateInitializerRHI(const TStaticArray<FRenderTarget, NumRenderTargets>& InRenderTargetBlendStates)
	//	: bUseIndependentRenderTargetBlendStates(NumRenderTargets > 1)
	//{
	//	static_assert(NumRenderTargets <= MaxSimultaneousRenderTargets, "Too many render target blend states.");

	//	for (uint32 RenderTargetIndex = 0; RenderTargetIndex < NumRenderTargets; ++RenderTargetIndex)
	//	{
	//		RenderTargets[RenderTargetIndex] = InRenderTargetBlendStates[RenderTargetIndex];
	//	}
	//}

	std::vector<FRenderTarget> RenderTargets;
	bool bUseIndependentRenderTargetBlendStates;

};

/**
*	Screen Resolution
*/
struct FScreenResolutionRHI
{
	uint32	Width;
	uint32	Height;
	uint32	RefreshRate;
};

/**
*	Viewport bounds structure to set multiple view ports for the geometry shader
*  (needs to be 1:1 to the D3D11 structure)
*/
struct FViewportBounds
{
	float	TopLeftX;
	float	TopLeftY;
	float	Width;
	float	Height;
	float	MinDepth;
	float	MaxDepth;

	FViewportBounds() {}

	FViewportBounds(float InTopLeftX, float InTopLeftY, float InWidth, float InHeight, float InMinDepth = 0.0f, float InMaxDepth = 1.0f)
		:TopLeftX(InTopLeftX), TopLeftY(InTopLeftY), Width(InWidth), Height(InHeight), MinDepth(InMinDepth), MaxDepth(InMaxDepth)
	{
	}


};


struct FVRamAllocation
{
	FVRamAllocation(uint32 InAllocationStart = 0, uint32 InAllocationSize = 0)
		: AllocationStart(InAllocationStart)
		, AllocationSize(InAllocationSize)
	{
	}

	bool IsValid() { return AllocationSize > 0; }

	// in bytes
	uint32 AllocationStart;
	// in bytes
	uint32 AllocationSize;
};

struct FRHIResourceInfo
{
	FVRamAllocation VRamAllocation;
};

enum class EClearBinding
{
	ENoneBound, //no clear color associated with this target.  Target will not do hardware clears on most platforms
	EColorBound, //target has a clear color bound.  Clears will use the bound color, and do hardware clears.
	EDepthStencilBound, //target has a depthstencil value bound.  Clears will use the bound values and do hardware clears.
};

class FClearValueBinding
{
public:
	struct DSVAlue
	{
		float Depth;
		uint32 Stencil;
	};

	FClearValueBinding()
		: ColorBinding(EClearBinding::EColorBound)
	{
		Value.Color[0] = 0.0f;
		Value.Color[1] = 0.0f;
		Value.Color[2] = 0.0f;
		Value.Color[3] = 0.0f;
	}

	FClearValueBinding(EClearBinding NoBinding)
		: ColorBinding(NoBinding)
	{
	}

	explicit FClearValueBinding(const FLinearColor& InClearColor)
		: ColorBinding(EClearBinding::EColorBound)
	{
		Value.Color[0] = InClearColor.R;
		Value.Color[1] = InClearColor.G;
		Value.Color[2] = InClearColor.B;
		Value.Color[3] = InClearColor.A;
	}

	explicit FClearValueBinding(float DepthClearValue, uint32 StencilClearValue = 0)
		: ColorBinding(EClearBinding::EDepthStencilBound)
	{
		Value.DSValue.Depth = DepthClearValue;
		Value.DSValue.Stencil = StencilClearValue;
	}

	FLinearColor GetClearColor() const
	{
		return FLinearColor(Value.Color[0], Value.Color[1], Value.Color[2], Value.Color[3]);
	}

	void GetDepthStencil(float& OutDepth, uint32& OutStencil) const
	{
		OutDepth = Value.DSValue.Depth;
		OutStencil = Value.DSValue.Stencil;
	}

	bool operator==(const FClearValueBinding& Other) const
	{
		if (ColorBinding == Other.ColorBinding)
		{
			if (ColorBinding == EClearBinding::EColorBound)
			{
				return
					Value.Color[0] == Other.Value.Color[0] &&
					Value.Color[1] == Other.Value.Color[1] &&
					Value.Color[2] == Other.Value.Color[2] &&
					Value.Color[3] == Other.Value.Color[3];

			}
			if (ColorBinding == EClearBinding::EDepthStencilBound)
			{
				return
					Value.DSValue.Depth == Value.DSValue.Depth &&
					Value.DSValue.Stencil == Value.DSValue.Stencil;
			}
			return true;
		}
		return false;
	}

	EClearBinding ColorBinding;

	union ClearValueType
	{
		float Color[4];
		DSVAlue DSValue;
	} Value;

	// common clear values
	static  const FClearValueBinding None;
	static  const FClearValueBinding Black;
	static  const FClearValueBinding White;
	static  const FClearValueBinding Transparent;
	static  const FClearValueBinding DepthOne;
	static  const FClearValueBinding DepthZero;
	static  const FClearValueBinding DepthNear;
	static  const FClearValueBinding DepthFar;
};

struct FRHIResourceCreateInfo
{
	FRHIResourceCreateInfo()
		: BulkData()
		, ResourceArray(nullptr)
		, ClearValueBinding(FLinearColor::Transparent)
	{}

	// for CreateTexture calls
	FRHIResourceCreateInfo(vector<void*> InBulkData)
		: BulkData(InBulkData)
		, ResourceArray()
		, ClearValueBinding(FLinearColor::Transparent)
	{}

	// for CreateVertexBuffer/CreateStructuredBuffer calls
	FRHIResourceCreateInfo(void* InResourceArray)
		: BulkData()
		, ResourceArray(InResourceArray)
		, ClearValueBinding(FLinearColor::Transparent)
	{}

	FRHIResourceCreateInfo(const FClearValueBinding& InClearValueBinding)
		: BulkData()
		, ResourceArray(nullptr)
		, ClearValueBinding(InClearValueBinding)
	{
	}

	// for CreateTexture calls
	vector<void*> BulkData;
	// for CreateVertexBuffer/CreateStructuredBuffer calls
	void* ResourceArray;

	// for binding clear colors to rendertargets.
	FClearValueBinding ClearValueBinding;
};
struct FIntPoint
{
	/** Holds the point's x-coordinate. */
	int32 X;

	/** Holds the point's y-coordinate. */
	int32 Y;
	FIntPoint(){}
	FIntPoint(int32 InX, int32 InY)
		: X(InX)
		, Y(InY)
	{ }
};

struct FIntRect
{
	/** Holds the first pixel line/row (like in Win32 RECT). */
	FIntPoint Min;
	FIntPoint Max;

	FIntRect(FIntPoint l, FIntPoint r) :
		Min(l),
		Max(r)
	{}

	FIntRect(){}
};

