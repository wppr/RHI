// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	OpenGLResources.h: OpenGL resource RHI definitions.
=============================================================================*/

#pragma once

//#include "BoundShaderStateCache.h"
#include "OpenGLShaderResources.h"
#include "OpenGLRHI.h"
#include <array>
#include <string>

using std::array;
typedef void (*BufferBindFunction)( GLuint Buffer );

template <typename BaseType, GLenum Type, BufferBindFunction BufBind>
class TOpenGLBuffer : public BaseType
{
	GLenum GetAccess()
	{
		// Previously there was special-case logic to always use GL_STATIC_DRAW for vertex buffers allocated from staging buffer.
		// However it seems to be incorrect as NVidia drivers complain (via debug output callback) about VIDEO->HOST copying for buffers with such hints
		return bStreamDraw ? GL_STREAM_DRAW : (IsDynamic() ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}
public:

	GLuint Resource=0;
	GLuint ResourceUsed=false;
	bool bStreamDraw = false;
	TOpenGLBuffer(uint32 InStride, uint32 InSize, uint32 InUsage,
		const void *InData = NULL, GLuint ResourceToUse = 0, uint32 ResourceSize = 0)
		: BaseType(InStride, InSize, InUsage)
		, Resource(0)
		, RealSize(InSize)
	{
		RealSize = ResourceSize ? ResourceSize : InSize;
		if (ResourceUsed)
		{
			Bind();
			glBufferSubData(Type, 0, InSize, InData);
			
		}
		else
		{
			glGenBuffers(1, &Resource);
			ResourceUsed = true;
			Bind();
			glBufferData(Type, RealSize, InData, GetAccess());

		}

	}

	virtual ~TOpenGLBuffer()
	{
		if (Resource != 0)
		{
			glDeleteBuffers(1, &Resource);
		}
	}

	void Bind()
	{
		glBindBuffer(Type,Resource);
	}

	void Update(void *InData, uint32 InOffset, uint32 InSize, bool bDiscard)
	{
		Bind();
		glBufferSubData(Type, InOffset, InSize, InData);
	}

	bool IsDynamic() const { return (this->GetUsage() & BUF_AnyDynamic) != 0; }
private:

	uint32 RealSize;	// sometimes (for example, for uniform buffer pool) we allocate more in OpenGL than is requested of us.
};

//class FOpenGLBasePixelBuffer : public FRefCountedObject
//{
//public:
//	FOpenGLBasePixelBuffer(uint32 InStride,uint32 InSize,uint32 InUsage)
//	: Size(InSize)
//	, Usage(InUsage)
//	{}
//	static bool OnDelete(GLuint Resource,uint32 Size,bool bStreamDraw,uint32 Offset)
//	{
//		OnPixelBufferDeletion(Resource);
//		return true;
//	}
//	uint32 GetSize() const { return Size; }
//	uint32 GetUsage() const { return Usage; }
//
//	static FORCEINLINE bool GLSupportsType()
//	{
//		return FOpenGL::SupportsPixelBuffers();
//	}
//
//	static void CreateType(GLuint& Resource, const void* InData, uint32 InSize)
//	{
//		// @todo-mobile
//	}
//
//	static bool IsStructuredBuffer() { return false; }
//
//private:
//	uint32 Size;
//	uint32 Usage;
//};

class FOpenGLBaseVertexBuffer : public FRHIVertexBuffer
{
public:
	FOpenGLBaseVertexBuffer(uint32 InStride,uint32 InSize,uint32 InUsage): FRHIVertexBuffer(InSize,InUsage), ZeroStrideVertexBuffer(0)
	{
		//if(!(FOpenGL::SupportsVertexAttribBinding() && OpenGLConsoleVariables::bUseVAB) && InUsage & BUF_ZeroStride )
		//{
		//	ZeroStrideVertexBuffer = FMemory::Malloc( InSize );
		//}
	}

	~FOpenGLBaseVertexBuffer( void )
	{
		//if( ZeroStrideVertexBuffer )
		//{
		//	FMemory::Free( ZeroStrideVertexBuffer );
		//}
	}

	void* GetZeroStrideBuffer( void )
	{
		return ZeroStrideVertexBuffer;
	}

	static bool OnDelete(GLuint Resource,uint32 Size,bool bStreamDraw,uint32 Offset)
	{
		//OnVertexBufferDeletion(Resource);
		return true;
	}

	static FORCEINLINE bool GLSupportsType()
	{
		return true;
	}

	static void CreateType(GLuint& Resource, const void* InData, uint32 InSize)
	{
		// @todo-mobile
	}

