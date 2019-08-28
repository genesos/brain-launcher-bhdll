#pragma once

#include <string>
#include <vector>

class Updater
{
	std::wstring url, header, binary, windowName;

	int update;

	std::vector<std::wstring> updateFiles;

	bool cancel;
public:
	int downloadMax;
	int downloadDone;
	int downloadByte;
	std::wstring downloadFile;

	Updater(std::wstring url, std::wstring header, std::wstring binary, std::wstring windowName);
	void checkUpdate();
	bool isCheckingDone();
	bool isCheckedUpdated();
	void doUpdate();
	void cancelUpdating();
	void onStart();
private:
	static UINT __cdecl __threadCheckUpdate(LPVOID ptr);
	void threadCheckUpdate();
	static UINT __cdecl __threadDoUpdate(LPVOID ptr);
	void doUpdateCallback(int byte, bool &shutdown);
	void doingUpdate();
};
