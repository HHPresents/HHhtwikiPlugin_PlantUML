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
			throw CPluginException(L"CreateJobObject�Ɏ��s���܂����B", GetLastError());
		}

		JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
		jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
		BOOL bRet = SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));
		if (!bRet)
		{
			CloseHandle(hJob);
			throw CPluginException(L"SetInformationJobObject�Ɏ��s���܂����B", GetLastError());
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
	// htwiki���g��PicoWeb�p�̂Q����v������
	int cbJBPIL = sizeof(JOBOBJECT_BASIC_PROCESS_ID_LIST) + sizeof(ULONG_PTR);
	JOBOBJECT_BASIC_PROCESS_ID_LIST* pJBPIL;
	pJBPIL = (JOBOBJECT_BASIC_PROCESS_ID_LIST*)malloc(cbJBPIL);
	pJBPIL->NumberOfAssignedProcesses = 2;

	if (!QueryInformationJobObject(GetHandle(), JobObjectBasicProcessIdList, pJBPIL, cbJBPIL, NULL))
	{
		free(pJBPIL);
		throw CPluginException(L"QueryInformationJobObject�Ɏ��s���܂���", GetLastError());
	}

	if (pJBPIL->NumberOfProcessIdsInList < 2)
	{
		// PicoWeb���ُ�I�����Ă���Ȃ�J�E���g���������Ă���n�Y
		free(pJBPIL);
		return false;
	}
	else
	{
		// ����ɓ��삵�Ă���Ȃ�J�E���g���͂Q�̃n�Y
		free(pJBPIL);
		return true;
	}
}

void CPluginJobProcess::AssignProcess(HANDLE hProcess)
{
	AssignProcessToJobObject(GetHandle(), hProcess);
}