	static bool IsStructuredBuffer() { return false; }

private:
	void*	ZeroStrideVertexBuffer;
};

//struct FOpenGLEUniformBufferData : public FRefCountedObject
//{
//	FOpenGLEUniformBufferData(uint32 SizeInBytes)
//	{
//		uint32 SizeInUint32s = (SizeInBytes + 3) / 4;
//		Data.Empty(SizeInUint32s);
//		Data.AddUninitialized(SizeInUint32s);
//		IncrementBufferMemory(GL_UNIFORM_BUFFER,false,Data.GetAllocatedSize());
//	}
//
//	~FOpenGLEUniformBufferData()
//	{
//		DecrementBufferMemory(GL_UNIFORM_BUFFER,false,Data.GetAllocatedSize());
//	}
//
//	TArray<uint32> Data;
//};
//typedef shared_ptr<FOpenGLEUniformBufferData> FOpenGLEUniformBufferDataRef;
//
//class FOpenGLUniformBuffer : public FRHIUniformBuffer
//{
//public:
//	/** The GL resource for this uniform buffer. */
//	GLuint Resource;
//
//	/** The offset of the uniform buffer's contents in the resource. */
//	uint32 Offset;
//
//	/** When using a persistently mapped buffer this is a pointer to the CPU accessible data. */
//	uint8* PersistentlyMappedBuffer;
//
//	/** Unique ID for state shadowing purposes. */
//	const uint32 UniqueID;
//
//	/** Resource table containing RHI references. */
//	TArray<shared_ptr<FRHIResource> > ResourceTable;
//
//	/** Raw resource table, cached once per frame. */
//	TArray<void*> RawResourceTable;
//
//	/** The frame in which RawResourceTable was last cached. */
//	uint32 LastCachedFrame;
//
//	/** Emulated uniform data for ES2. */
//	FOpenGLEUniformBufferDataRef EmulatedBufferData;
//
//	/** The size of the buffer allocated to hold the uniform buffer contents. May be larger than necessary. */
//	uint32 AllocatedSize;
//
//	/** True if the uniform buffer is not used across frames. */
//	bool bStreamDraw;
//
//	/** Initialization constructor. */
//	FOpenGLUniformBuffer(const FRHIUniformBufferLayout& InLayout, GLuint InResource, uint32 InOffset, uint8* InPersistentlyMappedBuffer, uint32 InAllocatedSize, FOpenGLEUniformBufferDataRef& InEmulatedBuffer, bool bInStreamDraw);
//
//	/** Destructor. */
//	~FOpenGLUniformBuffer();
//
//	/** Cache resources if needed. */
//	inline void CacheResources(uint32 InFrameCounter)
//	{
//		if (InFrameCounter == INDEX_NONE || LastCachedFrame != InFrameCounter)
//		{
//			CacheResourcesInternal();
//			LastCachedFrame = InFrameCounter;
//		}
//	}
//
//private:
//	/** Actually cache resources. */
//	void CacheResourcesInternal();
//};


class FOpenGLBaseIndexBuffer : public FRHIIndexBuffer
{
public:
	FOpenGLBaseIndexBuffer(uint32 InStride,uint32 InSize,uint32 InUsage): FRHIIndexBuffer(InStride,InSize,InUsage) {}
	static bool OnDelete(GLuint Resource,uint32 Size,bool bStreamDraw,uint32 Offset)
	{
		return true;
	}

	static FORCEINLINE bool GLSupportsType()
	{
		return true;
	}

	static void CreateType(GLuint& Resource, const void* InData, uint32 InSize)
	{
		// @todo-mobile
	}

	static bool IsStructuredBuffer() { return false; }
};
//
//class FOpenGLBaseStructuredBuffer : public FRHIStructuredBuffer
//{
//public:
//	FOpenGLBaseStructuredBuffer(uint32 InStride,uint32 InSize,uint32 InUsage): FRHIStructuredBuffer(InStride,InSize,InUsage) {}
//	static bool OnDelete(GLuint Resource,uint32 Size,bool bStreamDraw,uint32 Offset)
//	{
//		OnVertexBufferDeletion(Resource);
//		return true;
//	}
//
//	static FORCEINLINE bool GLSupportsType()
//	{
//		return FOpenGL::SupportsStructuredBuffers();
//	}
//
//	static void CreateType(GLuint& Resource, const void* InData, uint32 InSize)
//	{
//		// @todo-mobile
//	}
//
//	static bool IsStructuredBuffer() { return true; }
//};

//typedef TOpenGLBuffer<FOpenGLBasePixelBuffer, GL_PIXEL_UNPACK_BUFFER, CachedBindPixelUnpackBuffer> FOpenGLPixelBuffer;
typedef TOpenGLBuffer<FOpenGLBaseVertexBuffer, GL_ARRAY_BUFFER, NULL> FOpenGLVertexBuffer;
typedef TOpenGLBuffer<FOpenGLBaseIndexBuffer,GL_ELEMENT_ARRAY_BUFFER,NULL> FOpenGLIndexBuffer;
//typedef TOpenGLBuffer<FOpenGLBaseStructuredBuffer,GL_ARRAY_BUFFER,CachedBindArrayBuffer> FOpenGLStructuredBuffer;

#define MAX_STREAMED_BUFFERS_IN_ARRAY 2	// must be > 1!
#define MIN_DRAWS_IN_SINGLE_BUFFER 16

template <typename BaseType, uint32 Stride>
class TOpenGLStreamedBufferArray
{
public:

	TOpenGLStreamedBufferArray( void ) {}
	virtual ~TOpenGLStreamedBufferArray( void ) {}

	void Init( uint32 InitialBufferSize )
	{
		for( int32 BufferIndex = 0; BufferIndex < MAX_STREAMED_BUFFERS_IN_ARRAY; ++BufferIndex )
		{
			Buffer[BufferIndex] = new BaseType(Stride, InitialBufferSize, BUF_Volatile, NULL, true);
		}
		CurrentBufferIndex = 0;
		CurrentOffset = 0;
		LastOffset = 0;
		MinNeededBufferSize = InitialBufferSize / MIN_DRAWS_IN_SINGLE_BUFFER;
	}

