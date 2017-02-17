#include "Engine/Renderer/RHI/Sampler.hpp"
#include "Engine/Renderer/RHI/dx11.hpp"

Sampler::Sampler(RHIDevice *owner)
	: m_device(owner)
	, m_dxSampler(nullptr)
	{}

Sampler::Sampler(RHIDevice *owner, FilterMode minFilter, FilterMode magFilter)
	: Sampler(owner)
	{
	createSampler(minFilter,magFilter);
	}

Sampler::~Sampler()
	{
	DX_SAFE_RELEASE(m_dxSampler);
	}

//------------------------------------------------------------------------
static D3D11_FILTER DXGetFilterMode(FilterMode min, FilterMode mag)
	{
	if (min == FILTER_POINT) {
		if (mag == FILTER_POINT) {
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
			}
		else { // LINEAER
			return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
			}
		}
	else { // assume linear
		if (mag == FILTER_POINT) {
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
			}
		else { // assume linear
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			}
		}

	// UNSUPPORTED - Assert?
	// ASSERT_RETURN_VALUE( 0, D3D11_FILTER_MIN_MAG_MIP_POINT );
	// return D3D11_FILTER_MIN_MAG_MIP_POINT;
	}


bool Sampler::createSampler(FilterMode minFilter, FilterMode magFilter)
	{
	D3D11_SAMPLER_DESC desc;
	memset(&desc,0,sizeof(desc));

	desc.Filter = DXGetFilterMode(minFilter, magFilter);
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	// desc.BorderColor = BLACK TRANSPARENT6f 0.
	desc.MinLOD = 0;
	desc.MaxLOD = 0;

	ID3D11Device *m_dxDevice = m_device->m_dxDevice;
	HRESULT hr = m_dxDevice->CreateSamplerState(&desc, &m_dxSampler);

	return SUCCEEDED(hr);
	}
