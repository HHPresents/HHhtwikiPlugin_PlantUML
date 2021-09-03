#include "stdafx.h"
#include "Plugin.h"

CHAR* PluginInlineImpl::A2UTF8(LPCSTR pszAnsi, int* pcchUTF8)
{
	int cchAnsi = strlen(pszAnsi);

	int cchUnicode = MultiByteToWideChar(CP_THREAD_ACP, 0, pszAnsi, cchAnsi, NULL, 0);
	WCHAR* pwzUnicode = new WCHAR[cchUnicode];
	MultiByteToWideChar(CP_THREAD_ACP, 0, pszAnsi, cchAnsi, pwzUnicode, cchUnicode);

	int cchUTF8 = WideCharToMultiByte(CP_UTF8, 0, pwzUnicode, cchUnicode, NULL, 0, 0, NULL);
	CHAR* pszUTF8 = new CHAR[cchUTF8 + 1];
	WideCharToMultiByte(CP_UTF8, 0, pwzUnicode, cchUnicode, pszUTF8, cchUTF8, 0, NULL);
	delete[] pwzUnicode;
	pszUTF8[cchUTF8] = L'\0';

	*pcchUTF8 = cchUTF8;
	return pszUTF8;
}

int PluginInlineImpl::EncodeHexString(byte* pbBlob, unsigned int cbBlob, LPWSTR pwzHex)
{
	static WCHAR chTBL[] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F' };

	int cchHex = 2 + (cbBlob << 1);

	if (pwzHex != NULL)
	{
		*pwzHex++ = L'~';
		*pwzHex++ = L'h';
		for (unsigned int i = 0; i < cbBlob; ++i)
		{
			byte val = pbBlob[i];
			byte hi = (val >> 4) & 0x0F;
			byte lo = val & 0x0F;
			*pwzHex++ = chTBL[hi];
			*pwzHex++ = chTBL[lo];
		}
		*pwzHex++ = L'\0';
	}

	return cchHex;
}

WCHAR* PluginInlineImpl::ReplaceEnvironmentStrings(WCHAR* pwzJavaHome, WCHAR* pwzGraphvizDot)
{
	int cchJavaHome = 0;
	int cchGraphvizDot = 0;
	bool bJavaHome = false;
	bool bGraphvizDot = false;

	if (pwzJavaHome != NULL)
	{
		cchJavaHome = wcslen(pwzJavaHome);
	}
	if (pwzGraphvizDot != NULL)
	{
		cchGraphvizDot = wcslen(pwzGraphvizDot);
	}

	WCHAR* pwzSrc = GetEnvironmentStringsW();
	
	WCHAR* p = NULL;
	int cchP = 0;

	int cchKeyBuff = 0;

	p = pwzSrc;
	cchP = wcslen(p);
	while (cchP > 0)
	{
		WCHAR* q = wcschr(p, L'=');

		if (q != NULL)
		{
			int cchK = q - p;

			if (cchK > cchKeyBuff)
			{
				cchKeyBuff = cchK;
			}
		}

		p += cchP + 1;
		cchP = wcslen(p);
	}

	cchKeyBuff += 1;
	WCHAR* pwzKeyBuff = new WCHAR[cchKeyBuff];
	int cchEnv = 0;

	p = pwzSrc;
	cchP = wcslen(p);
	while (cchP > 0)
	{
		WCHAR* q = wcschr(p, L'=');

		if (q != NULL)
		{
			WCHAR* r = q + 1;
			int cchK = q - p;
			int cchV = cchP - (r - p);
			wcsncpy_s(pwzKeyBuff, cchKeyBuff, p, cchK);

			if (cchJavaHome > 0 && _wcsicmp(pwzKeyBuff, L"JAVA_HOME") == 0)
			{
				cchEnv += cchK + 1 + cchJavaHome + 1;
				bJavaHome = true;
			}
			else if (cchGraphvizDot > 0 && _wcsicmp(pwzKeyBuff, L"GRAPHVIZ_DOT") == 0)
			{
				cchEnv += cchK + 1 + cchGraphvizDot + 1;
				bGraphvizDot = true;
			}
			else
			{
				cchEnv += cchP + 1;
			}
		}
		else
		{
			cchEnv += cchP + 1;
		}

		p += cchP + 1;
		cchP = wcslen(p);
	}
	if (cchJavaHome > 0 && !bJavaHome)
	{
		cchEnv += 9 + 1 + cchJavaHome + 1;
	}
	if (cchGraphvizDot > 0 && !bGraphvizDot)
	{
		cchEnv += 12 + 1 + cchGraphvizDot + 1;
	}
	cchEnv += 1;

	WCHAR* pwzEnv = new WCHAR[cchEnv];
	WCHAR* pw = pwzEnv;

	int cchAvail = cchEnv;

	p = pwzSrc;
	cchP = wcslen(p);
	while (cchP > 0)
	{
		WCHAR* q = wcschr(p, L'=');

		if (q != NULL)
		{
			int cchK = q - p;
			wcsncpy_s(pwzKeyBuff, cchKeyBuff, p, cchK);

			if (cchJavaHome > 0 && _wcsicmp(pwzKeyBuff, L"JAVA_HOME") == 0)
			{
				memcpy_s(pw, cchAvail * sizeof(WCHAR), p, (cchK + 1) * sizeof(WCHAR));
				cchAvail -= cchK + 1;
				pw += cchK + 1;

				memcpy_s(pw, cchAvail * sizeof(WCHAR), pwzJavaHome, (cchJavaHome + 1) * sizeof(WCHAR));
				cchAvail -= cchJavaHome + 1;
				pw += cchJavaHome + 1;
			}
			else if (cchGraphvizDot > 0 && _wcsicmp(pwzKeyBuff, L"GRAPHVIZ_DOT") == 0)
			{
				memcpy_s(pw, cchAvail * sizeof(WCHAR), p, (cchK + 1) * sizeof(WCHAR));
				cchAvail -= cchK + 1;
				pw += cchK + 1;

				memcpy_s(pw, cchAvail * sizeof(WCHAR), pwzGraphvizDot, (cchGraphvizDot + 1) * sizeof(WCHAR));
				cchAvail -= cchGraphvizDot + 1;
				pw += cchGraphvizDot + 1;
			}
			else
			{
				memcpy_s(pw, cchAvail * sizeof(WCHAR), p, (cchP + 1) * sizeof(WCHAR));
				cchAvail -= cchP + 1;
				pw += cchP + 1;
			}
		}
		else
		{
			memcpy_s(pw, cchAvail * sizeof(WCHAR), p, (cchP + 1) * sizeof(WCHAR));
			cchAvail -= cchP + 1;
			pw += cchP + 1;
		}

		p += cchP + 1;
		cchP = wcslen(p);
	}
	if (cchJavaHome > 0 && !bJavaHome)
	{
		memcpy_s(pw, cchAvail * sizeof(WCHAR), L"JAVA_HOME=", 10 * sizeof(WCHAR));
		cchAvail -= 10;
		pw += 10;

		memcpy_s(pw, cchAvail * sizeof(WCHAR), pwzJavaHome, (cchJavaHome + 1) * sizeof(WCHAR));
		cchAvail -= cchJavaHome + 1;
		pw += cchJavaHome + 1;
	}
	if (cchGraphvizDot > 0 && !bGraphvizDot)
	{
		memcpy_s(pw, cchAvail * sizeof(WCHAR), L"GRAPHVIZ_DOT=", 13 * sizeof(WCHAR));
		cchAvail -= 13;
		pw += 13;

		memcpy_s(pw, cchAvail * sizeof(WCHAR), pwzGraphvizDot, (cchGraphvizDot + 1) * sizeof(WCHAR));
		cchAvail -= cchGraphvizDot + 1;
		pw += cchGraphvizDot + 1;
	}

	*pw = L'\0';
	--cchAvail;
	++pw;

	delete[] pwzKeyBuff;
	FreeEnvironmentStringsW(pwzSrc);

	return pwzEnv;
}