	void Cleanup( void )
	{
		for( int32 BufferIndex = 0; BufferIndex < MAX_STREAMED_BUFFERS_IN_ARRAY; ++BufferIndex )
		{
			Buffer[BufferIndex].SafeRelease();
		}
	}

	uint8* Lock( uint32 DataSize )
	{
		check(!Buffer[CurrentBufferIndex]->IsLocked());
		DataSize = Align(DataSize, (1<<8));	// to keep the speed up, let's start data for each next draw at 256-byte aligned offset

		// Keep our dynamic buffers at least MIN_DRAWS_IN_SINGLE_BUFFER times bigger than max single request size
		uint32 NeededBufSize = Align( MIN_DRAWS_IN_SINGLE_BUFFER * DataSize, (1 << 20) );	// 1MB increments
		if (NeededBufSize > MinNeededBufferSize)
		{
			MinNeededBufferSize = NeededBufSize;
		}

		// Check if we need to switch buffer, as the current draw data won't fit in current one
		bool bDiscard = false;
		if (Buffer[CurrentBufferIndex]->GetSize() < CurrentOffset + DataSize)
		{
			// We do.
			++CurrentBufferIndex;
			if (CurrentBufferIndex == MAX_STREAMED_BUFFERS_IN_ARRAY)
			{
				CurrentBufferIndex = 0;
			}
			CurrentOffset = 0;

			// Check if we should extend the next buffer, as max request size has changed
			if (MinNeededBufferSize > Buffer[CurrentBufferIndex]->GetSize())
			{
				Buffer[CurrentBufferIndex].SafeRelease();
				Buffer[CurrentBufferIndex] = new BaseType(Stride, MinNeededBufferSize, BUF_Volatile);
			}

			bDiscard = true;
		}

		LastOffset = CurrentOffset;
		CurrentOffset += DataSize;

		return Buffer[CurrentBufferIndex]->LockWriteOnlyUnsynchronized(LastOffset, DataSize, bDiscard);
	}

	void Unlock( void )
	{
		check(Buffer[CurrentBufferIndex]->IsLocked());
		Buffer[CurrentBufferIndex]->Unlock();
	}

	BaseType* GetPendingBuffer( void ) { return Buffer[CurrentBufferIndex]; }
	uint32 GetPendingOffset( void ) { return LastOffset; }

private:
	shared_ptr<BaseType> Buffer[MAX_STREAMED_BUFFERS_IN_ARRAY];
	uint32 CurrentBufferIndex;
	uint32 CurrentOffset;
	uint32 LastOffset;
	uint32 MinNeededBufferSize;
};

typedef TOpenGLStreamedBufferArray<FOpenGLVertexBuffer,0> FOpenGLStreamedVertexBufferArray;
//typedef TOpenGLStreamedBufferArray<FOpenGLIndexBuffer,sizeof(uint16)> FOpenGLStreamedIndexBufferArray;

struct FOpenGLVertexElement
{
	GLenum Type;
	GLuint StreamIndex;
	GLuint Offset;
	GLuint Size;
	GLuint Divisor;
	uint8 bNormalized;
	uint8 AttributeIndex;
	uint8 bShouldConvertToFloat;
	bool bValid = false;
	FOpenGLVertexElement() :
		Type(0),
		StreamIndex(0),
		Offset(0),
		Size(0),
		Divisor(0),
		bNormalized(0),
		AttributeIndex(0),
		bShouldConvertToFloat(0)
	{
	}
};

/** Convenience typedef: preallocated array of OpenGL input element descriptions. */
typedef array<FOpenGLVertexElement,MaxVertexElementCount> FOpenGLVertexElements;

/** This represents a vertex declaration that hasn't been combined with a specific shader to create a bound shader. */
class FOpenGLVertexDeclaration : public FRHIVertexDeclaration
{
public:
	/** Elements of the vertex declaration. */
	FOpenGLVertexElements VertexElements;

	/** Initialization constructor. */
	explicit FOpenGLVertexDeclaration(const FOpenGLVertexElements& InElements)
		: VertexElements(InElements)
	{
	}
};


/**
 * Combined shader state and vertex definition for rendering geometry. 
 * Each unique instance consists of a vertex decl, vertex shader, and pixel shader.
 */
class FOpenGLBoundShaderState : public FRHIBoundShaderState
{
public:

	GLuint ProgramID = -1;
	bool bLinked = false;
	FOpenGLVertexShader* VertexShader;
	FOpenGLPixelShader* PixelShader;


	/** Initialization constructor. */
	FOpenGLBoundShaderState(
		FVertexDeclarationRHIParamRef InVertexDeclarationRHI,
		FVertexShaderRHIParamRef InVertexShaderRHI,
		FPixelShaderRHIParamRef InPixelShaderRHI
		);

	void LinkProgram();
	
