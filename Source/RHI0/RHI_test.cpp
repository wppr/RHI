//
//#include "../OpenglDriver/v1/OpenGLRHI.h"
//#include "PlatformHelper.h"
//#include "Image.h"
//int main() {
//	PlatformHelper ph;
//	ph.CreateWindowAndDevice(600, 600);
//	FOpenGLDynamicRHI* RHI = new FOpenGLDynamicRHI;
//	RHI->Init();
//
//	float points[] = {
//		0.0f,	0.5f,	0.0f,
//		0.5f, -0.5f,	0.0f,
//		-0.5f, -0.5f,	0.0f
//	};
//
//	const char* vertex_shader =
//		"#version 300 es\n"
//		"in vec3 vp;"
//		"void main () {"
//		"	gl_Position = vec4 (vp, 1.0);"
//		"}";
//
//	const char* fragment_shader =
//		"#version 300 es\n"
//		"out vec4 frag_colour;"
//		"void main () {"
//		"	frag_colour = vec4 (0.5, 0.0, 0.5, 1.0);"
//		"}";
//	auto vs=RHI->RHICreateVertexShader(vertex_shader);
//	auto fs = RHI->RHICreatePixelShader(fragment_shader);
//
//	auto program = RHI->RHICreateBoundShaderState(NULL, vs.get(), fs.get());
//	
//
//	FDepthStencilStateInitializerRHI Depth_state_desc;
//	auto depth_state=RHI->RHICreateDepthStencilState(Depth_state_desc);
//	RHI->RHISetDepthStencilState(depth_state.get(),0);
//
//	auto vertex_buffer=RHI->RHICreateVertexBuffer(9 * sizeof(float), BUF_Static, FRHIResourceCreateInfo(points));
//
//	FVertexDeclarationElementList VertexDesc;
//	VertexDesc.push_back(FVertexElement(0,VET_Float3));
//	auto VertexDecl = RHI->RHICreateVertexDeclaration(VertexDesc);
//
//
//	FBoundBufferDesc BufferDesc;
//	BufferDesc.VertexBuffers[0] = vertex_buffer.get();
//	auto buffer_state=RHI->RHICreateBoundBufferState(BufferDesc, VertexDecl.get());
//
//	FIntRect r;
//	
//	//texture test
//		Image im1("default_asserts/3.jpg");
//		Image im2("default_asserts/2.tga");
//		Image im3("default_asserts/4.jpg");
//		Image im4("default_asserts/2.png");
//		vector<void*> ImageData;
//		ImageData.push_back(im1.pdata);
//		//GLuint tex;
//		//glGenTextures(1, &tex);
//		//glBindTexture(GL_TEXTURE_2D, tex);
//		//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);
//		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 512, 512, GL_RGB, GL_UNSIGNED_BYTE, ImageData[0]);
//		//assert(glGetError() == GL_NO_ERROR);
//		vector<string> e1 = { "default_asserts/ennis_c00.hdr", "default_asserts/ennis_c01.hdr", "default_asserts/ennis_c02.hdr",
//			"default_asserts/ennis_c03.hdr", "default_asserts/ennis_c04.hdr", "default_asserts/ennis_c05.hdr" };
//		auto ims=ImageManager::getInstance().CreateImageListFromFiles(e1);
//		auto tex2 = RHI->RHICreateTexture2D(im1.width, im1.height, EPixelFormat::PF_R8G8B8, 0, 1, TexCreate_None, FRHIResourceCreateInfo(ImageData));
//		vector<void*> ImageData2;
//		for (auto& x : ims)
//			ImageData2.push_back(x->pdata);
//		auto tex3 = RHI->RHICreateTextureCube(ims[0]->width, PF_FloatRGB, 1, TexCreate_None, FRHIResourceCreateInfo(ImageData2));
//		RHI->RHISetShaderTexture(vs.get(), 0, tex2.get());
//		
//		//RenderTarget Test
//		int s = 512;
//		auto color1= RHI->RHICreateTexture2D(s, s, EPixelFormat::PF_R8G8B8A8, 1, 1, TexCreate_None, FRHIResourceCreateInfo());
//		auto color2 = RHI->RHICreateTexture2D(s, s, EPixelFormat::PF_R8G8B8A8, 1, 1, TexCreate_None, FRHIResourceCreateInfo());
//		auto depth = RHI->RHICreateTexture2D(s, s, EPixelFormat::PF_D24, 1, 1, TexCreate_None, FRHIResourceCreateInfo());
//
//		FRHIRenderTargetView v1(color1.get());
//		FRHIRenderTargetView v2(color2.get());
//		FRHIDepthRenderTargetView d(depth.get());
//
//		FRHISetRenderTargetsInfo info;
//		info.NumColorRenderTargets = 2;
//		info.ColorRenderTarget[0] = v1;
//		info.ColorRenderTarget[1] = v2;
//		info.DepthStencilRenderTarget = d;
//		auto RenderTarget=RHI->RHICreateRenderTarget(info);
//
//
//		// 
//		struct UniformBuffer {
//			float a[4];
//			uint16 b[3];
//			bool c;
//			//float View[16];
//			//float World[16];
//		} u1;
//		cout << sizeof(u1) << endl;
//		vector<uint8> layout{ 
//			UBT_FLOAT4,
//			UBT_MAT4,
//			UBT_MAT4 };
//		FRHIUniformBufferLayout RHILayout;
//		RHILayout.Resources = layout;
//
//		RHI->RHICreateUniformBuffer(&u1, RHILayout, UniformBuffer_SingleDraw);
//		//RHI->RHISetRenderTarget(FOpenGLRenderTarget());
//		//RHI->RHISetBoundShaderState(program.get());
//		while (true) {
//			MSG msg;
//			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//			{
//				if (msg.message == WM_QUIT)
//					break;
//				else
//				{
//					TranslateMessage(&msg);
//					DispatchMessage(&msg);
//				}
//			}
//			else {
//				RHI->RHIClear(true, FLinearColor::Black, true, 1.0, false, 0, r);
//				RHI->RHISetBoundShaderState(program.get());
//				RHI->RHISetBoundBufferState(buffer_state.get());
//				RHI->RHIDrawPrimitive(PT_TriangleList, 0, 3, 1);
//				ph.SwapBuffer();
//			}
//
//	}
//	RHI->Shutdown();
//	return 0;
//}