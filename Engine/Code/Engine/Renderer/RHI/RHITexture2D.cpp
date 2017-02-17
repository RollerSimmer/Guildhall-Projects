#include "Engine/Renderer/RHI/RHITexture2D.hpp"
#include "Engine/Renderer/RHI/RHIDevice.hpp"

RHITexture2D::RHITexture2D(RHIDevice *owner)
	: m_device(owner)
	, m_dxResource(nullptr)
	, m_dxRtv(nullptr)
	, m_dxSrv(nullptr)
	, m_width(0)
	, m_height(0)
	, m_dxBindFlags(0U)
	{}

RHITexture2D::RHITexture2D(RHIDevice *device,ID3D11Texture2D *tex)
	:	m_device(device)
	,	m_dxResource(tex)
	,	m_dxRtv(nullptr)
	,	m_width(0)
	,	m_height(0)
	{
	createRenderTargetView();
	}

RHITexture2D::RHITexture2D(RHIDevice *owner, char const *filename)
	: RHITexture2D(owner)
	{
	RHIImage image;
	if (!image.loadFromFile(filename)) {
		return;
		}

	loadFromImage(image);
	}

RHITexture2D::~RHITexture2D()
	{

	}

unsigned int RHITexture2D::getWidth()
	{
	D3D11_TEXTURE2D_DESC desc;
	m_dxResource->GetDesc(&desc);
	return desc.Width;
	}

unsigned int RHITexture2D::getHeight()
	{
	D3D11_TEXTURE2D_DESC desc;
	m_dxResource->GetDesc(&desc);
	return desc.Height;
	}

void RHITexture2D::createViews()
	{
	ID3D11Device *dd = m_device->m_dxDevice;

	if (m_dxBindFlags & D3D11_BIND_RENDER_TARGET) {
		dd->CreateRenderTargetView(m_dxResource, nullptr, &m_dxRtv);
		}

	if (m_dxBindFlags & D3D11_BIND_SHADER_RESOURCE) {
		dd->CreateShaderResourceView(m_dxResource, nullptr, &m_dxSrv);
		}
	}

bool RHITexture2D::loadFromImage(RHIImage const &img)
	{
	D3D11_USAGE usage = D3D11_USAGE_IMMUTABLE;

	D3D11_TEXTURE2D_DESC tex_desc;
	memset(&tex_desc, 0, sizeof(tex_desc));

	tex_desc.Width = img.getWidth();
	tex_desc.Height = img.getHeight();
	tex_desc.MipLevels = 1;                // no mip mapping
	tex_desc.ArraySize = 1;                // NO ARRAYS!  
	tex_desc.Usage = usage;
	tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags = 0U;
	tex_desc.MiscFlags = 0U; // there is one for generating mip maps.

							 // multisampling options
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;

	// Helps us figure out what views to make
	m_dxBindFlags = D3D11_BIND_SHADER_RESOURCE;

	// Setup initial data
	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(data));
	data.pSysMem = img.m_buffer;
	data.SysMemPitch = img.m_bpp * tex_desc.Width;

	ID3D11Device *dx_device = m_device->m_dxDevice;
	HRESULT hr = dx_device->CreateTexture2D(&tex_desc, &data, &m_dxResource);

	if (SUCCEEDED(hr)) {
		createViews();
		return true;
		}
	else {
		return false;
		}
	}

void RHITexture2D::createRenderTargetView()
	{
	m_device->m_dxDevice->CreateRenderTargetView(m_dxResource,nullptr,&m_dxRtv);
	}
