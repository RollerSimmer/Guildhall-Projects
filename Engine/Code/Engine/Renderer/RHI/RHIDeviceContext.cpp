#include "Engine/Renderer/RHI/RHIDeviceContext.hpp"
#include "Engine/Renderer/RHI/RHITexture2D.hpp"
#include "Engine/Renderer/RgbaFloat.hpp"
#include "Engine/Renderer/RHI/Sampler.hpp"
#include "Engine/Renderer/RHI/ShaderProgram.hpp"
#include "Engine/Renderer/RHI/RasterState.hpp"
#include "Engine/Renderer/BlendState.hpp"

RHIDevice *m_device;

RHIDeviceContext::RHIDeviceContext(RHIDevice *owner,ID3D11DeviceContext *ctx)
	:	m_device(owner)
	,	m_dxContext(ctx)
	{

	}

RHIDeviceContext::~RHIDeviceContext()
	{

	}

void RHIDeviceContext::clearState()
	{

	}

void RHIDeviceContext::flush()
	{

	}

void RHIDeviceContext::setShader(ShaderProgram* program)
	{
	if(program->isValid())
		{
		m_dxContext->VSSetShader(program->m_dxVertexShader,nullptr,0U);
		m_dxContext->PSSetShader(program->m_dxFragmentShader,nullptr,0U);
		m_dxContext->IASetInputLayout(program->m_dxInputLayout);
		}
	}

void RHIDeviceContext::clearColorTarget(RHITexture2D* output,const RgbaFloat &color)
	{
	ID3D11RenderTargetView *rtv = output->m_dxRtv;
	
	m_dxContext->ClearRenderTargetView(rtv,color.asArray());
	}

void RHIDeviceContext::setRasterState(RasterState* rs)
	{
	m_dxContext->RSSetState(rs->dx_state);
	}

void RHIDeviceContext::draw(uint vcount, uint offset)
	{

	m_dxContext->Draw(vcount,offset);
	}

void RHIDeviceContext::setTexture(unsigned int textureIndex, RHITexture2D * texture)
	{
	m_dxContext->VSSetShaderResources(textureIndex, 1, &texture->m_dxSrv);
	m_dxContext->PSSetShaderResources(textureIndex, 1, &texture->m_dxSrv);
	}

void RHIDeviceContext::setSampler(unsigned int samplerIndex, Sampler* SamplerDx)
	{
	m_dxContext->VSSetSamplers(samplerIndex,1,&SamplerDx->m_dxSampler);
	m_dxContext->PSSetSamplers(samplerIndex,1,&SamplerDx->m_dxSampler);
	}

void RHIDeviceContext::setBlendState(BlendState* bs)
	{
	float constant[]={1,1,1,1};
	m_dxContext->OMSetBlendState(bs->dx_state,
		constant,0xffffffff);
	}
