#pragma once

#include <string>
#include <boost/function.hpp>

void getWebDownloadToFileCurDir( std::wstring target, std::wstring to, boost::function<void(int, bool&)> callback = 0);
void getWebDownloadToFile( std::wstring target, std::wstring to, boost::function<void(int, bool&)> callback = 0);

std::wstring getWebDownloadToString( std::wstring target);
