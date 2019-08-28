#pragma once

#include <string>
#include <sstream>
#include <vector>

typedef struct
{
	std::wstring serverName;
	int zone;
	std::wstring host;
} BattleZone;
typedef std::vector<BattleZone> BattleZones;

void scxSetCurrentBattleZoneIndex(int idx);
int scxGetCurrentBattleZoneIndex();
void scxSetBattleZones(const BattleZones& zones);
BattleZones scxGetBattleZones();
std::wstring scxGetCurrentServer();
bool scxAddRegServer( std::wstring host, std::wstring serverName, int timeZone = -9 );

BattleZones w3xGetBattleZones();
std::wstring w3xGetCurrentServer();
bool w3xAddRegServer( std::wstring host, std::wstring serverName, int timeZone = -9);