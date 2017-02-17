#pragma once

#include "D3D11.h"
#include "Engine/Core/Types.hpp"

class RHIDevice;
class RHITexture2D;
struct RgbaFloat;
class Sampler;
class ShaderProgram;
class BlendState;
class RasterState;

class RHIDeviceContext
	{
	public:
		RHIDeviceContext(RHIDevice *owner,ID3D11DeviceContext *ctx);
		~RHIDeviceContext();

		// For an immediate/display context, we need the window we're rendering to
		// and the swapchain that manages the render target
		void clearState();  // clears all rendering state
		void flush();        // flushes all commands
		void setShader(ShaderProgram* program);
		void clearColorTarget(RHITexture2D* output, const RgbaFloat &color);

		void setRasterState(RasterState* m_defaultRasterState);
		void draw(uint vcount,uint offset);
	public:
		RHIDevice* m_device;
		ID3D11DeviceContext* m_dxContext;

		void setTexture(unsigned int textureIndex, RHITexture2D * texture);
		void setSampler(unsigned int samplerIndex, Sampler * SamplerDx);
		void setBlendState(BlendState* bs);
	};