	virtual ~FOpenGLBoundShaderState(){}

};


class FOpenGLBoundBufferState : public FRHIBoundBufferState
{
public:
	GLuint VAO;//VAO object in OpengGL
	void Bind() {
		glBindVertexArray(VAO);
	}
	FOpenGLBoundBufferState(FBoundBufferDesc Desc, FRHIVertexDeclaration* VertexDecl) {
		glGenVertexArrays(1, &VAO);
		Bind();
		FOpenGLVertexDeclaration* Decl = static_cast<FOpenGLVertexDeclaration*>(VertexDecl);
		for (int Index = 0; Index < MaxVertexElementCount; Index++) {
			auto VertexElement = Decl->VertexElements[Index];

			if (Desc.VertexBuffers[Index] != NULL&&VertexElement.bValid) {
				FOpenGLVertexBuffer* VertexBuffer = static_cast<FOpenGLVertexBuffer*>(Desc.VertexBuffers[Index]);
				VertexBuffer->Bind();
				if(VertexElement.bShouldConvertToFloat)
					glVertexAttribPointer(Index, VertexElement.Size, VertexElement.Type, VertexElement.bNormalized, 0, NULL);
				else 
					glVertexAttribIPointer(Index, VertexElement.Size, VertexElement.Type, 0, (GLubyte*)NULL);
				glEnableVertexAttribArray(Index);
			}
		}

	}

	~FOpenGLBoundBufferState() {
		glDeleteVertexArrays(1, &VAO);
	}
};


inline GLenum GetOpenGLTargetFromRHITexture(FRHITexture* Texture)
{
	if(!Texture)
	{
		return GL_NONE;
	}
	else if(Texture->GetTexture2D())
	{
		return GL_TEXTURE_2D;
	}
	else if(Texture->GetTexture2DArray())
	{
		return GL_TEXTURE_2D_ARRAY;
	}
	else if(Texture->GetTexture3D())
	{
		return GL_TEXTURE_3D;
	}
	else if(Texture->GetTextureCube())
	{
		return GL_TEXTURE_CUBE_MAP;
	}
	else
	{
		return GL_NONE;
	}
}


class FOpenGLTextureBase
{
protected:
	class FOpenGLDynamicRHI* OpenGLRHI;

public:
	// Pointer to current sampler state in this unit
	class FOpenGLSamplerState* SamplerState;
	
	/** The OpenGL texture resource. */
	GLuint Resource;

	/** The OpenGL texture target. */
	GLenum Target;

	/** The number of mips in the texture. */
	uint32 NumMips;

	/** The OpenGL attachment point. This should always be GL_COLOR_ATTACHMENT0 in case of color buffer, but the actual texture may be attached on other color attachments. */
	GLenum Attachment;

	/** Initialization constructor. */
	FOpenGLTextureBase(
		FOpenGLDynamicRHI* InOpenGLRHI,
		GLuint InResource,
		GLenum InTarget,
		uint32 InNumMips,
		GLenum InAttachment
		)
	: OpenGLRHI(InOpenGLRHI)
	, SamplerState(nullptr)
	, Resource(InResource)
	, Target(InTarget)
	, NumMips(InNumMips)
	, Attachment(InAttachment)
	, MemorySize( 0 )
	, bIsPowerOfTwo(false)
	{}

	int32 GetMemorySize() const
	{
		return MemorySize;
	}

	void SetMemorySize(uint32 InMemorySize)
	{
		MemorySize = InMemorySize;
	}
	
	void SetIsPowerOfTwo(bool bInIsPowerOfTwo)
	{
		bIsPowerOfTwo  = bInIsPowerOfTwo ? 1 : 0;
	}

	bool IsPowerOfTwo() const
	{
		return bIsPowerOfTwo != 0;
	}

#if PLATFORM_MAC || PLATFORM_ANDROIDES31 // Flithy hack to workaround radr://16011763
	GLuint GetOpenGLFramebuffer(uint32 ArrayIndices, uint32 MipmapLevels);
#endif

	void InvalidateTextureResourceInCache();

private:
	uint32 MemorySize		: 31;
	uint32 bIsPowerOfTwo	: 1;
};

// Textures.
template<typename BaseType>
class  TOpenGLTexture : public BaseType, public FOpenGLTextureBase
{
public:

	/** Initialization constructor. */
	TOpenGLTexture(
		class FOpenGLDynamicRHI* InOpenGLRHI,
		GLuint InResource,
		GLenum InTarget,
		GLenum InAttachment,
		uint32 InSizeX,
		uint32 InSizeY,
		uint32 InSizeZ,
		uint32 InNumMips,
		uint32 InNumSamples,
		uint32 InArraySize,
		EPixelFormat InFormat,
		bool bInCubemap,
		bool bInAllocatedStorage,
		uint32 InFlags,
		uint8* InTextureRange,
		const FClearValueBinding& InClearValue
		)
	: BaseType(InSizeX,InSizeY,InSizeZ,InNumMips,InNumSamples, InArraySize, InFormat,InFlags, InClearValue)
	, FOpenGLTextureBase(
		InOpenGLRHI,
		InResource,
		InTarget,
		InNumMips,
		InAttachment
		)
	, TextureRange(InTextureRange)
	, BaseLevel(0)
	, bCubemap(bInCubemap)
	{
		//PixelBuffers.AddZeroed(this->GetNumMips() * (bCubemap ? 6 : 1) * GetEffectiveSizeZ());
		//bAllocatedStorage.Init(bInAllocatedStorage, this->GetNumMips() * (bCubemap ? 6 : 1));
		//ClientStorageBuffers.AddZeroed(this->GetNumMips() * (bCubemap ? 6 : 1) * GetEffectiveSizeZ());
	
		//FShaderCache* ShaderCache = FShaderCache::GetShaderCache();
		//if ( ShaderCache )
		//{
		//	FShaderTextureKey Tex;
		//	Tex.Format = (EPixelFormat)InFormat;
		//	Tex.Flags = InFlags;
		//	Tex.MipLevels = InNumMips;
		//	Tex.Samples = InNumSamples;
		//	Tex.X = InSizeX;
		//	Tex.Y = InSizeY;
		//	Tex.Z = InSizeZ;
		//	switch(InTarget)
		//	{
		//		case GL_TEXTURE_2D:
		//		case GL_TEXTURE_2D_MULTISAMPLE:
		//		{
		//			Tex.Type = SCTT_Texture2D;
		//			break;
		//		}
		//		case GL_TEXTURE_3D:
		//		{
		//			Tex.Type = SCTT_Texture3D;
		//			break;
		//		}
		//		case GL_TEXTURE_CUBE_MAP:
		//		{
		//			Tex.Type = SCTT_TextureCube;
		//			break;
		//		}
		//		case GL_TEXTURE_2D_ARRAY:
		//		{
		//			Tex.Type = SCTT_Texture2DArray;
		//			break;
		//		}
		//		case GL_TEXTURE_CUBE_MAP_ARRAY:
		//		{
		//			Tex.Type = SCTT_TextureCubeArray;
		//			Tex.Z = InArraySize;
		//			break;
		//		}
		//		default:
		//		{
		//			check(false);
		//		}
		//	}
		//
		//	FShaderCache::LogTexture(Tex, this);
		//}
	}

