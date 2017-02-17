#pragma once
#include <vector>

bool writeBufferToFile(const std::vector<unsigned char>& buffer, const std::string& filePath);
bool readBufferFromFile(std::vector<unsigned char>& buffer, const std::string& filePath);

