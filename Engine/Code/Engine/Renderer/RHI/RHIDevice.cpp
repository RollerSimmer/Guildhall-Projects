#include "Engine/Renderer/RHI/RHIDevice.hpp"
#include <vector>
#include "Engine/File/FileUtils.hpp"
#include <d3dcommon.h>
#include "Engine/Renderer/RHI/ShaderProgram.hpp"
#include <d3dcompiler.h>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RHI/dx11.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/RHI/VertexBuffer.hpp"
#include "Engine/Renderer/Vertex3.hpp"


RHIDevice::RHIDevice()
	{

	}

RHIDevice::~RHIDevice()
	{

	}

RHIDeviceContext* RHIDevice::getImmediateContext()
	{
	RHIDeviceContext* result=nullptr;
	//TODO: RHIDevice::getImmediateContext

	return result;
	}

RHIOutput* RHIDevice::createOutput(Window *window,RHIOutputMode const mode)
	{
	RHIOutput* result=nullptr;
	//TODO: RHIDevice::createOutput 
	return result;
	}

RHIOutput* RHIDevice::createOutput(unsigned int const px_width,unsigned int const px_height,RHIOutputMode const mode)
	{
	RHIOutput* result=nullptr;
	//TODO: RHIDevice::createOutput 
	return result;
	}

ShaderProgram* RHIDevice::createShaderFromFile(const char* fileName)
	{
	size_t src_size = 0U;
	std::vector<unsigned char> srcBuffer;
	if (!readBufferFromFile(srcBuffer,std::string(fileName)))
		{
		return nullptr;
		}
	srcBuffer.push_back(0);


	ID3DBlob* vs_byte_code = nullptr;
	ID3DBlob* fs_byte_code = nullptr;

	vs_byte_code = CompileHLSLToShaderBlob(fileName,srcBuffer,"VertexFunction","vs_5_0");
	fs_byte_code = CompileHLSLToShaderBlob(fileName,srcBuffer,"FragmentFunction","ps_5_0");

	if ((vs_byte_code != nullptr)
		&& (fs_byte_code != nullptr))
		{

		ID3D11VertexShader *vs = nullptr;
		ID3D11PixelShader *fs = nullptr;

		void* vsBuffer=vs_byte_code->GetBufferPointer();
		size_t vsSize=vs_byte_code->GetBufferSize();

		void* fsBuffer=fs_byte_code->GetBufferPointer();
		size_t fsSize=fs_byte_code->GetBufferSize();

		m_dxDevice->CreateVertexShader(vsBuffer,vsSize,nullptr,&vs);
		
		m_dxDevice->CreatePixelShader(fsBuffer,fsSize,nullptr,&fs);

		ShaderProgram *program = new ShaderProgram(this,vs,fs,vs_byte_code,fs_byte_code);
		return program;
		}

	return nullptr;
	}

ID3DBlob* RHIDevice::CompileHLSLToShaderBlob(const char* filename,const std::vector<unsigned char>& srcBuffer,
											 LPCSTR entryPoint,LPCSTR target)
	{
	/* DEFINE MACROS - CONTROLS SHADER
	// You can optionally define macros to control compilation (for instance, DEBUG builds, disabling lighting, etc...)
	D3D_SHADER_MACRO defines[1];
	defines[0].Name = "TEST_MACRO";
	defines[0].Definition = nullptr;
	*/

	DWORD compile_flags = 0U;
	#if defined(DEBUG_SHADERS)
	compile_flags |= D3DCOMPILE_DEBUG;
	compile_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	compile_flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX YOUR WARNINGS
	#else 
	// compile_flags |= D3DCOMPILE_SKIP_VALIDATION;       // Only do this if you know for a fact this shader works with this device (so second run through of a game)
	compile_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;   // Yay, fastness (default is level 1)
	#endif

	ID3DBlob *code = nullptr;
	ID3DBlob *errors = nullptr;

	const unsigned char* srcChars;
	srcChars=srcBuffer.data();

	HRESULT hr = ::D3DCompile( srcChars, 
							  srcBuffer.size(),                   // plain text source code
							  filename,                       // optional, used for error messages (If you HLSL has includes - it will not use the includes names, it will use this name)
							  nullptr,                            // pre-compiler defines - used more for compiling multiple versions of a single shader (different quality specs, or shaders that are mostly the same outside some constants)
							  D3D_COMPILE_STANDARD_FILE_INCLUDE,  // include rules - this allows #includes in the shader to work relative to the src_file path or my current working directly
							  entryPoint,                         // Entry Point for this shader
							  target,                             // Compile Target (MSDN - "Specifying Compiler Targets")
							  compile_flags,                      // Flags that control compilation
							  0,                                  // Effect Flags (we will not be doing Effect Files)
							  &code,                              // [OUT] ID3DBlob (buffer) that will store the byte code.
							  &errors );                          // [OUT] ID3DBlob (buffer) that will store error information

	if (FAILED(hr) || (errors != nullptr)) {
		if (errors != nullptr) {
			char *error_string = (char*) errors->GetBufferPointer();
			DebuggerPrintf( "Failed to compile [%s].  Compiler gave the following output;\n%s", 
				 filename, 
				 error_string );
			DX_SAFE_RELEASE(errors);
			}
		} 

	// will be nullptr if it failed to compile
	return code;
	}

VertexBuffer* RHIDevice::createVertexBuffer(Vertex3F *vertices,unsigned int vertex_count)
	{
	VertexBuffer *buffer = new VertexBuffer(this,vertices,vertex_count);
	return buffer;
	}
