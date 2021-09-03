#include "stdafx.h"
#include "Plugin.h"

_bstr_t PluginInlineImpl::ExecuteWebMode(LPCSTR szPlainStringUTF8, int cchPlainStringUTF8)
{
	int cbBuff;
	BYTE* pbBuff;

	_bstr_t bszHostNameBuff;
	bszHostNameBuff.Assign(SysAllocStringLen(NULL, 256)); // INTERNET_MAX_HOST_NAME_LENGTH(wininet.h)
	_bstr_t bszUrlPathBuff;
	bszUrlPathBuff.Assign(SysAllocStringLen(NULL, 2048)); // INTERNET_MAX_PATH_LENGTH_NAME_LENGTH(wininet.h)

	URL_COMPONENTSW uc = { 0 };
	uc.dwStructSize = sizeof(uc);
	uc.lpszHostName = static_cast<WCHAR*>(bszHostNameBuff);
	uc.dwHostNameLength = bszHostNameBuff.length();
	uc.lpszUrlPath = static_cast<WCHAR*>(bszUrlPathBuff);
	uc.dwUrlPathLength = bszUrlPathBuff.length();

	_bstr_t bszWebUrl = CPluginConfig::GetWebUrl();
	if (!WinHttpCrackUrl(bszWebUrl, bszWebUrl.length(), 0, &uc))
	{
		throw CPluginException(L"WinHttpCrackUrlに失敗しました。", GetLastError(), L"Winhttp.dll");
	}

	_bstr_t bszReqOutput;
	bszReqOutput.Assign(SysAllocString(static_cast<WCHAR*>(bszUrlPathBuff)));
	if (static_cast<WCHAR*>(bszReqOutput)[bszReqOutput.length() - 1] != L'/')
	{
		bszReqOutput += L"/";
	}

	int outputDiv = CPluginConfig::GetOutput();
	if (outputDiv == 1)
	{
		bszReqOutput += L"svg/";
	}
	else if (outputDiv == 2)
	{
		bszReqOutput += L"txt/";
	}
	else if (outputDiv == 3)
	{
		bszReqOutput += L"utxt/"; // あったらいいなぁ・・・
	}
	else
	{
		outputDiv = 0;
		bszReqOutput += L"png/";
	}

	int cchEncode = PluginInlineImpl::EncodeHexString((BYTE*)szPlainStringUTF8, cchPlainStringUTF8, NULL);
	int cchObject = bszReqOutput.length() + cchEncode;

	_bstr_t bszObject;
	bszObject.Assign(SysAllocStringLen(NULL, cchObject));
	wcsncpy_s(bszObject, cchObject + 1, bszReqOutput, bszReqOutput.length());
	PluginInlineImpl::EncodeHexString((BYTE*)szPlainStringUTF8, cchPlainStringUTF8, static_cast<WCHAR*>(bszObject) + bszReqOutput.length());

	pbBuff = PluginInlineImpl::SendRequestToPlantUmlHttpServer(uc.lpszHostName, uc.nPort, bszObject, &cbBuff);

	_bstr_t bszOutput = PluginInlineImpl::OutputToHtWiki(outputDiv, pbBuff, cbBuff);
	free(pbBuff);
	return bszOutput;
}
