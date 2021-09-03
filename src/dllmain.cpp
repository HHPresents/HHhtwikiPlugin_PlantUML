#include "stdafx.h"
#include "Plugin.h"

static HMODULE hModule__ = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hModule__ = hModule;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		CPluginJobProcess::Release();
		CPluginConfig::Release();
		CPluginVersion::Release();
		break;
	}
	return TRUE;
}

HMODULE GetPluginModuleHandle()
{
	return hModule__;
}

_bstr_t GetPluginPath(LPCWSTR pwzFile)
{
	WCHAR* pwzPath = new WCHAR[MAX_PATH];
	DWORD cchPath = GetModuleFileNameW(hModule__, pwzPath, MAX_PATH);

	if (cchPath == 0)
	{
		throw CPluginException(L"GetPluginPathÇ…é∏îsÇµÇ‹ÇµÇΩÅB", GetLastError());
	}

	PathRemoveFileSpecW(pwzPath);
	PathAppendW(pwzPath, pwzFile);
	_bstr_t bszPath = pwzPath;
	delete[] pwzPath;

	return bszPath;
}
