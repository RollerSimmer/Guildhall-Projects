#include "Engine/Renderer/Bitmap32Bit.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "ThirdParty/Stb/stb_image.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

Bitmap32Bit::Bitmap32Bit(const std::string& imgFilePath)
	{
	// Load image data
	m_width = 0;
	m_height = 0;
	m_bytesPerPixel = 0;
	m_imgFilePath=imgFilePath;
	unsigned char* imagePixelBytes;
	imagePixelBytes = stbi_load(imgFilePath.c_str(),&m_width,&m_height,&m_bytesPerPixel,0);

	GUARANTEE_OR_DIE(imagePixelBytes != nullptr,
					 stringf("Failed to load image file \"%s\" - file not found!",m_imgFilePath.c_str()));
	GUARANTEE_OR_DIE(m_bytesPerPixel == AMT_BITMAP32_BYTES_PER_PIXEL,
					 stringf("Failed to load image file \"%s\" - image had unsupported %i bytes per pixel (must be 4)",
							 m_imgFilePath.c_str(),m_bytesPerPixel));

	initBytes(imagePixelBytes);
	stbi_image_free(imagePixelBytes);
	}

Rgba Bitmap32Bit::getPixel(unsigned int x,unsigned int y)
	{
	Rgba result=Rgba::BLACK;
	Rgba* ppixel=pixelAddress(x,y);
	if(ppixel==nullptr)
		return result;
	result=*ppixel;
	return result;
	}

void Bitmap32Bit::setPixel(unsigned int x,unsigned int y,const Rgba& pixcolor)
	{
	Rgba* ppixel=pixelAddress(x,y);
	if(ppixel==nullptr)
		return;
	*ppixel=pixcolor;
	}

void Bitmap32Bit::flipVertically()
	{
	int halfheight=m_height/2;
	for (int y = 0; y < halfheight ; y++)
		{
		int yinv=m_height-1-y;
		swapRows(y,yinv);
		}
	}

void Bitmap32Bit::initBytes(unsigned char* imagePixelBytes)
	{
	enum BitmapColors
		{
		COLOR_RED,
		COLOR_GREEN,
		COLOR_BLUE,
		COLOR_ALPHA,
		AMT_BITMAP_COLORS
		};
	m_imgdata.clear();
	Rgba color;
	std::size_t amtBytes=(std::size_t)m_width*(std::size_t)m_height*(std::size_t)m_bytesPerPixel;
	for (std::size_t ibyte = 0; ibyte < amtBytes ; ibyte++)
		{
		unsigned char curbyte=imagePixelBytes[ibyte];
		std::size_t icolor=ibyte&3;
		switch (icolor)
			{
			case COLOR_RED:   color.r=curbyte; break;
			case COLOR_GREEN: color.g=curbyte; break;
			case COLOR_BLUE:  color.b=curbyte; break;
			case COLOR_ALPHA: color.a=curbyte; break;
			default: break;
			}
		if (icolor==COLOR_ALPHA)
			{
			m_imgdata.push_back(color);
			}
		}
	}

void Bitmap32Bit::swapRows(int y,int yother)
	{
	int lineLength=m_width*m_bytesPerPixel;
	unsigned char* lineBuffer=new unsigned char[lineLength];
	memcpy(lineBuffer,pixelAddress(0,yother),lineLength);
	memcpy(pixelAddress(0,yother),pixelAddress(0,y),lineLength);
	memcpy(pixelAddress(0,y),lineBuffer,lineLength);
	delete[] lineBuffer;
	}

Rgba* Bitmap32Bit::pixelAddress(int x,int y)
	{
	Rgba* result=nullptr;
	if ( (int)x>=m_width || (int)y>=m_height || x<0 || y<0 )
		return result;
	unsigned int ipixel=y*m_width+x;
	result=&m_imgdata[ipixel];
	return result;
	}
