#include "Engine/Renderer/RHI/ConstantBuffer.hpp"
#include "Engine/Renderer/RHI/RHIDevice.hpp"
#include "Engine/Renderer/RHI/RHIDeviceContext.hpp"

#include "dx11.hpp"
#include <d3d11.h>

ConstantBuffer::ConstantBuffer(RHIDevice *device,
	void const *buffer,
	size_t const size)
	{
	m_bufferSize = size;

	// First, describe the buffer
	D3D11_BUFFER_DESC vbDesc;
	memset(&vbDesc, 0, sizeof(vbDesc));

	vbDesc.ByteWidth = (unsigned int)m_bufferSize;                      // How much data are we putting into this buffer
	vbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;       // What can we bind this data as (in this case, only vertex data)
	vbDesc.Usage = D3D11_USAGE_DYNAMIC;                  // Hint on how this memory is used (in this case, it is immutable, or constant - can't be changed)
														  // for limitations/strenghts of each, see;  
														  //    https://msdn.microsoft.com/en-us/library/windows/desktop/ff476259(v=vs.85).aspx
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbDesc.StructureByteStride = 0;                      // How large is each element in this buffer

														  // Next, setup the initial data (required since this is an immutable buffer - so it must be instantiated at creation time)
	D3D11_SUBRESOURCE_DATA initial_data;
	memset(&initial_data, 0, sizeof(initial_data));
	initial_data.pSysMem = buffer;

	// Finally create the vertex buffer
	m_dxBuffer = nullptr;
	device->m_dxDevice->CreateBuffer(&vbDesc, &initial_data, &m_dxBuffer);
	}

//------------------------------------------------------------------------
ConstantBuffer::~ConstantBuffer()
	{
	DX_SAFE_RELEASE(m_dxBuffer);
	}

//------------------------------------------------------------------------
bool ConstantBuffer::update(RHIDeviceContext *context, void const *buffer)
	{
	D3D11_MAPPED_SUBRESOURCE resource;

	ID3D11DeviceContext *m_dxContext = context->m_dxContext;
	if (SUCCEEDED(m_dxContext->Map(m_dxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource))) {
		memcpy(resource.pData, buffer, m_bufferSize);
		m_dxContext->Unmap(m_dxBuffer, 0);

		return true;
		}

	// ASSERT?
	return false;
	}
