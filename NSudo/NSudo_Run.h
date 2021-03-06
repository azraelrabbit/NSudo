void NSudo_Run(HWND hDlg, LPCWSTR szUser, LPCWSTR szPrivilege, LPCWSTR szMandatory, LPCWSTR szCMDLine)
{
	if (_wcsicmp(L"", szCMDLine) == 0)
	{
		MessageBoxW(hDlg, TextRes.NSudo_Error_Text2, L"NSudo", MB_ICONEXCLAMATION);
	}
	else
	{
		wchar_t szBuffer[512] = { NULL };

		wchar_t szPath[260];
		DWORD dwLength = GetPrivateProfileStringW(szCMDLine, L"CommandLine", L"", szPath, 260, szShortCutListPath);

		if (wcscmp(szPath, L"") != 0)
		{
			wcscat_s(szBuffer, 512, szPath);
		}
		else
		{
			wcscat_s(szBuffer, 512, szCMDLine);
		}

		HANDLE hUserToken = INVALID_HANDLE_VALUE;
		if (_wcsicmp(TextRes.NSudo_Text_CU, szUser) == 0)
		{
			NSudoGetCurrentUserToken(&hUserToken);
		}
		else if (_wcsicmp(TextRes.NSudo_Text_CP, szUser) == 0)
		{
			NSudoGetCurrentProcessToken(&hUserToken);
		}
		else if (_wcsicmp(TextRes.NSudo_Text_CPD, szUser) == 0)
		{
			NSudoCreateLUAToken(&hUserToken);
		}
		else
		{
			if (_wcsicmp(TextRes.NSudo_Text_Sys, szUser) == 0)
			{
				NSudoGetSystemToken(&hUserToken);
			}
			else if (_wcsicmp(TextRes.NSudo_Text_TI, szUser) == 0)
			{
				NSudoGetTrustedInstallerToken(&hUserToken);
			}
		}

		if (_wcsicmp(TextRes.NSudo_Text_EnableAll, szPrivilege) == 0)
		{
			NSudoAdjustAllTokenPrivileges(hUserToken, true);
		}
		else if (_wcsicmp(TextRes.NSudo_Text_DisableAll, szPrivilege) == 0)
		{
			NSudoAdjustAllTokenPrivileges(hUserToken, false);
		}

		if (_wcsicmp(TextRes.NSudo_Text_Low, szMandatory) == 0)
		{
			SetTokenIntegrity(hUserToken, L"S-1-16-4096");
		}
		else if (_wcsicmp(TextRes.NSudo_Text_Medium, szMandatory) == 0)
		{
			SetTokenIntegrity(hUserToken, L"S-1-16-8192");
		}
		else if (_wcsicmp(TextRes.NSudo_Text_High, szMandatory) == 0)
		{
			SetTokenIntegrity(hUserToken, L"S-1-16-12288");
		}

		if (NSudoImpersonateSystemToken())
		{
			if (!NSudoCreateProcess(hUserToken, szBuffer))
			{
				NSudoReturnMessage(TextRes.NSudo_Error_Text3);
			}
			RevertToSelf();
		}

		CloseHandle(hUserToken);
	}
}