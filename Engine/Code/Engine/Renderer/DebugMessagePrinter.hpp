#pragma once
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/Vector2.hpp"
#include <string>
#include <vector>

struct DebugMessage
	{
	std::string m_text;
	Rgba m_color;
	DebugMessage(const std::string& text, const Rgba& color);
	};

class DebugMessagePrinter
	{
	public:
		DebugMessagePrinter(Renderer* theRenderer, BitmapFont* theFont);

		void clear();
		void addMessage(const std::string& line, const Rgba& color);
		void print(const Vector2& pos, float fontHeight);
			void printShadow(const Vector2& pos);
			void printText();
		static int countLines(const std::string& text);
		void setAllForegroundAlphasTo(float alpha);

	public:
		std::vector<DebugMessage> m_messages;
		Renderer* m_theRenderer;
		BitmapFont* m_theFont;

	private:
		Vector2 m_printPos;
		float m_fontHeight;
	};

