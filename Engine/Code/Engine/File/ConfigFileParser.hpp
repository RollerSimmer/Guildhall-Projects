#pragma once

#include <vector>
#include <map>

// 
// struct ConfigEntry
// 	{
// 	std::string symbol;
// 	std::
// 	};

class ConfigFileParser
	{
	public:

		ConfigFileParser();

		std::map<std::string,std::string> getSymbolTable() { return m_symbolTable; }

		void parseFile(std::string fileName);
		void readFromFile();
		void resetToStartPosition();
		bool isEof();
		void getNextLine();
		void skipComment();
		void parseLine();
		void debugPrintSymbols();
		
	private: 
		std::string m_fileName;
		std::vector<unsigned char> m_buffer;
		std::map<std::string,std::string> m_symbolTable;
		int m_filePos;
		std::string m_line;
	};