	virtual ~TOpenGLTexture()
	{
		//if (GIsRHIInitialized)
		//{
		//	VERIFY_GL_SCOPE();

		//	OpenGLTextureDeleted(this);

		//	if (Resource != 0)
		//	{
		//		switch (Target)
		//		{
		//			case GL_TEXTURE_2D:
		//			case GL_TEXTURE_2D_MULTISAMPLE:
		//			case GL_TEXTURE_3D:
		//			case GL_TEXTURE_CUBE_MAP:
		//			case GL_TEXTURE_2D_ARRAY:
		//			case GL_TEXTURE_CUBE_MAP_ARRAY:
		//			{
		//				InvalidateTextureResourceInCache();
		//				FOpenGL::DeleteTextures(1, &Resource);
		//				break;
		//			}
		//			case GL_RENDERBUFFER:
		//			{
		//				if (!(this->GetFlags() & TexCreate_Presentable))
		//				{
		//					glDeleteRenderbuffers(1, &Resource);
		//				}
		//				break;
		//			}
		//			default:
		//			{
		//				checkNoEntry();
		//			}
		//		}
		//	}

		//	if (TextureRange)
		//	{
		//		delete[] TextureRange;
		//		TextureRange = nullptr;
		//	}

		//	ReleaseOpenGLFramebuffers(OpenGLRHI, this);
		//}
	}


	void Bind() {
		glBindTexture(Target, Resource);
	}
	/** Updates the host accessible version of the texture */
	void UpdateHost(uint32 MipIndex,uint32 ArrayIndex);

	/** Get PBO Resource for readback */
	GLuint GetBufferResource(uint32 MipIndex,uint32 ArrayIndex);
	
	// Accessors.
	bool IsDynamic() const { return (this->GetFlags() & TexCreate_Dynamic) != 0; }
	bool IsCubemap() const { return bCubemap != 0; }
	bool IsStaging() const { return (this->GetFlags() & TexCreate_CPUReadback) != 0; }


	/** FRHITexture override.  See FRHITexture::GetNativeResource() */
	virtual void* GetNativeResource() const override
	{ 
		return const_cast<void *>(reinterpret_cast<const void*>(&Resource));
	}

	/**
	 * Accessors to mark whether or not we have allocated storage for each mip/face.
	 * For non-cubemaps FaceIndex should always be zero.
	 */
	bool GetAllocatedStorageForMip(uint32 MipIndex, uint32 FaceIndex) const
	{
		return bAllocatedStorage[MipIndex * (bCubemap ? 6 : 1) + FaceIndex];
	}
	void SetAllocatedStorageForMip(uint32 MipIndex, uint32 FaceIndex)
	{
		bAllocatedStorage[MipIndex * (bCubemap ? 6 : 1) + FaceIndex] = true;
	}

	/**
	 * Clone texture from a source using CopyImageSubData
	 */
	void CloneViaCopyImage( TOpenGLTexture* Src, uint32 InNumMips, int32 SrcOffset, int32 DstOffset);
	
	/**
	 * Clone texture from a source going via PBOs
	 */
	void CloneViaPBO( TOpenGLTexture* Src, uint32 InNumMips, int32 SrcOffset, int32 DstOffset);

	/**
	 * Resolved the specified face for a read Lock, for non-renderable, CPU readable surfaces this eliminates the readback inside Lock itself.
	 */
	void Resolve(uint32 MipIndex,uint32 ArrayIndex);
private:

	//TArray< shared_ptr<FOpenGLPixelBuffer> > PixelBuffers;

	/** Backing store for all client storage buffers for platforms and textures types where this is faster than PBOs */
	uint8* TextureRange;

	/** Client storage buffers for platforms and textures types where this is faster than PBOs */
	struct FOpenGLClientStore
	{
		uint8* Data;
		uint32 Size;
		bool bReadOnly;
	};
	//TArray<FOpenGLClientStore> ClientStorageBuffers;

	//uint32 GetEffectiveSizeZ( void ) { return this->GetSizeZ() ? this->GetSizeZ() : 1; }

