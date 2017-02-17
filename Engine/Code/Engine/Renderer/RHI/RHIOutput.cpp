#include "Engine/Renderer/RHI/RHIOutput.hpp"
#include "Engine/Gui/Window.hpp"
#include <d3d11.h>
#include "Engine/Renderer/RHI/RHITexture2D.hpp"
//TODO: implement RHIOutput methods 

RHIOutput::RHIOutput(RHIDevice* owner,Window* wnd, /*[OPTIONAL]*/ RHIOutputMode const mode /*= RENDEROUTPUT_WINDOWED*/)
	:	m_device(owner)
	,	m_window(wnd)
	,	m_swapChain(nullptr)
	,	m_renderTarget(nullptr)
	{

	}

RHIOutput::~RHIOutput()
	{

	}

void RHIOutput::present()
	{
	m_swapChain->Present(1,0);
	}

void RHIOutput::close()
	{

	}

RHITexture2D* RHIOutput::getRenderTarget()
	{
	return m_renderTarget;
	}

float RHIOutput::getWidth() const
	{
	return m_window->getWidth();
	}

float RHIOutput::getHeight() const
	{
	return m_window->getHeight();
	}

float RHIOutput::getAspectRatio() const
	{
	return getWidth()*getHeight();
	}
bool RHIOutput::setDisplayMode(RHIOutputMode const mode,float const width /*= 0.0f*/,float const height /*= 0.0f*/)
	{
	return false;
	}
 
void RHIOutput::createRenderTarget()
	{
	ID3D11Texture2D* dxResource; 
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&dxResource);
	m_renderTarget=new RHITexture2D(m_device,dxResource);
	}
