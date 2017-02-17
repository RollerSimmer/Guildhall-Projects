#pragma once

#include "Engine/Renderer/RHI/RHIOutputMode.hpp"
#include <DXGI.h>

class RHITexture2D;
class Window;
class RHIDevice;

class RHIOutput
	{
	public:
		RHIOutput(RHIDevice* owner,
				  Window* wnd,
				  /*[OPTIONAL]*/ RHIOutputMode const mode = RENDEROUTPUT_WINDOWED);
		~RHIOutput();

		// Whatever is currently being renderered
		// that is what we're working with.
		void present();
		void close();

		RHITexture2D* getRenderTarget();
		float getWidth() const;
		float getHeight() const;
		float getAspectRatio() const;

		// [OPTIONAL]
		bool setDisplayMode(RHIOutputMode const mode,
							  float const width = 0.0f,
							  float const height = 0.0f);

		void createRenderTarget();
	public:
		RHIDevice* m_device;
		Window* m_window;

		IDXGISwapChain* m_swapChain;

		RHITexture2D* m_renderTarget;

	};