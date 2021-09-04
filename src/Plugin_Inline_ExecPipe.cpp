#include "stdafx.h"
#include "Plugin.h"

_bstr_t PluginInlineImpl::ExecutePipeMode(LPCSTR szPlainStringUTF8, int cchPlainStringUTF8)
{
	BOOL bRet;

	HANDLE hPipeSrcR, hPipeSrcW;
	HANDLE hPipeDstR, hPipeDstW;
	CreatePipe(&hPipeSrcR, &hPipeSrcW, NULL, 0);
	CreatePipe(&hPipeDstR, &hPipeDstW, NULL, 0);

	STARTUPINFOW si = { 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	DuplicateHandle(GetCurrentProcess(), hPipeSrcR, GetCurrentProcess(), &si.hStdInput, 0, TRUE, DUPLICATE_SAME_ACCESS);
	DuplicateHandle(GetCurrentProcess(), hPipeDstW, GetCurrentProcess(), &si.hStdOutput, 0, TRUE, DUPLICATE_SAME_ACCESS);
	CloseHandle(hPipeSrcR);
	CloseHandle(hPipeDstW);

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

	_bstr_t bszReqOutput;
	int outputDiv = CPluginConfig::GetOutput();
	if (outputDiv == 1)
	{
		bszReqOutput = L" -svg";
	}
	else if (outputDiv == 2)
	{
		bszReqOutput = L" -txt";
	}
	else if (outputDiv == 3)
	{
		bszReqOutput = L" -utxt";
	}
	else
	{
		outputDiv = 0;
		bszReqOutput = L" -png";
	}

	_bstr_t bszCommandLine;
	{
		int cch = 37 + bszPlantUmlPath.length() + 22 + bszReqOutput.length();
		bszCommandLine.Assign(SysAllocStringLen(NULL, cch));
		WCHAR* pwzAvail = static_cast<WCHAR*>(bszCommandLine);
		int cchAvail = cch + 1; // ��SysAllocStringLen��NUL���������v���X�Ŋm�ۂ��Ă���

		wcsncpy_s(pwzAvail, cchAvail, L"java.exe -Dfile.encoding=UTF-8 -jar \"", 37);
		pwzAvail += 37;
		cchAvail -= 37;

		wcsncpy_s(pwzAvail, cchAvail, bszPlantUmlPath, bszPlantUmlPath.length());
		pwzAvail += bszPlantUmlPath.length();
		cchAvail -= bszPlantUmlPath.length();

		wcsncpy_s(pwzAvail, cchAvail, L"\" -charset UTF-8 -pipe", 22);
		pwzAvail += 22;
		cchAvail -= 22;

		wcsncpy_s(pwzAvail, cchAvail, bszReqOutput, bszReqOutput.length());
		pwzAvail += bszReqOutput.length();
		cchAvail -= bszReqOutput.length();
	}

	WCHAR* pwzEnvString = PluginInlineImpl::ReplaceEnvironmentStrings(bszJavaHome, CPluginConfig::GetGraphvizDotPath());

	PROCESS_INFORMATION pi = { 0 };
	bRet = CreateProcessW(pwzAppName, bszCommandLine, NULL, NULL, TRUE, CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, pwzEnvString, NULL, &si, &pi);
	DWORD dwErrorCode = GetLastError();
	delete[] pwzEnvString;
	if (!bRet)
	{
		CloseHandle(si.hStdInput);
		CloseHandle(si.hStdOutput);
		CloseHandle(hPipeSrcW);
		CloseHandle(hPipeDstR);
		throw CPluginException(L"PlantUML(Pipe)�̋N���Ɏ��s���܂����B", dwErrorCode);
	}

	DWORD dwWrite;
	bRet = WriteFile(hPipeSrcW, szPlainStringUTF8, cchPlainStringUTF8, &dwWrite, NULL);
	CloseHandle(hPipeSrcW);

	// �v���Z�X�N���ɐ����������_�Ńn���h���̈����n���ɐ������Ă���̂Ŏ��v���Z�X�̃n���h���͕��Ă���
	// �����ɕW���o�͂̃p�C�v�͕��Ă����Ȃ���ReadFile���Ɏ��v���Z�X�̃n���h��������܂ŉi�v�ɑҋ@���Ă��܂�
	CloseHandle(si.hStdInput);
	CloseHandle(si.hStdOutput);

	int cvBuff = 4096;
	int cbBuff = 0;
	int cbAvail = cvBuff;
	void* pvBuff = malloc(cvBuff);
	BYTE* pbAvail = (BYTE*)pvBuff;

	DWORD dwRead;
	do
	{
		if (!ReadFile(hPipeDstR, pbAvail, cbAvail, &dwRead, NULL))
		{
			dwRead = 0;
			DWORD dwErrorCode = GetLastError();
			if (dwErrorCode == ERROR_BROKEN_PIPE)
			{
				// �ǂݐ؂�����v���Z�X���I�����ăp�C�v���j�������̂ŃG���[�Ƃ��Ĉ���Ȃ�
				// �{���͂���ȍl���͕K�v�Ȃ��n�Y�����APlantUML �� System.exit ���g���ăv���Z�X�������I�ɏI�������Ă���悤�Ȃ̂�
				// ���̉e����������Ȃ��E�E�E
			}
			else
			{
				free(pvBuff);
				throw CPluginException(L"ReadFile�Ɏ��s���܂����B", dwErrorCode);
			}
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
	CloseHandle(hPipeDstR);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	_bstr_t bszOutput = PluginInlineImpl::OutputToHtWiki(outputDiv, (BYTE*)pvBuff, cbBuff);
	free(pvBuff);
	return bszOutput;
}
