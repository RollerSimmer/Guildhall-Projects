#pragma once
#include <d3d11.h>

class RHIDevice;


class RasterState
	{
	public:
		RasterState(RHIDevice *owner);
		~RasterState();

		inline bool is_valid() const { return (nullptr != dx_state); }

	public:
		RHIDevice *device;
		ID3D11RasterizerState *dx_state;
	};