	///** Index of the largest mip-map in the texture */
	uint32 BaseLevel;

	///** Bitfields marking whether we have allocated storage for each mip */
	//TBitArray<TInlineAllocator<1> > bAllocatedStorage;

	/** Whether the texture is a cube-map. */
	const uint32 bCubemap : 1;
};

class  FOpenGLBaseTexture2D : public FRHITexture2D
{
public:
	FOpenGLBaseTexture2D(uint32 InSizeX, uint32 InSizeY, uint32 InSizeZ, uint32 InNumMips, uint32 InNumSamples, uint32 InArraySize, EPixelFormat InFormat, uint32 InFlags, const FClearValueBinding& InClearValue)
		: FRHITexture2D(InSizeX, InSizeY, InNumMips, InNumSamples, InFormat, InFlags, InClearValue)
		, SampleCount(InNumSamples)
	{}
	uint32 GetSizeZ() const { return 0; }
	uint32 GetNumSamples() const { return SampleCount; }
private:
	uint32 SampleCount;
};

//class FOpenGLBaseTexture2DArray : public FRHITexture2DArray
//{
//public:
//	FOpenGLBaseTexture2DArray(uint32 InSizeX, uint32 InSizeY, uint32 InSizeZ, uint32 InNumMips, uint32 InNumSamples, uint32 InArraySize, EPixelFormat InFormat, uint32 InFlags, const FClearValueBinding& InClearValue)
//	: FRHITexture2DArray(InSizeX,InSizeY,InSizeZ,InNumMips,InFormat,InFlags, InClearValue)
//	{
//		check(InNumSamples == 1);	// OpenGL supports multisampled texture arrays, but they're currently not implemented in OpenGLDrv.
//	}
//};
//
class FOpenGLBaseTextureCube : public FRHITextureCube
{
public:
	FOpenGLBaseTextureCube(uint32 InSizeX, uint32 InSizeY, uint32 InSizeZ, uint32 InNumMips, uint32 InNumSamples, uint32 InArraySize, EPixelFormat InFormat, uint32 InFlags, const FClearValueBinding& InClearValue)
	: FRHITextureCube(InSizeX,InNumMips,InFormat,InFlags,InClearValue)
	, ArraySize(InArraySize)
	{
		assert(InNumSamples == 1);	// OpenGL doesn't currently support multisampled cube textures
	
	}
	uint32 GetSizeX() const { return GetSize(); }
	uint32 GetSizeY() const { return GetSize(); } //-V524
	uint32 GetSizeZ() const { return ArraySize > 1 ? ArraySize : 0; }
	
	uint32 GetArraySize() const {return ArraySize;}
private:
	uint32 ArraySize;
};
//
//class FOpenGLBaseTexture3D : public FRHITexture3D
//{
//public:
//	FOpenGLBaseTexture3D(uint32 InSizeX, uint32 InSizeY, uint32 InSizeZ, uint32 InNumMips, uint32 InNumSamples, uint32 InArraySize, EPixelFormat InFormat, uint32 InFlags, const FClearValueBinding& InClearValue)
//	: FRHITexture3D(InSizeX,InSizeY,InSizeZ,InNumMips,InFormat,InFlags,InClearValue)
//	{
//	}
//};

typedef TOpenGLTexture<FRHITexture>						FOpenGLTexture;
typedef TOpenGLTexture<FOpenGLBaseTexture2D>			FOpenGLTexture2D;
//typedef TOpenGLTexture<FOpenGLBaseTexture2DArray>		FOpenGLTexture2DArray;
//typedef TOpenGLTexture<FOpenGLBaseTexture3D>			FOpenGLTexture3D;
typedef TOpenGLTexture<FOpenGLBaseTextureCube>			FOpenGLTextureCube;
//
//class FOpenGLTextureReference : public FRHITextureReference
//{
//	FOpenGLTextureBase* TexturePtr;
//
//public:
//	explicit FOpenGLTextureReference(FLastRenderTimeContainer* InLastRenderTime)
//		: FRHITextureReference(InLastRenderTime)
//		, TexturePtr(NULL)
//	{}
//
//	void SetReferencedTexture(FRHITexture* InTexture);
//	FOpenGLTextureBase* GetTexturePtr() const { return TexturePtr; }
//};

/** Given a pointer to a RHI texture that was created by the OpenGL RHI, returns a pointer to the FOpenGLTextureBase it encapsulates. */
inline FOpenGLTextureBase* GetOpenGLTextureFromRHITexture(FRHITexture* Texture)
{
	if(!Texture)
	{
		return NULL;
	}
	//else if(Texture->GetTextureReference())
	//{
	//	return ((FOpenGLTextureReference*)Texture)->GetTexturePtr();
	//}
	else if(Texture->GetTexture2D())
	{
		return (FOpenGLTexture2D*)Texture;
	}
	//else if(Texture->GetTexture2DArray())
	//{
	//	return (FOpenGLTexture2DArray*)Texture;
	//}
	//else if(Texture->GetTexture3D())
	//{
	//	return (FOpenGLTexture3D*)Texture;
	//}
	else if(Texture->GetTextureCube())
	{
		return (FOpenGLTextureCube*)Texture;
	}
	else
	{
		return NULL;
	}
}
//
//inline uint32 GetOpenGLTextureSizeXFromRHITexture(FRHITexture* Texture)
//{
//	if(!Texture)
//	{
//		return 0;
//	}
//	else if(Texture->GetTexture2D())
//	{
//		return ((FOpenGLTexture2D*)Texture)->GetSizeX();
//	}
//	else if(Texture->GetTexture2DArray())
//	{
//		return ((FOpenGLTexture2DArray*)Texture)->GetSizeX();
//	}
//	else if(Texture->GetTexture3D())
//	{
//		return ((FOpenGLTexture3D*)Texture)->GetSizeX();
//	}
//	else if(Texture->GetTextureCube())
//	{
//		return ((FOpenGLTextureCube*)Texture)->GetSize();
//	}
//	else
//	{
//		return 0;
//	}
//}
//
//inline uint32 GetOpenGLTextureSizeYFromRHITexture(FRHITexture* Texture)
//{
//	if(!Texture)
//	{
//		return 0;
//	}
//	else if(Texture->GetTexture2D())
//	{
//		return ((FOpenGLTexture2D*)Texture)->GetSizeY();
//	}
//	else if(Texture->GetTexture2DArray())
//	{
//		return ((FOpenGLTexture2DArray*)Texture)->GetSizeY();
//	}
//	else if(Texture->GetTexture3D())
//	{
//		return ((FOpenGLTexture3D*)Texture)->GetSizeY();
//	}
//	else if(Texture->GetTextureCube())
//	{
//		return ((FOpenGLTextureCube*)Texture)->GetSize();
//	}
//	else
//	{
//		return 0;
//	}
//}
//
//inline uint32 GetOpenGLTextureSizeZFromRHITexture(FRHITexture* Texture)
//{
//	if(!Texture)
//	{
//		return 0;
//	}
//	else if(Texture->GetTexture2D())
//	{
//		return 0;
//	}
//	else if(Texture->GetTexture2DArray())
//	{
//		return ((FOpenGLTexture2DArray*)Texture)->GetSizeZ();
//	}
//	else if(Texture->GetTexture3D())
//	{
//		return ((FOpenGLTexture3D*)Texture)->GetSizeZ();
//	}
//	else if(Texture->GetTextureCube())
//	{
//		return ((FOpenGLTextureCube*)Texture)->GetSizeZ();
//	}
//	else
//	{
//		return 0;
//	}
//}
//


class FOpenGLUnorderedAccessView : public FRHIUnorderedAccessView
{

public:
	FOpenGLUnorderedAccessView():
	  Resource(0),
		Format(0)
	{

	}
	  
