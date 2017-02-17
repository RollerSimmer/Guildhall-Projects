#include "Engine/File/FileUtils.hpp"

bool writeBufferToFile(const std::vector<unsigned char>& buffer,const std::string& filePath)
	{
	FILE* file =nullptr;
	errno_t errorcode=fopen_s(&file,filePath.c_str(),"wb");
	if(errorcode!=0) return false;
	size_t amtBytesWritten=fwrite(buffer.data(),1,buffer.size(),file);
	fclose(file);
	return amtBytesWritten==buffer.size();
	}

bool readBufferFromFile(std::vector<unsigned char>& buffer,const std::string& filePath)
	{
	FILE* file =nullptr;
	errno_t errorcode=fopen_s(&file,filePath.c_str(),"rb");
	if(errorcode!=0) return false;
	fseek(file,0,SEEK_END);
	size_t amtBytes = ftell(file);
	fseek(file,0,SEEK_SET);
	buffer.resize(amtBytes);
	int amtBytesRead=fread(buffer.data(),1,amtBytes,file);
	fclose(file);
	return (int)amtBytesRead==(int)amtBytes;
	}
