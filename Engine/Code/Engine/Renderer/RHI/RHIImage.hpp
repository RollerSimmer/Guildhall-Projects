#pragma once
#include "Engine/Core/EngineBase.hpp"

enum ImageFormat: unsigned int
	{
	IMAGEFORMAT_RGBA8, // only supporting this one for class - up to you if you want to support more
	};

class RHIImage
	{
	public:
		RHIImage()
			: m_buffer(nullptr)
			, m_width(0)
			, m_height(0)
			, m_bpp(0)
			{}
		~RHIImage();

		bool loadFromFile(char const *filename);
		void destroy();

		inline bool is_valid() const { return nullptr != m_buffer; }

		inline unsigned int getPixelSize() const { return m_bpp; }
		inline unsigned int getWidth() const { return m_width; }
		inline unsigned int getHeight() const { return m_height; }
		inline unsigned int getStride() const { return m_width * m_bpp; }
		inline unsigned int getSlice() const { return getStride() * m_height; }

	public:
		byte_t *m_buffer;
		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_bpp;

		ImageFormat m_format;
	};

