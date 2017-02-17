/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "VertexBuffer.hpp"
#include "Engine/Core/Types.hpp"
#include "Engine/Renderer/RHI/RHIDeviceContext.hpp"
#include "Engine/Renderer/RHI/RHIDevice.hpp"


/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* MACROS                                                               */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* TYPES                                                                */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* LOCAL VARIABLES                                                      */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* LOCAL FUNCTIONS                                                      */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* EXTERNAL FUNCTIONS                                                   */
/*                                                                      */
/************************************************************************/
//------------------------------------------------------------------------
VertexBuffer::VertexBuffer(RHIDevice *device,
	Vertex3F const *vertices,
	uint const vertex_count,
	BufferUsage desired_usage /*= BUFFERUSAGE_STATIC*/)
	{

	m_usage = desired_usage;
	m_dxBuffer = nullptr;
	m_bufferSize = 0U;

	create(device, vertices, vertex_count);


	// Finally create the vertex buffer
	}

//------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
	{
	DX_SAFE_RELEASE(m_dxBuffer);
	}

//------------------------------------------------------------------------
bool VertexBuffer::create(RHIDevice *device, Vertex3F const *data, uint const count)
	{
	// make sure I free the old one
	DX_SAFE_RELEASE(m_dxBuffer);

	m_bufferSize = count * sizeof(Vertex3F);

	D3D11_BUFFER_DESC vb_desc;
	memZero(&vb_desc);

	vb_desc.ByteWidth = m_bufferSize;  // How much data are we putting into this buffer
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;         // What can we bind this data as (in this case, only vertex data)
	vb_desc.StructureByteStride = sizeof(Vertex3F);       // How large is each element in this buffer

	if (m_usage == BUFFERUSAGE_DYNAMIC) {
		vb_desc.Usage = D3D11_USAGE_DYNAMIC;
		vb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
	else {
		vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
		}

	// Next, setup the initial data (required since this is an immutable buffer - so it must be instantiated at creation time)
	m_dxBuffer = nullptr;
	if (nullptr != data) {
		D3D11_SUBRESOURCE_DATA initial_data;
		memset(&initial_data, 0, sizeof(initial_data));
		initial_data.pSysMem = data;

		device->m_dxDevice->CreateBuffer(&vb_desc, &initial_data, &m_dxBuffer);
		}
	else {
		device->m_dxDevice->CreateBuffer(&vb_desc, nullptr, &m_dxBuffer);
		}

	if (m_dxBuffer == nullptr) {
		m_bufferSize = 0U;
		return false;
		}
	else {
		return true;
		}
	}

//------------------------------------------------------------------------
void VertexBuffer::update(RHIDeviceContext *context, Vertex3F const* data, uint const count)
	{
	size_t size = sizeof(Vertex3F) * count;
	if (size > m_bufferSize) {
		// too large - can't map it.  Recreate a larger one [note, will require a rebind
		// to the context before it takes affect]
		create(context->m_device, data, count);
		}
	else {
 // Smaller than the block of memory allocated - so just copy into it.
		D3D11_MAPPED_SUBRESOURCE resource;
		ID3D11DeviceContext *m_dxContext = context->m_dxContext;
		if (SUCCEEDED(m_dxContext->Map(m_dxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource))) {
			memcpy(resource.pData, data, size);
			m_dxContext->Unmap(m_dxBuffer, 0);
			}
		}
	}


/************************************************************************/
/*                                                                      */
/* COMMANDS                                                             */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* UNIT TESTS                                                           */
/*                                                                      */
/************************************************************************/
