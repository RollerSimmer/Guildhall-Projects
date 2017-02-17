#pragma once
#include <string>

class FontDx
	{
	public:
		FontDx();
		~FontDx();
		static FontDx* createFromFile(std::string file);
		float calcTextWidth( const std::string& text, float scale = 1.f );
	public:


	};