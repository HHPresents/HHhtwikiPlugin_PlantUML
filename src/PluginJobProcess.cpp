#include "stdafx.h"
#include "Plugin.h"

HANDLE CPluginJobProcess::hJob_ = NULL;

HANDLE CPluginJobProcess::GetHandle()
{
	if (CPluginJobProcess::hJob_ == NULL)
	{
		HANDLE hJob = CreateJobObjectW(NULL, L"HHhtwikiPlugin_PlantUML");

		if (hJob == NULL)
		{
			throw CPluginException(L"CreateJobObjectに失敗しました。", GetLastError());
		}

		JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
		jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
		BOOL bRet = SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));
		if (!bRet)
		{
			CloseHandle(hJob);
			throw CPluginException(L"SetInformationJobObjectに失敗しました。", GetLastError());
		}

		CPluginJobProcess::hJob_ = hJob;
	}
	
	return CPluginJobProcess::hJob_;
}

void CPluginJobProcess::Release() throw()
{
	if (CPluginJobProcess::hJob_ != NULL)
	{
		CloseHandle(CPluginJobProcess::hJob_);
		CPluginJobProcess::hJob_ = NULL;
	}
}

bool CPluginJobProcess::IsExistsProcess()
{
	// htwiki自身とPicoWeb用の２つ分を要求する
	int cbJBPIL = sizeof(JOBOBJECT_BASIC_PROCESS_ID_LIST) + sizeof(ULONG_PTR);
	JOBOBJECT_BASIC_PROCESS_ID_LIST* pJBPIL;
	pJBPIL = (JOBOBJECT_BASIC_PROCESS_ID_LIST*)malloc(cbJBPIL);
	pJBPIL->NumberOfAssignedProcesses = 2;

	if (!QueryInformationJobObject(GetHandle(), JobObjectBasicProcessIdList, pJBPIL, cbJBPIL, NULL))
	{
		free(pJBPIL);
		throw CPluginException(L"QueryInformationJobObjectに失敗しました", GetLastError());
	}

	if (pJBPIL->NumberOfProcessIdsInList < 2)
	{
		// PicoWebが異常終了しているならカウント数が減っているハズ
		free(pJBPIL);
		return false;
	}
	else
	{
		// 正常に動作しているならカウント数は２のハズ
		free(pJBPIL);
		return true;
	}
}

void CPluginJobProcess::AssignProcess(HANDLE hProcess)
{
	AssignProcessToJobObject(GetHandle(), hProcess);
}
