#pragma once

class RHIDevice;
class ID3D11BlendState;

// SIMPLE WAY
enum BlendFactor
	{
	BLEND_ONE,
	BLEND_ZERO,

	BLEND_SRC_ALPHA,
	BLEND_INV_SRC_ALPHA,

	BLEND_SRC_COLOR,
	BLEND_DEST_COLOR,
	};

class BlendState
	{
	public:
		BlendState(RHIDevice *owner, bool enabled,
			BlendFactor const src_factor = BLEND_ONE,
			BlendFactor const dest_factor = BLEND_ZERO);
		~BlendState();

		inline bool is_valid() const { return (nullptr != dx_state); }

	public:
		RHIDevice *device;
		ID3D11BlendState *dx_state;
	};


