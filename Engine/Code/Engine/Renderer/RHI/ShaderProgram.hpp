#pragma once
#include <d3dcommon.h>

class RHIDevice;
class ID3D11VertexShader;
class ID3D11PixelShader;
class ID3D11InputLayout;

class ShaderProgram
	{
	public:

		ShaderProgram(RHIDevice* device,ID3D11VertexShader* va,ID3D11PixelShader* fs,
					  ID3DBlob* vaBytecode,ID3DBlob* faBytecode);
		~ShaderProgram();

		void createInputLayout(RHIDevice* device);

		bool isValid() const;


		ID3D11VertexShader* m_dxVertexShader;
		ID3D11PixelShader* m_dxFragmentShader;

		// Input Layout - for now, we're going 
		// to assume every shader makes their own
		// for simplicity, but honestly you should make these
		// based on need
		ID3D11InputLayout* m_dxInputLayout;

		// [OPTIONAL] ByteCode - only need to keep it around if using Reflection
		ID3DBlob* m_vsByteCode;
		ID3DBlob* m_fsByteCode;

	};