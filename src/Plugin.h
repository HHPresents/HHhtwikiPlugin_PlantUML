#pragma once

class CPluginConfig
{
private:
	static CPluginConfig* pConfig_;
	static CPluginConfig& GetInstance() throw();
private:
	CPluginConfig() {}
	MSXML2::IXMLDOMNodePtr m_pXmlRoot_;
	bool Load();
	_bstr_t GetValue(LPCWSTR pwzXPath);
public:
	static void Release() throw();
	static const int GetMode();
	static const int GetOutput();
	static const _bstr_t GetWebUrl();
	static const int GetPicoWebPortNo();
	static const _bstr_t GetPlantUmlPath();
	static const _bstr_t GetJavaHomePath();
	static const _bstr_t GetGraphvizDotPath();
};

class CPluginException
{
private:
	_bstr_t m_bszMessage;
public:
	CPluginException(LPCWSTR pwzMessage) throw();
	CPluginException(LPCWSTR pwzMessage, HRESULT innerHR) throw();
	CPluginException(LPCWSTR pwzMessage, const _com_error& innerException) throw();
	CPluginException(LPCWSTR pwzMessage, DWORD dwErrorCode, LPCWSTR pcwzSourceName = NULL) throw();
	CPluginException(const CPluginException& that) throw();
	BSTR GetErrorMessage() throw();
};

class CPluginJobProcess
{
private:
	static HANDLE hJob_;
	static HANDLE GetHandle();
public:
	static void Release() throw();
	static bool IsExistsProcess();
	static void AssignProcess(HANDLE hProcess);
};

class CPluginVersion
{
private:
	static CPluginVersion* pVersion_;
	static CPluginVersion& GetInstance() throw();
private:
	BYTE* m_pbBuff;
	CPluginVersion() throw();
	virtual ~CPluginVersion() throw();
	BOOL QueryValue(LPCWSTR pwzQueryString, void** ppvBuff, UINT* pcvBuff);
public:
	static void Release() throw();
	static _bstr_t GetVersion();
	static _bstr_t GetCompanyName();
	static _bstr_t GetDescription();
};

class CSafeHINTERNET
{
private:
	HINTERNET m_hInternet;
public:
	CSafeHINTERNET(HINTERNET hInternet)
		: m_hInternet(hInternet)
	{
	}
	operator const HINTERNET() const throw()
	{
		return m_hInternet;
	}
	virtual ~CSafeHINTERNET()
	{
		if (m_hInternet != NULL)
		{
			WinHttpCloseHandle(m_hInternet);
			m_hInternet = NULL;
		}
	}
};

HMODULE GetPluginModuleHandle();
_bstr_t GetPluginPath(LPCWSTR pwzFile);

namespace PluginInlineImpl
{
	_bstr_t ExecuteWebMode(LPCSTR szPlainStringUTF8, int cchPlainStringUTF8);
	_bstr_t ExecutePicoWebMode(LPCSTR szPlainStringUTF8, int cchPlainStringUTF8);
	_bstr_t ExecutePipeMode(LPCSTR szPlainStringUTF8, int cchPlainStringUTF8);
	_bstr_t OutputToHtWiki(int outputDiv, BYTE* pbBuff, int cbBuff);

	CHAR* A2UTF8(LPCSTR pszAnsi, int* pcchUTF8);
	int EncodeHexString(byte* pbBlob, unsigned int cbBlob, LPWSTR pwzHex);
	WCHAR* ReplaceEnvironmentStrings(WCHAR* pwzJavaHome, WCHAR* pwzGraphvizDot);
	BYTE* SendRequestToPlantUmlHttpServer(LPWSTR pwzServerName, INTERNET_PORT nServerPort, _bstr_t bszObject, int* pcbBuff);
}
