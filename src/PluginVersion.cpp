#include "stdafx.h"
#include "Plugin.h"

CPluginVersion* CPluginVersion::pVersion_ = NULL;

CPluginVersion& CPluginVersion::GetInstance() throw()
{
	if (pVersion_ == NULL)
	{
		pVersion_ = new CPluginVersion();
	}
	return *pVersion_;
}

void CPluginVersion::Release() throw()
{
	if (pVersion_ != NULL)
	{
		delete pVersion_;
		pVersion_ = NULL;
	}
}

CPluginVersion::CPluginVersion() throw()
	: m_pbBuff(NULL)
{
}

CPluginVersion::~CPluginVersion() throw()
{
	if (m_pbBuff != NULL)
	{
		delete[] m_pbBuff;
		m_pbBuff = NULL;
	}
}

BOOL CPluginVersion::QueryValue(LPCWSTR pwzQueryString, void** ppvBuff, UINT* pcvBuff)
{
	if (m_pbBuff == NULL)
	{
		DWORD dwHandle;
		DWORD cbBuff = GetFileVersionInfoSizeW(L"HHhtwikiPlugin_PlantUML.dll", &dwHandle);

		BYTE* pbBuff = new BYTE[cbBuff];
		if (!GetFileVersionInfoW(L"HHhtwikiPlugin_PlantUML.dll", dwHandle, cbBuff, pbBuff))
		{
			delete[] pbBuff;
			throw CPluginException(L"GetFileVersionInfoÇ…é∏îsÇµÇ‹ÇµÇΩÅB", GetLastError());
		}

		m_pbBuff = pbBuff;
	}

	return VerQueryValueW(m_pbBuff, pwzQueryString, ppvBuff, pcvBuff);
}

_bstr_t CPluginVersion::GetVersion(void)
{
	WCHAR* pwzVI;
	UINT cchVI;
	if (!CPluginVersion::GetInstance().QueryValue(L"\\StringFileInfo\\041104b0\\ProductVersion", (void**)&pwzVI, &cchVI))
	{
		throw CPluginException(L"VerQueryValue[ProductVersion]Ç…é∏îsÇµÇ‹ÇµÇΩÅB", GetLastError());
	}
	_bstr_t bszVal = pwzVI;
	return bszVal;
}

_bstr_t CPluginVersion::GetCompanyName(void)
{
	WCHAR* pwzVI;
	UINT cchVI;
	if (!CPluginVersion::GetInstance().QueryValue(L"\\StringFileInfo\\041104b0\\CompanyName", (void**)&pwzVI, &cchVI))
	{
		throw CPluginException(L"VerQueryValue[CompanyName]Ç…é∏îsÇµÇ‹ÇµÇΩÅB", GetLastError());
	}
	_bstr_t bszVal = pwzVI;
	return bszVal;
}

_bstr_t CPluginVersion::GetDescription(void)
{
	WCHAR* pwzVI;
	UINT cchVI;
	if (!CPluginVersion::GetInstance().QueryValue(L"\\StringFileInfo\\041104b0\\FileDescription", (void**)&pwzVI, &cchVI))
	{
		throw CPluginException(L"VerQueryValue[FileDescription]Ç…é∏îsÇµÇ‹ÇµÇΩÅB", GetLastError());
	}
	_bstr_t bszVal = pwzVI;
	return bszVal;
}
