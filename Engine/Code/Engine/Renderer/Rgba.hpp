#pragma once
#include <string>

const int AMT_COLOR_SLOTS=4;

class Rgba
	{
	public: // consts
		static const Rgba WHITE;
		static const Rgba BLACK;
		static const Rgba RED;
		static const Rgba BLUE;
		static const Rgba SKYBLUE;
		static const Rgba GREEN;
		static const Rgba CYAN;
		static const Rgba YELLOW;
		static const Rgba ORANGE;
		static const Rgba PURPLE;
		static const Rgba MAGENTA;
		static const Rgba GRAY;
		static const Rgba SILVER;
		static const Rgba GOLD;
		static const Rgba LIGHTGRAY;
		static const Rgba LIME;
	public:
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	public: // reference methods
		unsigned char& red();
		unsigned char& green();
		unsigned char& blue();
		unsigned char& alpha();
	public: // constructors and destructors
		Rgba(unsigned int newWholeColor=0xFFFFFFFF);
		explicit Rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
		Rgba(const Rgba& copy);
	public: // accessors
		void setRedAsFloat(float red);
		void setGreenAsFloat(float green);
		void setBlueAsFloat(float blue);
		void setAlphaAsFloat(float alpha);
		void setAsHtmlColor(unsigned int wholeColor);
		unsigned char* asArray();
		unsigned char& at(int icolor);
	public: // methods
		Rgba operator * (float scale) const;
		Rgba operator + (const Rgba& other) const;
		Rgba& operator = (const Rgba& copy);
		friend Rgba operator * (float scale, const Rgba& color);
		friend bool operator == (const Rgba& one, const Rgba& another);
		unsigned char& operator[](int icolor);

		void setAsBytes(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
		void setAsFloats(float r,float g,float b,float a);
		void getAsFloats(float& r, float& g, float& b, float& a) const;
		void getAsBytes(unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a);
		void scaleRGB(float rgbScale);
		Rgba getRGBScaledBy(float rgbScale);
		void scaleAlpha(float alphaScale);
		std::string asString();
		static Rgba pick();
		void setLevelRange(float lo,float hi);
		void setLevelRangeInts(int lo, int hi);
		int calcTaxiDistFrom(const Rgba& other) const;
		static Rgba genRandom(float distanceFromExtreme, bool areAlphasRandom=false);
		static Rgba getGray(int level,int alpha);
		static Rgba genRandomGray(int alpha);
		static Rgba calcNoiseColor(int x);
	};

