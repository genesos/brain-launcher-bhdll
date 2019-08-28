#include <stdafx.h>

#include "lib_starcraft.h"
#include "../../lib/Registry/Registry.h"
#include "all.h"

using std::wstring;
using std::wstringstream;

wstring scxGetCurrentServer()
{
	CRegistry regMyReg;
	regMyReg.Open( _T("Software\\Battle.net\\Configuration"), HKEY_CURRENT_USER );
	CRegEntry &reg=regMyReg[_T("Battle.net Gateways")];

	if(reg.IsBinary())
	{
		int len = reg.GetBinaryLength();
		auto_ptr<BYTE> b(new BYTE[len]);
		auto_ptr<wchar_t> bn(new wchar_t[len]);
		reg.GetBinary(b.get(), len);
		REP(i, len - 1)
			bn.get()[i] = (wchar_t)b.get()[i];
		reg.SetMulti(bn.get(), len);
	}

	bool bb=reg.IsMultiString();
	if(bb)
	{
		int len=reg.MultiCount();
		if(len>=2)
		{
			int idx;
			wstring idxStr(reg.MultiGetAt(1));

			wstringstream ss;
			ss<<idxStr.c_str();
			ss>>idx;
			idx--;

			if(len-1>=2+3*idx)
				return reg.MultiGetAt(2+3*idx);
		}
	}
	return _T("");
}
BattleZones scxGetBattleZones()
{
	BattleZones rets;

	CRegistry regMyReg;
	regMyReg.Open( _T("Software\\Battle.net\\Configuration"), HKEY_CURRENT_USER );
	CRegEntry &reg=regMyReg[_T("Battle.net Gateways")];

	if(reg.IsBinary())
	{
		int len = reg.GetBinaryLength();
		auto_ptr<BYTE> b(new BYTE[len]);
		auto_ptr<wchar_t> bn(new wchar_t[len]);
		reg.GetBinary(b.get(), len);
		REP(i, len - 1)
			bn.get()[i] = (wchar_t)b.get()[i];
		reg.SetMulti(bn.get(), len);
	}

	bool bb=reg.IsMultiString();
	if(bb)
	{
		int len=reg.MultiCount();
		REPTOBY(i, 2, len - 1, 3)
		{
			BattleZone ret;

			ret.host = reg.MultiGetAt(i);
			ret.zone = _wtoi(reg.MultiGetAt(i + 1));
			ret.serverName = reg.MultiGetAt(i + 2);

			rets.push_back(ret);
		}
	}
	return rets;
}
void scxSetBattleZones(const BattleZones& zones)
{
	CRegistry regMyReg;
	regMyReg.Open( _T("Software\\Battle.net\\Configuration"), HKEY_CURRENT_USER );
	CRegEntry &reg=regMyReg[_T("Battle.net Gateways")];

	if(reg.IsBinary())
	{
		int len = reg.GetBinaryLength();
		auto_ptr<BYTE> b(new BYTE[len]);
		auto_ptr<wchar_t> bn(new wchar_t[len]);
		reg.GetBinary(b.get(), len);
		REP(i, len - 1)
			bn.get()[i] = (wchar_t)b.get()[i];
		reg.SetMulti(bn.get(), len);
	}

	while(reg.MultiCount() >= 3)
		reg.MultiRemoveAt(2);

	EACH(i, zones)
	{
		reg.MultiSetAt(2 + 3 * i, zones[i].host.c_str());
		wstringstream ss;
		ss << zones[i].zone;
		reg.MultiSetAt(2 + 3 * i + 1, ss.str().c_str());
		reg.MultiSetAt(2 + 3 * i + 2, zones[i].serverName.c_str());
	}
}
int scxGetCurrentBattleZoneIndex()
{
	BattleZone ret;

	CRegistry regMyReg;
	regMyReg.Open( _T("Software\\Battle.net\\Configuration"), HKEY_CURRENT_USER );
	CRegEntry &reg=regMyReg[_T("Battle.net Gateways")];

	if(reg.IsBinary())
	{
		int len = reg.GetBinaryLength();
		auto_ptr<BYTE> b(new BYTE[len]);
		auto_ptr<wchar_t> bn(new wchar_t[len]);
		reg.GetBinary(b.get(), len);
		REP(i, len - 1)
			bn.get()[i] = (wchar_t)b.get()[i];
		reg.SetMulti(bn.get(), len);
	}

	bool bb=reg.IsMultiString();
	if(bb)
	{
		int len=reg.MultiCount();
		int sel = _wtoi(reg.MultiGetAt(1));
		return range(1, sel, len / 3 - 1);
	}
	return 1;
}
void scxSetCurrentBattleZoneIndex(int idx)
{
	BattleZone ret;

	CRegistry regMyReg;
	regMyReg.Open( _T("Software\\Battle.net\\Configuration"), HKEY_CURRENT_USER );
	CRegEntry &reg=regMyReg[_T("Battle.net Gateways")];

	if(reg.IsBinary())
	{
		int len = reg.GetBinaryLength();
		auto_ptr<BYTE> b(new BYTE[len]);
		auto_ptr<wchar_t> bn(new wchar_t[len]);
		reg.GetBinary(b.get(), len);
		REP(i, len - 1)
			bn.get()[i] = (wchar_t)b.get()[i];
		reg.SetMulti(bn.get(), len);
	}

	bool bb=reg.IsMultiString();
	if(bb)
	{
		wstringstream ss;
		ss << idx;
		int len=reg.MultiCount();
		reg.MultiSetAt(1, ss.str().c_str());
	}
}

