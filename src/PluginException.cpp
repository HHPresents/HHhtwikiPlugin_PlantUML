#include "stdafx.h"
#include "Plugin.h"

CPluginException::CPluginException(LPCWSTR pwzMessage) throw()
	: m_bszMessage(pwzMessage)
{
}

CPluginException::CPluginException(LPCWSTR pwzMessage, HRESULT innerHR) throw()
{
	_com_error innerException = _com_error(innerHR);
	m_bszMessage += pwzMessage;
	m_bszMessage += L"(";
	m_bszMessage += innerException.ErrorMessage();
	m_bszMessage += L")";
}

CPluginException::CPluginException(LPCWSTR pwzMessage, const _com_error& innerException) throw()
{
	_bstr_t bszDescription = innerException.Description();
	if (bszDescription.length() == 0)
	{
		m_bszMessage += pwzMessage;
		m_bszMessage += L"(";
		m_bszMessage += innerException.ErrorMessage();
		m_bszMessage += L")";
	}
	else
	{
		m_bszMessage += pwzMessage;
		m_bszMessage += L"(";
		m_bszMessage += bszDescription;
		m_bszMessage += L")";
	}
}

CPluginException::CPluginException(LPCWSTR pwzMessage, DWORD dwErrorCode, LPCWSTR pcwzSourceName) throw()
{
	DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

	VOID* pvSource = NULL;
	if (pcwzSourceName != NULL)
	{
		pvSource = GetModuleHandleW(pcwzSourceName);
		if (pvSource != NULL)
		{
			dwFlags |= FORMAT_MESSAGE_FROM_HMODULE;
		}
	}

	WCHAR* pwzMsg;
	FormatMessageW(dwFlags, pvSource, dwErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&pwzMsg, 0, NULL);

	if (pwzMsg != NULL)
	{
		size_t const nLen = wcslen(pwzMsg);
		if (nLen > 1 && pwzMsg[nLen - 1] == '\n')
		{
			pwzMsg[nLen - 1] = '\0';
			if (pwzMsg[nLen - 2] == '\r')
			{
				pwzMsg[nLen - 2] = '\0';
			}
		}
	}
	else
	{
		pwzMsg = (WCHAR*)LocalAlloc(0, 32 * sizeof(WCHAR));
		if (pwzMsg != NULL)
		{
			swprintf_s(pwzMsg, 32, L"Unknown error 0x%0lX", dwErrorCode);
		}
	}

	m_bszMessage += pwzMessage;
	m_bszMessage += L"(";
	m_bszMessage += pwzMsg;
	m_bszMessage += L")";

	LocalFree((HLOCAL)pwzMsg);
}

CPluginException::CPluginException(const CPluginException& that) throw()
	: m_bszMessage(that.m_bszMessage)
{
}

BSTR CPluginException::GetErrorMessage() throw()
{
	return m_bszMessage.copy();
}
