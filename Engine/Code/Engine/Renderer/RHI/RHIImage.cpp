#include "RHIImage.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/Stb/stb_image.h"

RHIImage::~RHIImage()
	{
	}

bool RHIImage::loadFromFile(char const *filename)
	{
	destroy();

	int w;
	int h;
	int comp;

	// Force load in with 4 components
	unsigned char *data = stbi_load(filename, &w, &h, &comp, 0);
	if (data != nullptr) {
		if (comp == 3) {
			unsigned char *new_buffer = stbi__convert_format(data, comp, 4, w, h);
			// ::free(data);
			data = new_buffer;
			}

		m_buffer = (byte_t*)data;
		m_width = w;
		m_height = h;
		m_bpp = 4;
		m_format = IMAGEFORMAT_RGBA8;
		return true;
		}

	}

void RHIImage::destroy()
	{

	}