	GLuint	Resource;
	GLenum	Format;
};

class FOpenGLTextureUnorderedAccessView : public FOpenGLUnorderedAccessView
{
public:

	FOpenGLTextureUnorderedAccessView(FTextureRHIParamRef InTexture);

	FTextureRHIRef TextureRHI; // to keep the texture alive
};


class FOpenGLVertexBufferUnorderedAccessView : public FOpenGLUnorderedAccessView
{
public:

	FOpenGLVertexBufferUnorderedAccessView();

	FOpenGLVertexBufferUnorderedAccessView(	FOpenGLDynamicRHI* InOpenGLRHI, FVertexBufferRHIParamRef InVertexBuffer, uint8 Format);

	virtual ~FOpenGLVertexBufferUnorderedAccessView();

	FVertexBufferRHIRef VertexBufferRHI; // to keep the vertex buffer alive

	FOpenGLDynamicRHI* OpenGLRHI;
};

class FOpenGLShaderResourceView : public FRHIShaderResourceView
{
	// In OpenGL 3.2, the only view that actually works is a Buffer<type> kind of view from D3D10,
	// and it's mapped to OpenGL's buffer texture.

public:

	/** OpenGL texture the buffer is bound with */
	GLuint Resource;
	GLenum Target;

	int32 LimitMip;
	
	/** Needed on OS X to force a rebind of the texture buffer to the texture name to workaround radr://18379338 */
	FVertexBufferRHIRef VertexBuffer;
	uint64 ModificationVersion;
	uint8 Format;

	FOpenGLShaderResourceView( FOpenGLDynamicRHI* InOpenGLRHI, GLuint InResource, GLenum InTarget )
	:	Resource(InResource)
	,	Target(InTarget)
	,	LimitMip(-1)
	,	ModificationVersion(0)
	,	Format(0)
	,	OpenGLRHI(InOpenGLRHI)
	,	OwnsResource(true)
	{}
	
	FOpenGLShaderResourceView( FOpenGLDynamicRHI* InOpenGLRHI, GLuint InResource, GLenum InTarget, FVertexBufferRHIParamRef InVertexBuffer, uint8 InFormat )
	:	Resource(InResource)
	,	Target(InTarget)
	,	LimitMip(-1)
	,	VertexBuffer(InVertexBuffer)
	,	ModificationVersion(0)
	,	Format(InFormat)
	,	OpenGLRHI(InOpenGLRHI)
	,	OwnsResource(true)
	{
		FOpenGLVertexBuffer* VB = (FOpenGLVertexBuffer*)VertexBuffer.get();
		//ModificationVersion = VB->ModificationCount;
	}

	FOpenGLShaderResourceView( FOpenGLDynamicRHI* InOpenGLRHI, GLuint InResource, GLenum InTarget, GLuint Mip, bool InOwnsResource )
	:	Resource(InResource)
	,	Target(InTarget)
	,	LimitMip(Mip)
	,	ModificationVersion(0)
	,	Format(0)
	,	OpenGLRHI(InOpenGLRHI)
	,	OwnsResource(InOwnsResource)
	{}

