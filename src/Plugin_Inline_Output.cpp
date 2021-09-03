#include "stdafx.h"
#include "Plugin.h"

_bstr_t OutputToHtWikiPNG(BYTE* pbBuff, int cbBuff);
_bstr_t OutputToHtWikiSVG(BYTE* pbBuff, int cbBuff);
_bstr_t OutputToHtWikiTXT(BYTE* pbBuff, int cbBuff);

_bstr_t PluginInlineImpl::OutputToHtWiki(int outputDiv, BYTE* pbBuff, int cbBuff)
{
	_bstr_t bszOutput;

	if (cbBuff > 0)
	{
		if (outputDiv == 1)
		{
			bszOutput = OutputToHtWikiSVG(pbBuff, cbBuff);
		}
		else if (outputDiv == 2 || outputDiv == 3)
		{
			bszOutput = OutputToHtWikiTXT(pbBuff, cbBuff);
		}
		else
		{
			bszOutput = OutputToHtWikiPNG(pbBuff, cbBuff);
		}
	}

	return bszOutput;
}

static _bstr_t OutputToHtWikiPNG(BYTE* pbBuff, int cbBuff)
{
	DWORD cchBase64;
	BOOL bRet = CryptBinaryToStringW(pbBuff, cbBuff, CRYPT_STRING_BASE64, NULL, &cchBase64);
	if (!bRet)
	{
		throw CPluginException(L"CryptBinaryToStringに失敗しました。", GetLastError());
	}
	--cchBase64; // ※pszString が NULL である場合の pcchString の結果はNUL文字分のサイズが含まれる

	_bstr_t bszOutput;
	DWORD cchOutput = 35 + cchBase64 + 7;
	bszOutput.Assign(SysAllocStringLen(NULL, cchOutput));
	WCHAR* pwzAvail = static_cast<WCHAR*>(bszOutput);
	DWORD cchAvail = cchOutput + 1; // ※SysAllocStringLenはNUL文字分をプラスで確保している

	wcsncpy_s(pwzAvail, cchAvail, L"<p><img src=\"data:image/png;base64,", 35);
	pwzAvail += 35;
	cchAvail -= 35;

	++cchBase64; // ※バッファサイズにNUL文字分のサイズが含まれていないと pcchString の結果にNUL文字分のサイズが含まれてしまう
	CryptBinaryToStringW(pbBuff, cbBuff, CRYPT_STRING_BASE64, pwzAvail, &cchBase64);
	pwzAvail += cchBase64;
	cchAvail -= cchBase64;

	wcsncpy_s(pwzAvail, cchAvail, L"\"/></p>", 7);
	pwzAvail += 7;
	cchAvail -= 7;

	return bszOutput;
}

static _bstr_t OutputToHtWikiSVG(BYTE* pbBuff, int cbBuff)
{
	int cchXML = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)pbBuff, cbBuff, NULL, 0);
	if (cchXML == 0)
	{
		throw CPluginException(L"MultiByteToWideCharに失敗しました。", GetLastError());
	}

	_bstr_t bszXML;
	bszXML.Assign(SysAllocStringLen(NULL, cchXML));
	MultiByteToWideChar(CP_UTF8, 0, (LPCCH)pbBuff, cbBuff, static_cast<WCHAR*>(bszXML), cchXML);
	static_cast<WCHAR*>(bszXML)[cchXML] = L'\0';

	MSXML2::IXMLDOMDocumentPtr pXmlDocument(__uuidof(MSXML2::DOMDocument60));
	VARIANT_BOOL vBool = pXmlDocument->loadXML(bszXML);
	if (vBool == VARIANT_TRUE)
	{
		MSXML2::IXMLDOMNodePtr pRoot = pXmlDocument->documentElement;
		return pRoot->xml;
	}
	else
	{
		return L"失敗";
	}
}

static _bstr_t OutputToHtWikiTXT(BYTE* pbBuff, int cbBuff)
{
	int cchUnicode = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)pbBuff, cbBuff, NULL, 0);
	if (cchUnicode == 0)
	{
		throw CPluginException(L"MultiByteToWideCharに失敗しました。", GetLastError());
	}

	_bstr_t bszOutput;
	DWORD cchOutput = 5 + cchUnicode + 6;
	bszOutput.Assign(SysAllocStringLen(NULL, cchOutput));
	WCHAR* pwzAvail = static_cast<WCHAR*>(bszOutput);
	DWORD cchAvail = cchOutput + 1; // ※SysAllocStringLenはNUL文字分をプラスで確保している

	wcsncpy_s(pwzAvail, cchAvail, L"<pre>", 5);
	pwzAvail += 5;
	cchAvail -= 5;

	MultiByteToWideChar(CP_UTF8, 0, (LPCCH)pbBuff, cbBuff, pwzAvail, cchAvail);
	pwzAvail += cchUnicode;
	cchAvail -= cchUnicode;

	wcsncpy_s(pwzAvail, cchAvail, L"</pre>", 6);
	pwzAvail += 6;
	cchAvail -= 6;

	return bszOutput;
}