BYTE* PluginInlineImpl::SendRequestToPlantUmlHttpServer(LPWSTR pwzServerName, INTERNET_PORT nServerPort, _bstr_t bszObject, int* pcbBuff)
{
	CSafeHINTERNET hSession = WinHttpOpen(L"htwiki/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (hSession == NULL)
	{
		throw CPluginException(L"WinHttpOpen‚ÉŽ¸”s‚µ‚Ü‚µ‚½B", GetLastError(), L"Winhttp.dll");
	}

	CSafeHINTERNET hConnect = WinHttpConnect(hSession, pwzServerName, nServerPort, 0);
	if (hConnect == NULL)
	{
		throw CPluginException(L"WinHttpConnect‚ÉŽ¸”s‚µ‚Ü‚µ‚½B", GetLastError(), L"Winhttp.dll");
	}

	LPCWSTR wszAcceptTypes[] = { L"application/octet-stream", NULL };
	CSafeHINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", bszObject, NULL, WINHTTP_NO_REFERER, wszAcceptTypes, 0);
	if (hRequest == NULL)
	{
		throw CPluginException(L"WinHttpOpenRequest‚ÉŽ¸”s‚µ‚Ü‚µ‚½B", GetLastError(), L"Winhttp.dll");
	}

	if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, WINHTTP_IGNORE_REQUEST_TOTAL_LENGTH, 0))
	{
		throw CPluginException(L"WinHttpSendRequest‚ÉŽ¸”s‚µ‚Ü‚µ‚½B", GetLastError(), L"Winhttp.dll");
	}

	if (!WinHttpReceiveResponse(hRequest, NULL))
	{
		throw CPluginException(L"WinHttpReceiveResponse‚ÉŽ¸”s‚µ‚Ü‚µ‚½B", GetLastError(), L"Winhttp.dll");
	}

	int cvBuff = 4096;
	int cbBuff = 0;
	int cbAvail = cvBuff;
	void* pvBuff = malloc(cvBuff);
	BYTE* pbAvail = (BYTE*)pvBuff;

	DWORD dwRead = 0;
	do
	{
		if (!WinHttpReadData(hRequest, pbAvail, cbAvail, &dwRead))
		{
			free(pvBuff);
			throw CPluginException(L"WinHttpReadData‚ÉŽ¸”s‚µ‚Ü‚µ‚½B", GetLastError(), L"Winhttp.dll");
		}
		if (dwRead > 0)
		{
			cbBuff += dwRead;
			pbAvail += dwRead;
			cbAvail -= dwRead;
			if (cbAvail == 0)
			{
				cbAvail = cvBuff;
				cvBuff = cvBuff << 1;
				pvBuff = realloc(pvBuff, cvBuff);
				pbAvail = ((BYTE*)pvBuff) + cbAvail;
			}
		}
	} while (dwRead > 0);

	*pcbBuff = cbBuff;

	return (BYTE*)pvBuff;
}
