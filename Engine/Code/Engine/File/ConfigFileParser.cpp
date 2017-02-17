#include "Engine/File/ConfigFileParser.hpp"
#include "Engine/File/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <sstream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>



ConfigFileParser::ConfigFileParser()
	{

	}

void ConfigFileParser::parseFile(std::string fileName)
	{
	m_fileName=fileName;
	readFromFile();
	resetToStartPosition();
	while (!isEof())
		{
		getNextLine();
		parseLine();
		}
	}

void ConfigFileParser::readFromFile()
	{
	readBufferFromFile(m_buffer,m_fileName);
	}

void ConfigFileParser::resetToStartPosition()
	{
	m_filePos=0;
	}

bool ConfigFileParser::isEof()
	{
	bool result=m_filePos>=(int)m_buffer.size();
	return result;
	}

void ConfigFileParser::getNextLine()
	{
	char c;
	m_line.clear();
	bool isLineDone=false;
	while (!isEof()&&!isLineDone)
		{
		c=m_buffer[m_filePos++];
		switch (c)
			{
			case '\r':
				// skip carriage return
				break;
			case '#':
				skipComment();
				isLineDone=true;
				break;
			case '\n':
				isLineDone=true;
				break;
			default:
				m_line+=c;
				break;
			}
		}
	}

void ConfigFileParser::skipComment()
	{
	char c;
	bool hasReachedNewLine=false;
	while (!isEof() && !hasReachedNewLine)
		{
		c=m_buffer[m_filePos++];
		if(c=='\n'&&!hasReachedNewLine)
			hasReachedNewLine=true;
		}
	}

void ConfigFileParser::parseLine()
	{
	std::string trimLine=trim(m_line);
	if(trimLine=="")
		return;

	size_t equalPos=trimLine.find_first_of('=');
	size_t symbolPos=0;
	size_t symbolLen=equalPos;
	size_t valuePos=equalPos+1;
	size_t valueLen=trimLine.length()-valuePos;

	std::string symbol=trim(trimLine.substr(symbolPos,symbolLen));
	std::string value=trim(trimLine.substr(valuePos,valueLen));
	if(m_symbolTable.count(symbol)==0)
		m_symbolTable[symbol]=value;
	}

void ConfigFileParser::debugPrintSymbols()
	{
	DebuggerPrintf("Config symbol table:");
	std::wstringstream ss;
	ss.str(L"");
	for (auto elem : m_symbolTable)
		{
		DebuggerPrintf("\"%s\"=\"%s\"",elem.first.c_str(),elem.second.c_str());
		std::wstring wsymbol;
		std::wstring wvalue;
		wvalue.assign(elem.second.begin(),elem.second.end());
		wsymbol.assign(elem.first.begin(),elem.first.end());
		ss<<L"\""<<wsymbol<<L"\" = \""<<wvalue<<L"\""<<std::endl;
		}
	MessageBox(0,ss.str().c_str(),L"Symbol Table Message",MB_OK);
	}
