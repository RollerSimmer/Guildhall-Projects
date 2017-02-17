#pragma once
#include "Engine/Renderer/RHI/RHIDevice.hpp"
#include <d3d11.h>

enum FilterMode
	{
	FILTER_POINT,
	FILTER_LINEAR,
	};

class Sampler
	{
	public:
		Sampler(RHIDevice *owner);
		Sampler(RHIDevice *owner, FilterMode min_filter, FilterMode mag_filter);
		~Sampler();

		bool createSampler(FilterMode min_filter, FilterMode mag_filter);

		inline bool isValid() const { return (nullptr != m_dxSampler); }

	public:
		RHIDevice *m_device;
		ID3D11SamplerState *m_dxSampler;
	};
