#include "stdafx.h"
#include "Plugin.h"

/// プラグインタイプ
///
/// @return 常に1を返す ※「常に1を返すようにして下さい」とのこと
STDAPI_(long) GetPluginType(void)
{
	return 1;
}

/// プラグイン名(GetNameでエクスポート)
///
/// @return プラグインの名前(テキスト)
///
/// ここで設定した文字列がプラグイン名として登録されます
/// ファイル名と同じ名称にすることを推奨します
STDAPI_(BSTR) GetPluginName(void)
{
	return SysAllocString(L"PlantUML");
}

/// プラグインバージョン
/// @return バージョン文字列(テキスト)
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

/// 作者(GetAuthorでエクスポート)
/// @return 作者(テキスト)
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

/// 記法(GetSyntaxでエクスポート)
/// @return 記法(テキスト)
STDAPI_(BSTR) GetPluginSyntax(void)
{
	return SysAllocString(L"#PlantUML(){{ @startuml ... @enduml }} ※改行文字を省略してます。");
}

/// 解説(GetDescriptionでエクスポート)
/// @return 解説(テキスト)
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

/// 実例(GetExampleでエクスポート)
/// @return 実例(テキスト)
STDAPI_(BSTR) GetPluginExample(void)
{
	return SysAllocString(L"#PlantUML(){{ @startum Bob -> Alice : hello @enduml }} ※改行文字を省略してます。");
}

/// 主処理部分
///
/// @param[in] szEscapedString 引数をエスケープ処理したもの
/// @param[in] szPlainString 引数
/// @param[in] szDocumentName 文書名
/// @param[in] szFileName ファイル名
/// @param[in] szDocumentFolder 文書フォルダ名
/// @param[in] szTemplateFolder テンプレートフォルダ名
/// @param[in] szSettingsName (現在未使用)
/// @return 結果(HTML)
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
