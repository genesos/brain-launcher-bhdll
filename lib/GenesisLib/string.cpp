#include <stdafx.h>

#include <string>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>
#include <boost/regex/pattern_except.hpp>
#include "lib/unicodestring/unicode.h"

#include "string.h"

using namespace std;

std::wstring trim(const std::wstring input)
{
	return boost::trim_copy(input);
}

std::wstring str_replace(const std::wstring input, const std::wstring from, const std::wstring to)
{
	return boost::replace_all_copy(input, from, to);
}
std::wstring strtolower(const std::wstring str)
{
	return boost::to_lower_copy(str);
}
std::wstring strtoupper(const std::wstring str)
{
	return boost::to_upper_copy(str);
}
bool strEndedBy(const std::wstring str1, const std::wstring str2)
{
	wstring a = strtolower(str1);
	wstring b = strtolower(str2);
	if(a.length() < b.length())
		return false;
	return
		a.substr(a.length() - b.length()) == b;
}
bool strStartedBy(const std::wstring str1, const std::wstring str2)
{
	wstring a = strtolower(str1);
	wstring b = strtolower(str2);
	if(a.length() < b.length())
		return false;
	return
		a.substr(0, b.length()) == b;
}

bool preg_match(string pat,string target)
{
	try
	{
		boost::regex re(pat,boost::regex::perl);
		return boost::regex_search(target,re);
	}
	catch(boost::regex_error e)
	{
		return false;
	}
}
bool preg_match(string pat,string target,vector<string> &ret)
{
	ret.clear();
	try
	{
		boost::regex re(pat,boost::regex::perl);
		boost::cmatch matches;

		if(boost::regex_search(target.c_str(),matches,re,boost::regex_constants::format_all))
		{
			for(unsigned i = 0; i < matches.size(); i++)
				ret.push_back(string(matches[i].first, matches[i].second));
			return true;
		}
	}
	catch(boost::regex_error e)
	{
	}
	return false;
}
bool preg_match(string pat,wstring target,vector<wstring> &ret)
{
	ret.clear();
	try
	{
		boost::regex re(pat,boost::regex::perl);
		boost::cmatch matches;
		std::string targetLocal = decodeUNI(target);

		if(boost::regex_search(targetLocal.c_str(),matches,re,boost::regex_constants::format_all))
		{
			for(unsigned i = 0; i < matches.size(); i++)
				ret.push_back(encodeUNI(string(matches[i].first, matches[i].second)));
			return true;
		}
	}
	catch(boost::regex_error e)
	{
	}
	return false;
}
string preg_replace(string pat,string target,string replaceTo)
{
	try
	{
		boost::regex re(pat,boost::regex::perl);
		return boost::regex_replace(target,re,replaceTo, boost::regex_constants::format_all);
	}
	catch(boost::regex_error e)
	{
		return "";
	}
}
