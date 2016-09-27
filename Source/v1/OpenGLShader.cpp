#include "OpenGLRHI.h"
#include <memory>
using namespace std;
#include "Logger.h"
using namespace HW;
#include "OpenGLShaderResources.h"




/**
* Compiles an OpenGL shader using the given GLSL microcode.
* @returns the compiled shader upon success.
*/
template <typename ShaderType>
ShaderType* CompileOpenGLShader(const char* Code)
{


	ShaderType* Shader = nullptr;
	const GLenum TypeEnum = ShaderType::TypeEnum;
	GLuint Resource = glCreateShader(TypeEnum);

	glShaderSource(Resource, 1, &Code, NULL);

	// Compile the shader
	glCompileShader(Resource);

	// Check for errors
	int result;
	glGetShaderiv(Resource, GL_COMPILE_STATUS, &result);
	Shader = new ShaderType();
	Shader->Resource = Resource;

#define DEBUG_SHADER
#ifdef DEBUG_SHADER
	if (GL_FALSE == result) {
		// Compile failed, get log
		
		int length = 0;
		string logString;
		glGetShaderiv(Resource, GL_INFO_LOG_LENGTH, &length);
		if (length > 0) {
			char * c_log = new char[length];
			int written = 0;
			glGetShaderInfoLog(Resource, length, &written, c_log);
			logString = c_log;
			delete[] c_log;
		}
		string msg = "Shader compilation failed.\n";
		msg += logString;
		Shader->GlslCode = Code;
		Logger::WriteErrorLog(msg.c_str());
		Logger::WriteErrorLog(Code);
	}
	else {
		Shader->bSuccessfullyCompiled = 1;
	}
#endif


	return Shader;
}
FPixelShaderRHIRef FOpenGLDynamicRHI::RHICreatePixelShader(const char* Code)
{
	
	auto s = CompileOpenGLShader<FOpenGLPixelShader>(Code);
	shared_ptr<FRHIPixelShader> ref(s);
	return ref;
}

FVertexShaderRHIRef FOpenGLDynamicRHI::RHICreateVertexShader(const char* Code)
{
	auto s = CompileOpenGLShader<FOpenGLVertexShader>(Code);
	shared_ptr<FRHIVertexShader> ref(s);
	return ref;
}


FOpenGLBoundShaderState::FOpenGLBoundShaderState(FVertexDeclarationRHIParamRef InVertexDeclarationRHI,
	FVertexShaderRHIParamRef InVertexShaderRHI,
	FPixelShaderRHIParamRef InPixelShaderRHI
	) {
	VertexShader = static_cast<FOpenGLVertexShader*>(InVertexShaderRHI);
	PixelShader = static_cast<FOpenGLPixelShader*>(InPixelShaderRHI);
	LinkProgram();
}
void FOpenGLBoundShaderState::LinkProgram() {
	
		ProgramID = glCreateProgram();
		glGetError();
		glAttachShader(ProgramID, VertexShader->Resource);
		glAttachShader(ProgramID, PixelShader->Resource);
		if (bLinked) return;
		if (ProgramID <= 0)
			assert("Program has not been compiled.");

		glLinkProgram(ProgramID);

		int status = 0;
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &status);
		if (GL_FALSE == status) {
			// Store log and return false
			int length = 0;
			string logString;

			glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &length);

			if (length > 0) {
				char * c_log = new char[length];
				int written = 0;
				glGetProgramInfoLog(ProgramID, length, &written, c_log);
				logString = c_log;
				delete[] c_log;
			}
			Logger::WriteErrorLog((string("Program link failed:\n") + logString).c_str());
		}
		else {
			bLinked = true;
		}
}

FBoundShaderStateRHIRef FOpenGLDynamicRHI::RHICreateBoundShaderState(FVertexDeclarationRHIParamRef VertexDeclaration, FVertexShaderRHIParamRef VertexShader, FPixelShaderRHIParamRef PixelShader)
{
	FOpenGLBoundShaderState* State = new FOpenGLBoundShaderState(VertexDeclaration, VertexShader, PixelShader);

	
	shared_ptr<FRHIBoundShaderState> ref(State);
	return ref;
}

void FOpenGLDynamicRHI::BindPendingShaderState(FOpenGLContextState& ContextState)
{


	bool ForceUniformBindingUpdate = false;

	GLuint PendingProgram = PendingState.BoundShaderState->ProgramID;
	if (ContextState.Program != PendingProgram)
	{
		glUseProgram(PendingProgram);
		ContextState.Program = PendingProgram;
	}
}