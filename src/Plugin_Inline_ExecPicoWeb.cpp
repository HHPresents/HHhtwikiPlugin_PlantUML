#include "stdafx.h"
#include "Plugin.h"

static void ExecutePicoWebProcess();

_bstr_t PluginInlineImpl::ExecutePicoWebMode(LPCSTR szPlainStringUTF8, int cchPlainStringUTF8)
{
	if (!CPluginJobProcess::IsExistsProcess())
	{
		ExecutePicoWebProcess();
	}

	int outputDiv;
	int cbBuff;
	BYTE* pbBuff;
	{
		_bstr_t bszReqOutput;
		outputDiv = CPluginConfig::GetOutput();
		if (outputDiv == 1)
		{
			bszReqOutput = L"/plantuml/svg/";
		}
		else if (outputDiv == 2)
		{
			bszReqOutput = L"/plantuml/txt/"; // あったらいいなぁ・・・
		}
		else if (outputDiv == 3)
		{
			bszReqOutput = L"/plantuml/utxt/"; // あったらいいなぁ・・・
		}
		else
		{
			outputDiv = 0;
			bszReqOutput = L"/plantuml/png/";
		}

		int cchEncode = PluginInlineImpl::EncodeHexString((BYTE*)szPlainStringUTF8, cchPlainStringUTF8, NULL);
		int cchObject = bszReqOutput.length() + cchEncode;

		_bstr_t bszObject;
		bszObject.Assign(SysAllocStringLen(NULL, cchObject));
		wcsncpy_s(bszObject, cchObject + 1, bszReqOutput, bszReqOutput.length());
		PluginInlineImpl::EncodeHexString((BYTE*)szPlainStringUTF8, cchPlainStringUTF8, static_cast<WCHAR*>(bszObject) + bszReqOutput.length());

		pbBuff = PluginInlineImpl::SendRequestToPlantUmlHttpServer(L"127.0.0.1", CPluginConfig::GetPicoWebPortNo(), bszObject, &cbBuff);
	}

	_bstr_t bszOutput = PluginInlineImpl::OutputToHtWiki(outputDiv, pbBuff, cbBuff);
	free(pbBuff);
	return bszOutput;
}

static void ExecutePicoWebProcess()
{
	BOOL bRet;

	_bstr_t bszPlantUmlPath = CPluginConfig::GetPlantUmlPath();
	_bstr_t bszJavaHome = CPluginConfig::GetJavaHomePath();
	_bstr_t bszJavaPath = L"";

	WCHAR* pwzAppName = NULL;
	if (bszJavaHome.length() > 0)
	{
		WCHAR* pwzJavaPath = new WCHAR[MAX_PATH];
		PathCombineW(pwzJavaPath, bszJavaHome, L"bin");
		PathAppendW(pwzJavaPath, L"java.exe");
		bszJavaPath = pwzJavaPath;
		pwzAppName = static_cast<WCHAR*>(bszJavaPath);
		delete[] pwzJavaPath;
	}

	_bstr_t bszPicoWebArgs;
	{
		WCHAR* pwzPortNo = new WCHAR[6];
		_itow_s(CPluginConfig::GetPicoWebPortNo(), pwzPortNo, 6, 10);
		int cchPortNo = wcslen(pwzPortNo);

		int cch = 10 + cchPortNo;
		bszPicoWebArgs.Assign(SysAllocStringLen(NULL, cch));
		WCHAR* pwzAvail = static_cast<WCHAR*>(bszPicoWebArgs);
		int cchAvail = cch + 1; // ※SysAllocStringLenはNUL文字分をプラスで確保している

		wcsncpy_s(pwzAvail, cchAvail, L" -picoweb:", 10);
		pwzAvail += 10;
		cchAvail -= 10;

		wcsncpy_s(pwzAvail, cchAvail, pwzPortNo, cchPortNo);
		pwzAvail += cchPortNo;
		cchAvail -= cchPortNo;
		delete[] pwzPortNo;
	}

	_bstr_t bszCommandLine;
	{
		UINT cch = 37 + bszPlantUmlPath.length() + 16 + bszPicoWebArgs.length();
		bszCommandLine.Assign(SysAllocStringLen(NULL, cch));
		WCHAR* pwzAvail = static_cast<WCHAR*>(bszCommandLine);
		int cchAvail = cch + 1; // ※SysAllocStringLenはNUL文字分をプラスで確保している

		wcsncpy_s(pwzAvail, cchAvail, L"java.exe -Dfile.encoding=UTF-8 -jar \"", 37);
		pwzAvail += 37;
		cchAvail -= 37;

		wcsncpy_s(pwzAvail, cchAvail, bszPlantUmlPath, bszPlantUmlPath.length());
		pwzAvail += bszPlantUmlPath.length();
		cchAvail -= bszPlantUmlPath.length();

		wcsncpy_s(pwzAvail, cchAvail, L"\" -charset UTF-8", 16);
		pwzAvail += 16;
		cchAvail -= 16;

		wcsncpy_s(pwzAvail, cchAvail, bszPicoWebArgs, bszPicoWebArgs.length());
		pwzAvail += bszPicoWebArgs.length();
		cchAvail -= bszPicoWebArgs.length();
	}

	WCHAR* pwzEnvString = PluginInlineImpl::ReplaceEnvironmentStrings(bszJavaHome, CPluginConfig::GetGraphvizDotPath());

	STARTUPINFOW si = { 0 };
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = { 0 };
	bRet = CreateProcessW(pwzAppName, bszCommandLine, NULL, NULL, TRUE, CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, pwzEnvString, NULL, &si, &pi);
	DWORD dwErrorCode = GetLastError();
	delete[] pwzEnvString;
	if (!bRet)
	{
		throw CPluginException(L"PlantUML(PicoWeb)の起動に失敗しました。", dwErrorCode);
	}

	CPluginJobProcess::AssignProcess(pi.hProcess);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
