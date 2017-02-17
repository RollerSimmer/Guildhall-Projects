#include "Engine/Renderer/RHI/RHIInstance.hpp"
#include "Engine/Renderer/RHI/RHIOutput.hpp"
#include "Engine/Renderer/RHI/RHIDevice.hpp"
#include "Engine/Renderer/RHI/RHIDeviceContext.hpp"

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )


RHIInstance::~RHIInstance()
	{

	}

RHIInstance* RHIInstance::getInstance()
	{
	static RHIInstance* result=nullptr;
	if(result==nullptr)
		result=new RHIInstance();
	return result;
	}

RHIInstance::RHIInstance()
	{

	}

bool RHIInstance::createOutput(WindowMessageHandlerCallback winproc, 
							   RHIDeviceContext*& out_context,
							   RHIOutput*& out_output, 
							   const std::string& windowTitle,
							   int left,int top,int width,int height,
							   /*OPTIONAL*/ RHIOutputMode const initial_mode /*= RENDEROUTPUT_WINDOWED*/)
	{
	//TODO: finish RHIInstance::createOutput
	HINSTANCE appInst=GetModuleHandle(0);

	Window* window=new Window(appInst,winproc,windowTitle,left,top,width,height);
	RHIDevice* dev=new RHIDevice();
	out_output=new RHIOutput(dev,window);

	DXGI_SWAP_CHAIN_DESC swapDesc;

	memset(&swapDesc,0,sizeof(swapDesc));

	// fill the swap chain description struct
	swapDesc.BufferCount = 2;                                    // two buffers (one front, one back?)
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT       // how swap chain is to be used
		| DXGI_USAGE_BACK_BUFFER;
	swapDesc.OutputWindow = window->getHandle();                                // the window to be copied to on present
	swapDesc.SampleDesc.Count = 1;                               // how many multisamples (1 means no multi sampling)
																  // Default options.
	swapDesc.Windowed = TRUE;                                    // windowed/full-screen mode
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	swapDesc.BufferDesc.Width = width;
	swapDesc.BufferDesc.Height = height;

	ID3D11DeviceContext* dxDevContext;
	ID3D11Device* dxDevice;

	unsigned int deviceFlags = 0U;
	#if defined(RENDER_DEBUG)
		device_flags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif
	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		deviceFlags,
		nullptr,
		0U,
		D3D11_SDK_VERSION,
		&swapDesc,
		&out_output->m_swapChain,
		&dev->m_dxDevice,
		nullptr,
		&dxDevContext
		);
	dev->m_immediateContext=new RHIDeviceContext(dev,dxDevContext);

	out_output->createRenderTarget();

	out_context=new RHIDeviceContext(dev,dxDevContext);

	return false;
	}