bool scxAddRegServer( std::wstring host, std::wstring serverName, int timeZone /*= -9 */ )
{
	BattleZones zones = scxGetBattleZones();
	BEGIN(zone, zones)
	{
		if(zone->host == host)
		{
			return false;
		}
	}
	BattleZones newZones;
	BattleZone zone;
	zone.host = host;
	zone.zone = timeZone;
	zone.serverName = serverName;
	newZones.push_back(zone);
	newZones.insert(newZones.end(), zones.begin(), zones.end());
	scxSetBattleZones(newZones);
	return true;
}
wstring w3xGetCurrentServer()
{
	CRegistry regMyReg;
	regMyReg.Open( _T("Software\\Blizzard Entertainment\\Warcraft III"), HKEY_CURRENT_USER );
	CRegEntry &reg=regMyReg[_T("Battle.net Gateways")];

	if(reg.IsBinary())
	{
		int len = reg.GetBinaryLength();
		auto_ptr<BYTE> b(new BYTE[len]);
		auto_ptr<wchar_t> bn(new wchar_t[len]);
		reg.GetBinary(b.get(), len);
		REP(i, len - 1)
			bn.get()[i] = (wchar_t)b.get()[i];
		reg.SetMulti(bn.get(), len);
	}

	bool bb=reg.IsMultiString();
	if(bb)
	{
		int len=reg.MultiCount();
		if(len>=2)
		{
			int idx;
			wstring idxStr(reg.MultiGetAt(1));

			wstringstream ss;
			ss<<idxStr.c_str();
			ss>>idx;
			idx--;

			if(len-1>=2+3*idx)
				return reg.MultiGetAt(2+3*idx);
		}
	}
	return _T("");
}
BattleZones w3xGetBattleZones()
{
	BattleZones rets;

	CRegistry regMyReg;
	regMyReg.Open( _T("Software\\Blizzard Entertainment\\Warcraft III"), HKEY_CURRENT_USER );
	CRegEntry &reg=regMyReg[_T("Battle.net Gateways")];

	if(reg.IsBinary())
	{
		int len = reg.GetBinaryLength();
		auto_ptr<BYTE> b(new BYTE[len]);
		auto_ptr<wchar_t> bn(new wchar_t[len]);
		reg.GetBinary(b.get(), len);
		REP(i, len - 1)
			bn.get()[i] = (wchar_t)b.get()[i];
		reg.SetMulti(bn.get(), len);
	}

	bool bb=reg.IsMultiString();
	if(bb)
	{
		int len=reg.MultiCount();
		REPTOBY(i, 2, len - 1, 3)
		{
			BattleZone ret;

			ret.host = reg.MultiGetAt(i);
			ret.zone = _wtoi(reg.MultiGetAt(i + 1));
			ret.serverName = reg.MultiGetAt(i + 2);

			rets.push_back(ret);
		}
	}
	return rets;
}
void w3xSetBattleZones(const BattleZones& zones)
{
	CRegistry regMyReg;
	regMyReg.Open( _T("Software\\Blizzard Entertainment\\Warcraft III"), HKEY_CURRENT_USER );
	CRegEntry &reg=regMyReg[_T("Battle.net Gateways")];

	if(reg.IsBinary())
	{
		int len = reg.GetBinaryLength();
		auto_ptr<BYTE> b(new BYTE[len]);
		auto_ptr<wchar_t> bn(new wchar_t[len]);
		reg.GetBinary(b.get(), len);
		REP(i, len - 1)
			bn.get()[i] = (wchar_t)b.get()[i];
		reg.SetMulti(bn.get(), len);
	}

	while(reg.MultiCount() >= 3)
		reg.MultiRemoveAt(2);

	EACH(i, zones)
	{
		reg.MultiSetAt(2 + 3 * i, zones[i].host.c_str());
		wstringstream ss;
		ss << zones[i].zone;
		reg.MultiSetAt(2 + 3 * i + 1, ss.str().c_str());
		reg.MultiSetAt(2 + 3 * i + 2, zones[i].serverName.c_str());
	}
}
bool w3xAddRegServer( std::wstring host, std::wstring serverName, int timeZone /*= -9*/ )
{
	BattleZones zones = w3xGetBattleZones();
	BEGIN(zone, zones)
	{
		if(zone->host == host)
		{
			return false;
		}
	}
	BattleZones newZones;
	BattleZone zone;
	zone.host = host;
	zone.zone = timeZone;
	zone.serverName = serverName;
	newZones.push_back(zone);
	newZones.insert(newZones.end(), zones.begin(), zones.end());
	w3xSetBattleZones(newZones);
	return true;
}
