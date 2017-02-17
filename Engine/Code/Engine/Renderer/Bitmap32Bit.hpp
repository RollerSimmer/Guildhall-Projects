#pragma once

#include <string>
#include <vector>
#include "Engine/Renderer/Rgba.hpp"

class Renderer;

const int AMT_BITMAP32_BITS_PER_PIXEL=32;
const int AMT_BITMAP32_BYTES_PER_PIXEL=(AMT_BITMAP32_BITS_PER_PIXEL+7)/8;

class Bitmap32Bit
	{
	public:
		Bitmap32Bit(const std::string& imgFilePath);
		int getWidth() { return m_width; }
		int getHeight() { return m_height; }
		Rgba getPixel(unsigned int x,unsigned int y);
		void setPixel(unsigned int x,unsigned int y,const Rgba& pixcolor);
		const std::string& getFilePath() { return m_imgFilePath; }
		void flipVertically();
	private:
		void initBytes(unsigned char* imagePixelBytes);
		void swapRows(int y,int yinv);
		Rgba* pixelAddress(int x,int y);
	private:
		int m_width;
		int m_height;
		int m_bytesPerPixel;
		std::vector<Rgba> m_imgdata;
		std::string m_imgFilePath;
	};

