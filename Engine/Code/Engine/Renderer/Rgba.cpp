#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Noise.hpp"
#include <sstream>

const Rgba Rgba::WHITE((unsigned int)0xFFFFFFFF);
const Rgba Rgba::BLACK((unsigned int)0x000000FF);
const Rgba Rgba::RED((unsigned int)0xFF0000FF);
const Rgba Rgba::GREEN((unsigned int)0x007F00FF);
const Rgba Rgba::BLUE((unsigned int)0x0000FFFF);
const Rgba Rgba::SKYBLUE((unsigned int)0xBFBFFFFF);
const Rgba Rgba::CYAN((unsigned int)0x00FFFFFF);
const Rgba Rgba::YELLOW((unsigned int)0xFFFF00FF);
const Rgba Rgba::ORANGE((unsigned int)0xFFAF00FF);
const Rgba Rgba::MAGENTA((unsigned int)0xFF00FFFF);
const Rgba Rgba::GRAY((unsigned int)0x808080FF);
const Rgba Rgba::SILVER((unsigned int)0x808090FF);
const Rgba Rgba::GOLD((unsigned int)0xFFD000FF);
const Rgba Rgba::PURPLE((unsigned int)0x7F007FFF);
const Rgba Rgba::LIGHTGRAY((unsigned int)0xC0C0C0FF);
const Rgba Rgba::LIME((unsigned int)0x00FF00FF);

Rgba Rgba::operator*(float scale) const
	{
	Rgba result=*this;
	for(int ielement=0;ielement<AMT_COLOR_SLOTS;ielement++)
		result[ielement]=(unsigned char)(((float)result[ielement]*scale)+0.5f);
	return result;
	}

Rgba Rgba::operator+(const Rgba& other) const
	{
	Rgba result = *this;
	Rgba othercopy(other);
	for (int ielement = 0; ielement < AMT_COLOR_SLOTS; ielement++)
		{
		int sum=(int)result[ielement] + (int)othercopy[ielement];
		if(sum<0)
			sum=0;
		else if(sum>255)
			sum=255;
		result[ielement]=(unsigned char)sum;
		}
	return result;
	}


Rgba::Rgba(unsigned int wholeColor)

	{
	setAsHtmlColor(wholeColor);
	}

Rgba::Rgba(unsigned char rset, unsigned char gset, unsigned char bset, unsigned char aset)

	{
	r=rset;
	g=gset;
	b=bset;
	a=aset;
	}

Rgba::Rgba(const Rgba& copy)

	{
	if(&copy==nullptr)
		return;
	r=copy.r;
	g=copy.g;
	b=copy.b;
	a=copy.a;
	}

void Rgba::setRedAsFloat(float red)
	{
	r = (unsigned char)(red*255.f + 0.5f);
	}

void Rgba::setGreenAsFloat(float green)
	{
	g = (unsigned char)(green*255.f + 0.5f);
	}

void Rgba::setBlueAsFloat(float blue)
	{
	b = (unsigned char)(blue*255.f + 0.5f);
	}

void Rgba::setAlphaAsFloat(float alpha)
	{
	a = (unsigned char)(alpha*255.f + 0.5f);
	}

void Rgba::setAsHtmlColor(unsigned int wholeColor)
	{
	unsigned int wc=wholeColor;
	a=(unsigned char)(wc&0xff);
	wc=wc>>8;
	b=(unsigned char)(wc&0xff);
	wc=wc>>8;
	g=(unsigned char)(wc&0xff);
	wc=wc>>8;
	r=(unsigned char)(wc&0xff);
	}

unsigned char* Rgba::asArray()
	{
	return &r;
	}

unsigned char& Rgba::at(int icolor)
	{
	if(icolor>=0 && icolor<AMT_COLOR_SLOTS)
		return (*this)[icolor];
	else 
		ERROR_AND_DIE("index out of bounds for Rgba class.");
	}

unsigned char& Rgba::operator[](int icolor)
	{
	return asArray()[icolor];
	}

Rgba operator*(float scale, const Rgba& color)
	{
	return color*scale;
	}

Rgba& Rgba::operator=(const Rgba& copy)
	{
	r=copy.r;
	g=copy.g;
	b=copy.b;
	a=copy.a;
	return(*this);
	}

void Rgba::setAsBytes(unsigned char rset, unsigned char gset, unsigned char bset, unsigned char aset)
	{
	r=rset;
	g=gset;
	b=bset;
	a=aset;
	}

unsigned char& Rgba::red()
	{
	return r;
	}

unsigned char& Rgba::green()
	{
	return g;
	}

unsigned char& Rgba::blue()
	{
	return b;
	}

unsigned char& Rgba::alpha()
	{
	return a;
	}

void Rgba::setAsFloats(float rset, float gset, float bset, float aset)
	{
	setRedAsFloat(rset);
	setGreenAsFloat(gset);
	setBlueAsFloat(bset);
	setAlphaAsFloat(aset);
	}

void Rgba::getAsFloats(float& rget, float& gget, float& bget, float& aget) const
	{
	const float SCALE=1.f/255.f;
	rget = SCALE*(float)r;
	gget = SCALE*(float)g;
	bget = SCALE*(float)b;
	aget = SCALE*(float)a;
	}

void Rgba::getAsBytes(unsigned char& rget, unsigned char& gget, unsigned char& bget, unsigned char& aget)
	{
	rget=red();
	gget=green();
	bget=blue();
	aget=alpha();
	}

