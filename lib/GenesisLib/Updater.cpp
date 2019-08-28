#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <afx.h>
#include <sstream>

#include "lib/lib_http/lib_http.h"
#include "lib/CHash/CHash.h"
#include "lib/fileFinder/FileFinder.h"

#include "Updater.h"


Updater::Updater(std::wstring url, std::wstring header, std::wstring binary, std::wstring windowName)
:update(0)
,downloadMax(0)
,downloadDone(0)
,downloadByte(0)
,downloadFile()
,cancel(0)
,url(url)
,header(header)
,binary(binary)
,windowName(windowName)
{

}

void Updater::checkUpdate()
{
	AfxBeginThread((AFX_THREADPROC)__threadCheckUpdate, (LPVOID)this);
}

bool Updater::isCheckingDone()
{
	return update != 0;
}

bool Updater::isCheckedUpdated()
{
	return update == 1;
}
void Updater::doUpdate()
{
	AfxBeginThread((AFX_THREADPROC)__threadDoUpdate, (LPVOID)this);
}

void Updater::cancelUpdating()
{
	cancel = true;
}

UINT __cdecl Updater::__threadCheckUpdate( LPVOID ptr )
{
	Updater *_this = (Updater*)ptr;
	_this->threadCheckUpdate();

	return 0;
}

void Updater::threadCheckUpdate()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
	bool updated = 0;
	try{
		std::wstring versionInfo = trim(getWebDownloadToString(url));//L"http://u.brainclan.com/update_u/"
		std::wstringstream wss;
		std::wstring updateExe, updateMD5;

		wss << versionInfo;
		updateFiles.clear();
		wss >> updateExe;
		if(updateExe == header)//L"GosuLauncher"
			while(!wss.eof())
			{
				wss >> updateExe >> updateMD5;
				LOG(updateExe + L":" + updateMD5);
				if(updateExe.empty() || updateMD5.empty())
					continue;

				updateExe = str_replace(updateExe, L"/", L"\\");

				if(updateExe[0] == L'-')
				{
					{
						std::wstringstream wsslog;
						wsslog << L"File Update Info : " << updateExe << L" (" << updateMD5 << L")";
						LOG(wsslog);
					}

					if(isFile(getModuleAbsPath() + L"\\" + updateExe.substr(1)))
					{
						updated = true;
						updateFiles.push_back(updateExe);
					}
				}
				else
				{
					CHash md5;
					md5.SetHashFile((getModuleAbsPath() + L"\\" + updateExe).c_str());
					std::wstring md5Str(md5.MD5Hash().GetString());

					{
						std::wstringstream wsslog;
						wsslog << L"File Update Info : " << updateExe << L" (" << updateMD5 << L"=>" << md5Str << L")";
						if(updateMD5 != md5Str)
							wsslog << L"[!]";
						LOG(wsslog);
					}

					if(
						strEndedBy(updateExe, L".select.conf.txt")
						|| strEndedBy(updateExe, L".ini")
						)
					{
						std::wstringstream wsslog;
						wsslog << L"already has " << updateExe;
						LOG(wsslog);
						if(isFile(getModuleAbsPath() + L"\\" + updateExe))
							continue;
					}

					if(updateMD5 != L"d41d8cd98f00b204e9800998ecf8427e")
						if(updateMD5.length() > 0 && md5Str != updateMD5)
						{
							std::wstringstream wsslog;
							wsslog << L"Update Need : " << updateExe;
							LOG(wsslog);

							updated = true;
							updateFiles.push_back(updateExe);
						}
				}
			}
	}
	catch(std::exception e)
	{

	}
	std::wstringstream wsslog;
	wsslog << L"Update Check Result : " << updated;
	LOG(wsslog);
	if(updated)
		update = 1;
	else
		update = -1;
}

UINT __cdecl Updater::__threadDoUpdate( LPVOID ptr )
{
	Updater *_this = (Updater*)ptr;
	_this->doingUpdate();

	return 0;
}

void Updater::doUpdateCallback( int byte, bool &shutdown )
{
	shutdown = cancel;
	downloadByte += byte;
}

void Updater::doingUpdate()
{
	try{
		downloadDone = 0;
		downloadMax = updateFiles.size();
		boost::function<void(int, bool&)> callback = boost::bind(&Updater::doUpdateCallback, this, _1, _2);
		EACH(i, updateFiles)
		{
			if(updateFiles[i][0] == L'-')
			{
				std::wstring file = updateFiles[i].substr(1);
				std::wstring fullPath = getModuleAbsPath() + _T("\\") + file;

				_wunlink(fullPath.c_str());
				downloadFile = file;
			}
			else
			{
				downloadFile = updateFiles[i];

				getWebDownloadToFileCurDir(url + updateFiles[i], L"__u__" + str_replace(updateFiles[i], L"\\", L"[]"), callback);
			}
			if(cancel)
				ExitThread(0);
			downloadDone++;
		}

		if(cancel)
			ExitThread(0);
		std::wstring exe = getModuleAbsPath() + L"\\__u__" + binary; //_T("\\__u__GosuLauncher.exe")
		if(!isFile(exe))
			exe = getModuleAbsPath() + L"\\" + getModuleExeName();
		ShellExecute(0, _T("open"), exe.c_str(), (std::wstring(_T("/update ")) + getModuleExeName()).c_str(),0,0);
		ExitProcess(0);
	}
	catch(std::exception e)
	{
	}
}
void Updater::onStart()
{
	if(__argc >= 3)
	{
		if(std::wstring(__wargv[1]) == L"/update")
		{
			DWORD cur = GetTickCount();
			while(GetTickCount() - cur <= 10000)
			{
				if(FindWindow(0, windowName.c_str()))
					PostMessage(FindWindow(0, windowName.c_str()), WM_CLOSE, 0, 0);
				else 
					break;
				Sleep(100);
			}
			if(FindWindow(0, windowName.c_str()))
				SendMessage(FindWindow(0, windowName.c_str()), WM_CLOSE, 0, 0);
			Sleep(1000);

			CFileFinder ff;

			int fcnt = ff.FindFiles(getModuleAbsPath().c_str(), L"__u__*.*");
			REP(i, fcnt - 1)
			{
				std::wstring oldFileName = (const WCHAR*)ff.GetFilePath(i).GetFileName().GetString();
				std::wstring newFileName = ((const WCHAR*)ff.GetFilePath(i).GetFileName().GetString() + 5);

				newFileName = str_replace(newFileName, L"[]", L"\\");

				if(strtolower(newFileName) == binary)
					newFileName = __wargv[2];

				oldFileName = getModuleAbsPath() + _T("\\") + oldFileName;
				newFileName = getModuleAbsPath() + _T("\\") + newFileName;

				try
				{
					_wunlink(newFileName.c_str());

					std::wstring dir;
					{
						std::wstring::size_type dirIndex = newFileName.find_last_of(L'\\');
						if(dirIndex != std::wstring::npos)
							CreateDirectory(newFileName.substr(0, dirIndex).c_str(),0);
					}

					CFile::Rename(oldFileName.c_str(), newFileName.c_str());
				}
				catch (CMemoryException*)
				{
				}
				catch (CFileException*)
				{
					std::wstring arg = varStr(L"Error on delete '$'", newFileName);
					MessageBox(0, arg.c_str(), 0, 0);
				}
				catch (CException*)
				{
				}
			}

			std::wstring exe = getModuleAbsPath() + _T("\\") + __wargv[2];
			ShellExecute(0, _T("open"), exe.c_str(), _T("/updatedone"), 0, 0);
			ExitProcess(0);
		}
	}
}
