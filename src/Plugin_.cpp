#include "stdafx.h"
#include "Plugin.h"

/// �v���O�C���^�C�v
///
/// @return ���1��Ԃ� ���u���1��Ԃ��悤�ɂ��ĉ������v�Ƃ̂���
STDAPI_(long) GetPluginType(void)
{
	return 1;
}

/// �v���O�C����(GetName�ŃG�N�X�|�[�g)
///
/// @return �v���O�C���̖��O(�e�L�X�g)
///
/// �����Őݒ肵�������񂪃v���O�C�����Ƃ��ēo�^����܂�
/// �t�@�C�����Ɠ������̂ɂ��邱�Ƃ𐄏����܂�
STDAPI_(BSTR) GetPluginName(void)
{
	return SysAllocString(L"PlantUML");
}

/// �v���O�C���o�[�W����
/// @return �o�[�W����������(�e�L�X�g)
STDAPI_(BSTR) GetPluginVersion(void)
{
	try
	{
		_bstr_t bszOutput = CPluginVersion::GetVersion();
		return bszOutput.Detach();
	}
	catch (...)
	{
		return SysAllocString(L"NaN");
	}
}

/// ���(GetAuthor�ŃG�N�X�|�[�g)
/// @return ���(�e�L�X�g)
STDAPI_(BSTR) GetPluginAuthor(void)
{
	try
	{
		_bstr_t bszOutput = CPluginVersion::GetCompanyName();
		return bszOutput.Detach();
	}
	catch (...)
	{
		return SysAllocString(L"NaN");
	}
}

/// �L�@(GetSyntax�ŃG�N�X�|�[�g)
/// @return �L�@(�e�L�X�g)
STDAPI_(BSTR) GetPluginSyntax(void)
{
	return SysAllocString(L"#PlantUML(){{ @startuml ... @enduml }} �����s�������ȗ����Ă܂��B");
}

/// ���(GetDescription�ŃG�N�X�|�[�g)
/// @return ���(�e�L�X�g)
STDAPI_(BSTR) GetPluginDescription(void)
{
	try
	{
		_bstr_t bszOutput = CPluginVersion::GetDescription();
		return bszOutput.Detach();
	}
	catch (CPluginException e)
	{
		return e.GetErrorMessage();
	}
	catch (...)
	{
		return SysAllocString(L"NaN");
	}
}

/// ����(GetExample�ŃG�N�X�|�[�g)
/// @return ����(�e�L�X�g)
STDAPI_(BSTR) GetPluginExample(void)
{
	return SysAllocString(L"#PlantUML(){{ @startum Bob -> Alice : hello @enduml }} �����s�������ȗ����Ă܂��B");
}

/// �又������
///
/// @param[in] szEscapedString �������G�X�P�[�v������������
/// @param[in] szPlainString ����
/// @param[in] szDocumentName ������
/// @param[in] szFileName �t�@�C����
/// @param[in] szDocumentFolder �����t�H���_��
/// @param[in] szTemplateFolder �e���v���[�g�t�H���_��
/// @param[in] szSettingsName (���ݖ��g�p)
/// @return ����(HTML)
STDAPI_(BSTR) PluginInline(LPCSTR szEscapedString, LPCSTR szPlainString, LPCSTR szDocumentName, LPCSTR szFileName, LPCSTR szDocumentFolder, LPCSTR szTemplateFolder, LPCSTR szSettingsName)
{
	try
	{
		int cchUTF8;
		CHAR* pszUTF8 = PluginInlineImpl::A2UTF8(szPlainString, &cchUTF8);
		try
		{
			_bstr_t bszOutput;

			int modeDiv = CPluginConfig::GetMode();
			switch (modeDiv)
			{
			case 1:
				bszOutput = PluginInlineImpl::ExecutePicoWebMode(pszUTF8, cchUTF8);
				break;
			case 2:
				bszOutput = PluginInlineImpl::ExecutePipeMode(pszUTF8, cchUTF8);
				break;
			default:
				bszOutput = PluginInlineImpl::ExecuteWebMode(pszUTF8, cchUTF8);
				break;
			}

			delete[] pszUTF8;
			return bszOutput.Detach();
		}
		catch (...)
		{
			delete[] pszUTF8;
			throw;
		}
	}
	catch (CPluginException e)
	{
		return e.GetErrorMessage();
	}
	catch (...)
	{
		return SysAllocString(L"unknown error.");
	}
}
