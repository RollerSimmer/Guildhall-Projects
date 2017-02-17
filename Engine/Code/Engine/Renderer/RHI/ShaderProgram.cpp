#include "ShaderProgram.hpp"
#include "Engine/Renderer/RHI/ShaderProgram.hpp"
#include "Engine/Renderer/RHI/dx11.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/RHI/RHIDevice.hpp"
#include "Engine/Renderer/RHI/RHIDeviceContext.hpp"

ShaderProgram::ShaderProgram(RHIDevice* device,ID3D11VertexShader* va,ID3D11PixelShader* fs,ID3DBlob* vaBytecode,ID3DBlob* faBytecode)
	: m_dxVertexShader(va)
	,m_dxFragmentShader(fs)
	,m_vsByteCode(vaBytecode)
	,m_fsByteCode(faBytecode)
	{
	m_dxInputLayout=nullptr;
	createInputLayout(device);
	}

ShaderProgram::~ShaderProgram()
	{
	DX_SAFE_RELEASE(m_dxVertexShader);
	DX_SAFE_RELEASE(m_dxFragmentShader);
	DX_SAFE_RELEASE(m_dxInputLayout);
	DX_SAFE_RELEASE(m_vsByteCode);
	DX_SAFE_RELEASE(m_fsByteCode);
	}

void ShaderProgram::createInputLayout(RHIDevice* device)
	{

// 	typedef struct D3D11_INPUT_ELEMENT_DESC {
// 		LPCSTR                     SemanticName;
// 		UINT                       SemanticIndex;
// 		DXGI_FORMAT                Format;
// 		UINT                       InputSlot;
// 		UINT                       AlignedByteOffset;
// 		D3D11_INPUT_CLASSIFICATION InputSlotClass;
// 		UINT                       InstanceDataStepRate;
// 		} D3D11_INPUT_ELEMENT_DESC;
	D3D11_INPUT_ELEMENT_DESC desc[3];
	memset(desc,0,sizeof(desc));

	// 
	desc[0].SemanticName = "POSITION";
	desc[0].SemanticIndex = 0;
	desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc[0].InputSlot = 0U;
	desc[0].AlignedByteOffset = offsetof(Vertex3F,m_pos);
	desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[0].InstanceDataStepRate = 0U;

	// UV
	desc[1].SemanticName = "UV";
	desc[1].SemanticIndex = 0;
	desc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	desc[1].InputSlot = 0U;
	desc[1].AlignedByteOffset = offsetof(Vertex3F,m_texcoords);
	desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[1].InstanceDataStepRate = 0U;

	// TINT
	desc[2].SemanticName = "TINT";
	desc[2].SemanticIndex = 0;
	desc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc[2].InputSlot = 0U;
	desc[2].AlignedByteOffset = offsetof(Vertex3F,m_color);
	desc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[2].InstanceDataStepRate = 0U;

	HRESULT hres=
	device->m_dxDevice->CreateInputLayout(desc,
										 ARRAYSIZE(desc),
										 m_vsByteCode->GetBufferPointer(),
										 m_vsByteCode->GetBufferSize(),
										 &m_dxInputLayout);

	for(;;)
		break;

	}

bool ShaderProgram::isValid() const
	{
	return (m_dxVertexShader != nullptr) && (m_dxFragmentShader != nullptr);
	}

