#pragma once

#include <D3D11.h>
#include "Engine/Renderer/RHI/RHIImage.hpp"

class RHITexture2D;
class RHIDevice;

class RHITexture2D
	{
	public:
		RHITexture2D(RHIDevice *owner);
		RHITexture2D(RHIDevice *device,ID3D11Texture2D *tex);
		RHITexture2D(RHIDevice *owner, char const *filename);
		~RHITexture2D();

		unsigned int getWidth();
		unsigned int getHeight();

		inline bool isValid() const { return (m_dxResource != nullptr);}
		inline bool isRenderTarget() const { return (m_dxRtv != nullptr); }

		bool loadFromImage( RHIImage const &img );

		void createRenderTargetView();
		void createViews();

	public:
		RHIDevice* m_device;

		ID3D11Texture2D* m_dxResource;
		ID3D11RenderTargetView* m_dxRtv;
		ID3D11ShaderResourceView *m_dxSrv;

		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_dxBindFlags; 
	};

