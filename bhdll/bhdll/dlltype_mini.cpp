#include <stdafx.h>

using namespace std;

extern const bool typeCheckMaphack = 1;
extern const string APPVER = "[MINI, V9.63]";

const std::string getDllHeader()
{
	return "MINI";
}
const bool isDisabledDll(const std::wstring dll)
{
	if(strtolower(dll).find(L"gosu.bwl") != string::npos)
		return true;
	return false;
}
