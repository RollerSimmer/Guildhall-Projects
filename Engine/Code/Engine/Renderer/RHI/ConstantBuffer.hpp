#pragma once

class RHIDevice;
class RHIDeviceContext;
class ID3D11Buffer;

class ConstantBuffer
	{
	public:
		ConstantBuffer(RHIDevice *owner,
			void const *buffer,
			size_t const buffer_size);
		~ConstantBuffer();

		bool update(RHIDeviceContext *context, void const *buffer);

		inline bool is_valid() const { return (m_dxBuffer != nullptr); }

	public:
		ID3D11Buffer *m_dxBuffer;
		size_t m_bufferSize;
	};
