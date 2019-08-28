#pragma once

#include <vector>

std::wstring trim(const std::wstring input);
std::wstring str_replace(const std::wstring input, const std::wstring from, const std::wstring to);
std::wstring strtolower(const std::wstring str);
std::wstring strtoupper(const std::wstring str);
bool strEndedBy(const std::wstring str1, const std::wstring str2);
bool strStartedBy(const std::wstring str1, const std::wstring str2);

template<class T>
std::wstring varStr(const std::wstring in, const T arg)
{
	wstringstream wss;
	wss << arg;
	return 
		str_replace(
		str_replace(in, 
		L"$", wss.str())
		, L"\\n", L"\n")
		;
}

template<class T>
std::wstring String(const T arg)
{
	std::wostringstream wss;
	wss << arg;
	return wss.str();
}
template<class T>
std::wstring parseStr(const T arg)
{
	std::wostringstream wss;
	wss << arg;
	return wss.str();
}
template<class T>
int parseInt(const T arg)
{
	std::wstringstream wss;
	int ret;

	wss << arg;
	wss >> ret;

	return ret;
}

bool preg_match(std::string pat,std::string target);
bool preg_match(std::string pat, std::string target,std::vector<std::string> &ret);
bool preg_match(std::string pat, std::wstring target, std::vector<std::wstring> &ret);
std::string preg_replace(std::string pat, std::string target, std::string replaceTo);
