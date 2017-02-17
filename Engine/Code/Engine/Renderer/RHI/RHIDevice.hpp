#pragma once
#include <d3d11.h>
#include "RHIOutputMode.hpp"
#include <vector>
#include "Engine/Renderer/Vertex3.hpp"

class RHIDeviceContext;
class RHIOutput;
class Window;
class ShaderProgram;
class VertexBuffer;
struct Vertex3;

class RHIDevice
	{
	public:
		RHIDevice();
		~RHIDevice();

		RHIDeviceContext* getImmediateContext();

		// [OPTIONAL] Ability to create secondary outputs
		//            Neat for screensavers, tools, or utlizing a second monitor
		RHIOutput* createOutput(Window *window,RHIOutputMode const mode);
		RHIOutput* createOutput(unsigned int const px_width,
								unsigned int const px_height,
								RHIOutputMode const mode);
		 
		ShaderProgram* createShaderFromFile(const char* fileName);

		ID3DBlob* CompileHLSLToShaderBlob(const char* filename,const std::vector<unsigned char>& srcBuffer,
										  LPCSTR entryPoint,LPCSTR target);

		VertexBuffer* createVertexBuffer(Vertex3F *vertices, unsigned int vertex_count);
	public:
		// Immediate Mode Context - Created with the Device
		RHIDeviceContext *m_immediateContext;
		ID3D11Device *m_dxDevice;


	private:
	};