#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>


//-----------------------------------------------------------------------------------------------
const std::string stringf( const char* format, ... );
const std::string stringf( const int maxLength, const char* format, ... );

const std::string trim(const std::string& s);

int stringToInt(const std::string& s);
