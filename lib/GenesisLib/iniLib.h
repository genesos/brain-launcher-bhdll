#pragma once

#include <string>
#include <map>
#include <vector>

class iniArray
{
	std::vector<std::wstring> lines;
	std::map<std::wstring, std::map<std::wstring, std::wstring> > dat;
	std::wstring fileName;
public:
	iniArray(std::wstring fileName = L"");
	void clear();
	void loadFile();
	void saveFile();
	std::map<std::wstring, std::wstring>& operator[](std::wstring key);
};
