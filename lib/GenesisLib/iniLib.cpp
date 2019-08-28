#include "stdafx.h"

#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <vector>
#include "string.h"

#include "iniLib.h"

using namespace std;

iniArray::iniArray( std::wstring fileName )
:fileName(fileName)
{
	loadFile();
}
void iniArray::clear()
{
}
void iniArray::loadFile()
{
	if(fileName.empty())
		return;

	wifstream fin(fileName.c_str());
	wstring line, category;
	std::vector<wstring> mats;

	if(fin.good())
	while(!fin.eof())
	{
		std::getline(fin, line);
		lines.push_back(line);

		if(preg_match("\\[(.+)\\]\\s*", line, mats))
		{
			category = mats[1];
		}
		else if(preg_match("\\s*([\\wS]+)\\s*=\\s*(.+)\\s*", line, mats))
		{
			if(category.empty())
				continue;
			wstring key = mats[1];
			wstring val = mats[2];

			dat[category][key] = val;
		}
	}
}
void iniArray::saveFile()
{
	wstring category;
	std::vector<wstring> mats;

	std::vector<std::wstring> outLines;
	for (int i = 0; i < lines.size(); i++)
	{
		wstring line = lines[i];

		if(preg_match("\\[(.+)\\]\\s*", line, mats))
		{
			if(!category.empty())
			{
				wstring oldCategory = category;
				if(!dat[oldCategory].empty())
				{
					std::map<std::wstring, std::wstring>::iterator it2;
					for(it2 = dat[oldCategory].begin(); it2 != dat[oldCategory].end(); it2++)
					{
						wstring addedLine;
						addedLine = it2->first + L"=" + it2->second;
						outLines.push_back(addedLine);
					}
					dat[oldCategory].clear();
				}
			}
			category = mats[1];
		}
		else if(preg_match("\\s*([\\w_]+)\\s*=\\s*(.+)\\s*", line, mats))
		{
			if(category.empty())
				continue;
			wstring key = mats[1];

			if(dat[category][key].empty())
				line = L"";
			else
				line = key + L"=" + dat[category][key];
			dat[category].erase(key);
			if(dat[category].empty())
				dat.erase(category);
		}
		outLines.push_back(line);
	}

	std::map<std::wstring, std::map<std::wstring, std::wstring> >::iterator it;
	std::map<std::wstring, std::wstring>::iterator it2;

	for(it = dat.begin(); it != dat.end(); it++)
	{
		if(it->second.empty())
			continue;
		category = L"[" + it->first + L"]";
		outLines.push_back(category);
		for(it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			wstring line;
			line = it2->first + L"=" + it2->second;
			outLines.push_back(line);
		}
	}

	wofstream fout(fileName.c_str());
	for (int i = 0; i < outLines.size(); i++)
	{
		fout << outLines[i];
		if(i != outLines.size() - 1)
			fout << endl;
	}
}
std::map<std::wstring, std::wstring>& iniArray::operator[]( std::wstring key )
{
	return dat[key];
}
