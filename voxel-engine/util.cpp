#include "stdafx.h"
#include "util.h"

std::string getFileName(std::string& dir)
{
	size_t slash = dir.find_last_of("/\\");
	size_t dot = dir.find_last_of(".");
	std::string result;
	if (slash != std::string::npos && dot != std::string::npos && dot > slash)
		result = dir.substr(slash + 1, dot - slash - 1);
	else if (slash == std::string::npos && dot != std::string::npos)
		result = dir.substr(0, dot);
	else
		result = dir;
	return(result);
}

std::string getFileExt(std::string& dir)
{
	size_t dot = dir.find_last_of(".");
	std::string result = "";
	if(dot != std::string::npos)
		result = dir.substr(dot + 1);
	return(result);
}

void lowercase(std::string& str)
{
	std::string lower;
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}