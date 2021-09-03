#include "stdafx.h"
#include "Plugin.h"

CPluginConfig* CPluginConfig::pConfig_ = NULL;

CPluginConfig& CPluginConfig::GetInstance() throw()
{
	if (pConfig_ == NULL)
	{
		pConfig_ = new CPluginConfig();
	}
	return *pConfig_;
}

void CPluginConfig::Release() throw()
{
	if (pConfig_ != NULL)
	{
		delete pConfig_;
		pConfig_ = NULL;
	}
}

bool CPluginConfig::Load()
{
	if (m_pXmlRoot_ != NULL)
	{
		return true;
	}
	else
	{
		MSXML2::IXMLDOMDocumentPtr pXmlDocument(__uuidof(MSXML2::DOMDocument60));

		_bstr_t bszConfigPath = GetPluginPath(L"HHhtwikiPlugin_PlantUML.config");

		VARIANT_BOOL vBool
			= pXmlDocument->load(static_cast<_variant_t>(bszConfigPath));

		if (vBool == VARIANT_TRUE)
		{
			m_pXmlRoot_ = pXmlDocument->documentElement;
		}

		return (m_pXmlRoot_ != NULL);
	}
}

_bstr_t CPluginConfig::GetValue(LPCWSTR pwzXPath)
{
	if (this->Load())
	{
		MSXML2::IXMLDOMNodePtr pXmlValue
			= m_pXmlRoot_->selectSingleNode(pwzXPath);

		if (pXmlValue == NULL)
		{
			return static_cast<_bstr_t>(L"");
		}
		else
		{
			return pXmlValue->text;
		}
	}
	else
	{
		return static_cast<_bstr_t>(L"");
	}
}

const int CPluginConfig::GetMode()
{
	_bstr_t bszVal = GetInstance().GetValue(L"/configuration/PluginSettings/Mode/text()");

	if (_wcsicmp(bszVal, L"picoweb") == 0)
	{
		// picoweb
		return 1;
	}
	else if (_wcsicmp(bszVal, L"pipe") == 0)
	{
		// pipe
		return 2;
	}
	else
	{
		// web
		return 0;
	}
}

const int CPluginConfig::GetOutput()
{
	_bstr_t bszVal = GetInstance().GetValue(L"/configuration/PluginSettings/Output/text()");

	if (_wcsicmp(bszVal, L"utxt") == 0)
	{
		// utxt
		return 3;
	}
	if (_wcsicmp(bszVal, L"txt") == 0)
	{
		// txt
		return 2;
	}
	if (_wcsicmp(bszVal, L"svg") == 0)
	{
		// svg
		return 1;
	}
	else
	{
		// png
		return 0;
	}
}

const _bstr_t CPluginConfig::GetWebUrl()
{
	_bstr_t bszVal = GetInstance().GetValue(L"/configuration/WebSettings/Url/text()");

	if (bszVal.length() == 0)
	{
		return static_cast<_bstr_t>(L"http://www.plantuml.com/plantuml/");
	}
	else
	{
		return bszVal;
	}
}

const int CPluginConfig::GetPicoWebPortNo()
{
	_bstr_t bszVal = GetInstance().GetValue(L"/configuration/PicoWebSettings/Port/text()");
	int iVal = _wtoi(bszVal);

	if (iVal == 0)
	{
		return 8080;
	}
	else
	{
		return iVal & 0xFFFF;
	}
}

const _bstr_t CPluginConfig::GetPlantUmlPath()
{
	_bstr_t bszVal = GetPluginPath(L"plantuml.jar");
	DWORD dwAttr = GetFileAttributesW(bszVal);

	if (dwAttr == -1 || (dwAttr & FILE_ATTRIBUTE_DIRECTORY) != 0)
	{
		throw CPluginException(L"plantuml.jar ‚ªŒ©‚Â‚©‚è‚Ü‚¹‚ñB");
	}

	return bszVal;
}

const _bstr_t CPluginConfig::GetJavaHomePath()
{
	_bstr_t bszVal = GetInstance().GetValue(L"/configuration/JavaSettings/HomePath/text()");

	if (bszVal.length() == 0)
	{
		WCHAR* pwzBuff = new WCHAR[MAX_PATH];
		DWORD cchBuff = GetEnvironmentVariableW(L"JAVA_HOME", pwzBuff, MAX_PATH);

		if (cchBuff > 0)
		{
			bszVal = pwzBuff;
		}

		delete[] pwzBuff;
	}

	_bstr_t bszExpandVal;
	DWORD cchExpandVal = ExpandEnvironmentStringsW(bszVal, bszExpandVal, 0);
	bszExpandVal.Assign(SysAllocStringLen(NULL, cchExpandVal));
	ExpandEnvironmentStringsW(bszVal, bszExpandVal, bszExpandVal.length());

	return bszExpandVal;
}

const _bstr_t CPluginConfig::GetGraphvizDotPath()
{
	_bstr_t bszVal = GetInstance().GetValue(L"/configuration/GraphvizSettings/DotPath/text()");

	if (bszVal.length() == 0)
	{
		WCHAR* pwzBuff = new WCHAR[MAX_PATH];
		DWORD cchBuff = GetEnvironmentVariableW(L"GRAPHVIZ_DOT", pwzBuff, MAX_PATH);

		if (cchBuff > 0)
		{
			bszVal = pwzBuff;
		}

		delete[] pwzBuff;
	}

	_bstr_t bszExpandVal;
	DWORD cchExpandVal = ExpandEnvironmentStringsW(bszVal, bszExpandVal, 0);
	bszExpandVal.Assign(SysAllocStringLen(NULL, cchExpandVal));
	ExpandEnvironmentStringsW(bszVal, bszExpandVal, bszExpandVal.length());

	return bszExpandVal;
}
