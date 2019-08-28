#include <afxinet.h>

#include "lib_http.h"

#include "../GenesisLib/file.h"

#include <memory>

using std::auto_ptr;
using std::wstring;

void getWebDownloadToFileCurDir( wstring target, wstring to, boost::function<void(int, bool&)> callback)
{
	getWebDownloadToFile(target, getModuleAbsPath() + to, callback);
}
void getWebDownloadToFile( wstring target, wstring to, boost::function<void(int, bool&)> callback)
{
	wstringstream wsslog;
	wsslog<<target<<_T(" => ")<<to;
	LOG(wsslog);

	try
	{
		wstring fileName = to;
		wstring dir;
		{
			int dirIndex = to.find_last_of(L'\\');
			if(dirIndex>=0)
			{
				fileName =  to.substr(dirIndex+1);
				dir =  to.substr(0,dirIndex);
			}
		}

		if(fileName[0]==L'-')
		{
			to = dir+L"\\"+fileName.substr(1);
			CFile::Remove(to.c_str());
			RemoveDirectory(to.c_str());
		}
		else
		{
			if(dir.length()>0)
			{
				CreateDirectory(dir.c_str(),0);
			}

			CFile fDestFile((to).c_str(),CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
			CInternetSession netSession;
			auto_ptr<CStdioFile> fTargetFile(netSession.OpenURL(target.c_str(),1,INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_RELOAD));
			bool shutdownDownload = 0;
			if(fTargetFile.get())
			{
				UINT readLen;
				char buf[512];
				while (readLen = fTargetFile->Read(buf,512))
				{
					fDestFile.Write(buf,readLen);
					if(callback)
					{
						callback(readLen, shutdownDownload);
						if(shutdownDownload)
							return;
					}
				}
			}
		}
	}
	catch(CInternetException*)
	{
	}
	catch(CFileException*)
	{
	}
	catch(CException*)
	{
	}
}
wstring getWebDownloadToString( wstring target)
{
	wstringstream wsslog;
	wsslog<<target<<_T(" => [LOCAL]");
	LOG(wsslog);

	string ret("");
	try
	{
		CInternetSession netSession;
		auto_ptr<CStdioFile> fTargetFile(netSession.OpenURL(target.c_str(),1,INTERNET_FLAG_TRANSFER_ASCII|INTERNET_FLAG_RELOAD));

		if(fTargetFile.get())
		{
			UINT readLen;
			char buf[512];

			while (readLen=fTargetFile->Read(buf,512))
				ret.append(buf,readLen);
		}
	}
	catch(CInternetException*)
	{
	}
	return encodeUNI(ret);
}
