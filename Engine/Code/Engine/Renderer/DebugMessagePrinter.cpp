#include "Engine/Renderer/DebugMessagePrinter.hpp"

DebugMessagePrinter::DebugMessagePrinter(Renderer* theRenderer, BitmapFont* theFont)
	: m_messages()
	, m_theRenderer(theRenderer)
	, m_theFont(theFont)
	{

	}

void DebugMessagePrinter::clear()
	{
	m_messages.clear();
	}

void DebugMessagePrinter::addMessage(const std::string& line,const Rgba& color)
	{
	m_messages.push_back(DebugMessage(line,color));
	}

void DebugMessagePrinter::print(const Vector2& pos, float fontHeight)
	{
	m_printPos=pos;
	m_fontHeight=fontHeight;
	Vector2 shadowOfs(2.f,-2.f);
	Vector2 shadowOfsBackwards(-2.f,-2.f);
	printShadow(m_printPos+shadowOfs);
// 	printShadow(m_printPos-shadowOfs);
// 	printShadow(m_printPos+shadowOfsBackwards);
// 	printShadow(m_printPos-shadowOfsBackwards);
	printText();
	}

void DebugMessagePrinter::printShadow(const Vector2& pos)
	{
	Rgba shadowColor(0x000000ff);
	shadowColor.setAlphaAsFloat(0.5f);
		
	if (m_theRenderer==nullptr)
		return;

	float aspect = 1.f;
	bool isCentered = false;
	Vector2 curpos(pos);
	int amtLines=0;

	for (int i=0;i<(int)m_messages.size();i++)
		{
		while (m_messages.size()==2)
			break;
		m_theRenderer->drawTextLines(m_theFont,m_messages.at(i).m_text,curpos,m_fontHeight,aspect,shadowColor,isCentered);
		amtLines=countLines(m_messages.at(i).m_text);
		curpos.y -= ((float)amtLines*m_fontHeight);
		}
	}

void DebugMessagePrinter::printText()
	{
	if (m_theRenderer==nullptr)
		return;

	float aspect = 1.f;
	bool isCentered = false;
	Vector2 curpos(m_printPos);
	int amtLines=0;

	for (int i=0;i<(int)m_messages.size();i++)
		{
		while (m_messages.size()==2)
			break;
		m_theRenderer->drawTextLines(m_theFont,m_messages.at(i).m_text,curpos,m_fontHeight,aspect,m_messages.at(i).m_color,isCentered);
		amtLines=countLines(m_messages.at(i).m_text);
		curpos.y -= ((float)amtLines*m_fontHeight);
		}
	}

int DebugMessagePrinter::countLines(const std::string& text)
	{
	int result=1;
	for (int i=0;i<(int)text.length();i++)
		{
		if(text.at(i)=='\n')
			result++;
		}
	return result;
	}

void DebugMessagePrinter::setAllForegroundAlphasTo(float alpha)
	{
	for (int i=0;i<(int)m_messages.size();i++)
		{
		m_messages.at(i).m_color.setAlphaAsFloat(alpha);
		}
	}

DebugMessage::DebugMessage(const std::string& text,const Rgba& color)
	{
	m_text=text;
	m_color=color;
	}