	virtual ~FOpenGLShaderResourceView( void );

protected:
	FOpenGLDynamicRHI* OpenGLRHI;
	bool OwnsResource;
};

class FOpenGLRenderTarget :public FRHIRenderTarget {
public:
	GLuint Resource=0;
	bool bValid=false;

	void BindTexture(); 
};

//void  OpenGLTextureDeleted(FRHITexture* Texture);
//void  OpenGLTextureAllocated( FRHITexture* Texture , uint32 Flags);
//
//extern void ReleaseOpenGLFramebuffers(FOpenGLDynamicRHI* Device, FTextureRHIParamRef TextureRHI);
//

//
//class FOpenGLViewport : public FRHIViewport
//{
//public:
//
//	FOpenGLViewport(class FOpenGLDynamicRHI* InOpenGLRHI,void* InWindowHandle,uint32 InSizeX,uint32 InSizeY,bool bInIsFullscreen,EPixelFormat PreferredPixelFormat);
//	~FOpenGLViewport();
//
//	void Resize(uint32 InSizeX,uint32 InSizeY,bool bInIsFullscreen);
//
//	// Accessors.
//	FIntPoint GetSizeXY() const { return FIntPoint(SizeX, SizeY); }
//	FOpenGLTexture2D *GetBackBuffer() const { return BackBuffer.get(); }
//	bool IsFullscreen( void ) const { return bIsFullscreen; }
//
//	void WaitForFrameEventCompletion()
//	{
//		FrameSyncEvent.WaitForCompletion();
//	}
//
//	void IssueFrameEvent()
//	{
//		FrameSyncEvent.IssueEvent();
//	}
//
//	virtual void* GetNativeWindow(void** AddParam) const override;
//
//	struct FPlatformOpenGLContext* GetGLContext() const { return OpenGLContext; }
//	FOpenGLDynamicRHI* GetOpenGLRHI() const { return OpenGLRHI; }
//
//	virtual void SetCustomPresent(FRHICustomPresent* InCustomPresent) override
//	{
//		CustomPresent = make_shared<FRHICustomPresent>(InCustomPresent);
//	}
//	FRHICustomPresent* GetCustomPresent() const { return CustomPresent.get(); }
//private:
//
//	friend class FOpenGLDynamicRHI;
//
//	FOpenGLDynamicRHI* OpenGLRHI;
//	struct FPlatformOpenGLContext* OpenGLContext;
//	uint32 SizeX;
//	uint32 SizeY;
//	bool bIsFullscreen;
//	EPixelFormat PixelFormat;
//	bool bIsValid;
//	shared_ptr<FOpenGLTexture2D> BackBuffer;
//	FOpenGLEventQuery FrameSyncEvent;
//	FCustomPresentRHIRef CustomPresent;
//};

template<class T>
struct TOpenGLResourceTraits
{
};
template<>
struct TOpenGLResourceTraits<FRHIVertexDeclaration>
{
	typedef FOpenGLVertexDeclaration TConcreteType;
};
//template<>
//struct TOpenGLResourceTraits<FRHIVertexShader>
//{
//	typedef FOpenGLVertexShader TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHIGeometryShader>
//{
//	typedef FOpenGLGeometryShader TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHIHullShader>
//{
//	typedef FOpenGLHullShader TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHIDomainShader>
//{
//	typedef FOpenGLDomainShader TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHIPixelShader>
//{
//	typedef FOpenGLPixelShader TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHIComputeShader>
//{
//	typedef FOpenGLComputeShader TConcreteType;
//};
template<>
struct TOpenGLResourceTraits<FRHIBoundShaderState>
{
	typedef FOpenGLBoundShaderState TConcreteType;
};

template<>
struct TOpenGLResourceTraits<FRHIBoundBufferState>
{
	typedef FOpenGLBoundBufferState TConcreteType;
};
//template<>
//struct TOpenGLResourceTraits<FRHITexture3D>
//{
//	typedef FOpenGLTexture3D TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHITexture>
//{
//	typedef FOpenGLTexture TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHITexture2D>
//{
//	typedef FOpenGLTexture2D TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHITexture2DArray>
//{
//	typedef FOpenGLTexture2DArray TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHITextureCube>
//{
//	typedef FOpenGLTextureCube TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHIRenderQuery>
//{
//	typedef FOpenGLRenderQuery TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHIUniformBuffer>
//{
//	typedef FOpenGLUniformBuffer TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHIIndexBuffer>
//{
//	typedef FOpenGLIndexBuffer TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHIStructuredBuffer>
//{
//	typedef FOpenGLStructuredBuffer TConcreteType;
//};
template<>
struct TOpenGLResourceTraits<FRHIVertexBuffer>
{
	typedef FOpenGLVertexBuffer TConcreteType;
};
//template<>
//struct TOpenGLResourceTraits<FRHIShaderResourceView>
//{
//	typedef FOpenGLShaderResourceView TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHIUnorderedAccessView>
//{
//	typedef FOpenGLUnorderedAccessView TConcreteType;
//};
////
//template<>
//struct TOpenGLResourceTraits<FRHIViewport>
//{
//	typedef FOpenGLViewport TConcreteType;
//};
