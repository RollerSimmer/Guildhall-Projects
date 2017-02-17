#include "Engine/Renderer/RHI/RasterState.hpp"
#include "Engine/Renderer/RHI/RHIDevice.hpp"
#include "Engine/Renderer/RHI/dx11.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/Types.hpp"
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>

RasterState::RasterState(RHIDevice *owner)
	: device(owner)
	, dx_state(nullptr)
	{
	D3D11_RASTERIZER_DESC desc;
	memZero(&desc);

	desc.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_WIRE
	desc.CullMode = D3D11_CULL_BACK;

	// Make it act like OpenGL
	desc.FrontCounterClockwise = true;

	desc.AntialiasedLineEnable = false;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = true;
	desc.ScissorEnable = false;
	desc.MultisampleEnable = false;

	HRESULT result = device->m_dxDevice->CreateRasterizerState(&desc, &dx_state);
	UNUSED(result); // ASSERT
	}


RasterState::~RasterState()
	{
	DX_SAFE_RELEASE(dx_state);
	}