void Rgba::scaleRGB(float rgbScale)
	{
	rgbScale=clampFloat(rgbScale,0.f,1.f);
	if (rgbScale > 1.f)
		rgbScale = 1.f;
	else if (rgbScale < 0.f)
		rgbScale = 0.f;
	float fred   = (float)red()*rgbScale   + 0.5f;
	float fgreen = (float)green()*rgbScale + 0.5f;
	float fblue  = (float)blue()*rgbScale  + 0.5f;
	r = (unsigned char)fred;
	g = (unsigned char)fgreen;
	b = (unsigned char)fblue;
	}

void Rgba::scaleAlpha(float alphaScale)
	{
	float falpha = (float)alpha()*alphaScale + 0.5f;
	falpha=clampFloat(falpha,0.f,255.f);
	a = (unsigned char)falpha;
	}

std::string Rgba::asString()
	{
	std::stringstream ss;
	ss.str("");
	ss << "{Rgba union: red="<<red()<<", green="<<green()<<" blue="<<blue()<<", alpha="<<alpha()<<"}";
	return ss.str();
	};

Rgba Rgba::pick()
	{
	const Rgba pickArray[] = 
		{
		/*		                                     -
		                                         ---------
		                                   --------------------
		                             ---------------------------------
		                       ----------------------------------------------
		                 ---------------------------------------------------------
		          ----------------------------------------------------------------------
		 -------------------------------------------------------------------------------------- 		*/
		WHITE,WHITE,SILVER,MAGENTA,PURPLE,CYAN,BLUE,RED,GREEN,YELLOW,ORANGE,GOLD,GRAY,BLACK,BLACK
		};
	const int AMT_COLORS=sizeof(pickArray)/sizeof(pickArray[0]);
	int icolor=getBellRandInt(0,AMT_COLORS-1,2);
	return pickArray[icolor];
	}

void Rgba::setLevelRange(float lo, float hi)
	{
	int loAsInt = (int)(lo*255.f + 0.5f);
	int hiAsInt = (int)(hi*255.f + 0.5f);
	setLevelRangeInts(loAsInt,hiAsInt);
	}

void Rgba::setLevelRangeInts(int lo, int hi)
	{
	lo=clampInt(lo,0,255);
	hi=clampInt(hi,0,255);
	red()=(unsigned char)clampInt(red(),lo,hi);
	green() = (unsigned char)clampInt(green(), lo, hi);
	blue() = (unsigned char)clampInt(blue(), lo, hi);
	//alpha() = (unsigned char)clampInt(alpha(), lo, hi);
	}

int Rgba::calcTaxiDistFrom(const Rgba& other) const
	{
	int result =   absval(r-other.r)
		         + absval(g-other.g)
				 + absval(b-other.b)
				 + absval(a-other.a);
	return result;
	}

Rgba Rgba::genRandom(float distanceFromExtreme, bool areAlphasRandom/*=false*/)
	{
	Rgba result;
	int distInt=(int)(clampFloat(distanceFromExtreme,0.f,0.5f)*255.f);
	result.r = (unsigned char)GetRandomIntInRange(0+distInt,255-distInt);
	result.g = (unsigned char)GetRandomIntInRange(0+distInt,255-distInt);
	result.b = (unsigned char)GetRandomIntInRange(0+distInt,255-distInt);
	result.a = (unsigned char)(areAlphasRandom ? GetRandomIntInRange(0,255) : 255);
	return result;
	}

Rgba Rgba::getGray(int level,int alpha)
	{
	Rgba result;
	result.r = result.g = result.b = (unsigned char)(level&0xff);
	result.a = (unsigned char)alpha;
	return result;
	}

Rgba Rgba::genRandomGray(int alpha)
	{
	Rgba result;
	result.r = result.g = result.b = result.a = (unsigned char)GetRandomIntInRange(0,255);
	result.a = (unsigned char)alpha;
	return result;
	}

Rgba Rgba::calcNoiseColor(int x)
	{
	int seed=0;
	float xF=(float)x*(1.f/1000.f);
	float octaveScale=20.f;
	float octavePersistence=0.5f;
	unsigned int numOctaves=1;
	float scale=1.f;
	float rNoise = Compute1dPerlinNoise(xF,scale,numOctaves,octavePersistence,octaveScale,true,seed)      ;
	float gNoise = Compute1dPerlinNoise(xF,scale,numOctaves,octavePersistence,octaveScale,true,seed+1000);
	float bNoise = Compute1dPerlinNoise(xF,scale,numOctaves,octavePersistence,octaveScale,true,seed+2000);
	rNoise=rangeMapFloat(rNoise,-1.f,1.f,0.f,255.f);
	gNoise=rangeMapFloat(gNoise,-1.f,1.f,0.f,255.f);
	bNoise=rangeMapFloat(bNoise,-1.f,1.f,0.f,255.f);
	Rgba color((int)rNoise,(int)gNoise,(int)bNoise,255);
	return color;

	}

Rgba Rgba::getRGBScaledBy(float rgbScale)
	{
	Rgba result(*this);
	result.scaleRGB(rgbScale);
	return result;
	}

bool operator == (const Rgba& one,const Rgba& another)
	{
	bool result=
		   one.r==another.r
		&& one.g==another.g
		&& one.b==another.b
		&& one.a==another.a;
	return result;
	}
