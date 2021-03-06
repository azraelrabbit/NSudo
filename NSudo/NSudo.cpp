//NSudo 3.2
//NSudo Team. All rights reserved.

#include "stdafx.h"
#include "NSudo.h"

bool bGUIMode = false;

wchar_t szAppPath[260];
wchar_t szShortCutListPath[260];

#include "..\\NSudoAPI\\NSudoLib.h"

INT_PTR CALLBACK NSudoDlgCallBack(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void NSudoBrowseDialog(HWND hWnd, wchar_t* szPath)
{
	OPENFILENAME ofn = { 0 };

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrFile = szPath;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;

	GetOpenFileNameW(&ofn);
}

#include "MuitiLanguage.h"

LanguageResource TextRes = {
	L"默认",

	L"TrustedInstaller",
	L"System",
	L"当前用户",
	L"当前进程",
	L"当前进程(降权)",

	L"启用全部特权",
	L"禁用所有特权",

	L"低",
	L"中",
	L"高",
	L"系统",

	L"NSudo 3.2(Build 1001)\n"
	L"\xA9 2015 NSudo Team. All rights reserved.\n\n"
	L"捐赠支付宝账号: wxh32lkk@live.cn\n"
	L"感谢boyangpangzi,cjy__05,mhxkx,NotePad,tangmigoId,wondersnefu,xy137425740,月光光的大力支持（按照字母排序）\n\n",

	L"获取SE_DEBUG_NAME特权失败(请以管理员权限运行)\n",
	L"请在下拉框中输入命令行或选择快捷命令\n",
	L"进程创建失败\n",
	L"\n命令行参数有误，请修改(使用 - ? 参数查看帮助)\n",

	L"格式: NSudoC [ -U ] [ -P ] [ -M ] 命令行或常用任务名\n"
	L"  -U:[ T | S | C | P | D ] 用户\n"
	L"  	T TrustedInstaller\n"
	L"  	S System\n"
	L"  	C 当前用户\n"
	L"  	P 当前进程\n"
	L"  	D 当前进程(降权)\n\n"
	L"  -P:[ E | D ] 特权\n"
	L"  	E 启用全部特权\n"
	L"  	D 禁用所有特权\n"
	L"  	PS: 如果想以默认特权方式的话，请不要包含-P参数\n\n"
	L"  -M:[ S | H | M | L ] 完整性\n"
	L"  	S 系统\n"
	L"  	H 高\n"
	L"  	M 中\n"
	L"  	L 低\n"
	L"  	PS: 如果想以默认完整性方式的话，请不要包含-M参数\n\n"
	L"  -? 显示该内容\n\n"
	L"  例子：以TrustedInstaller权限，启用所有特权，完整性默认运行命令提示符\n"
	L"		NSudo -U:T -P:E cmd\n"
};



void NSudoReturnMessage(LPCWSTR lpText)
{
	if (bGUIMode)
	{
		MessageBoxW(NULL, lpText, L"NSudo", NULL);
	}
	else
	{
		DWORD result;
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), lpText, wcslen(lpText), &result, NULL);
	}
}

#include "NSudo_Run.h"

int wmain(int argc, _TCHAR* argv[])
{
	//******** NSudoInitialize Start ********
	NSudoReturnMessage(TextRes.NSudo_AboutText);
	
	if (argc == 1) bGUIMode = true;

	SetProcessDPIAware();

	GetModuleFileNameW(NULL, szAppPath, 260);
	wcsrchr(szAppPath, L'\\')[0] = NULL;

	wcscpy_s(szShortCutListPath, 260, szAppPath);
	wcscat_s(szShortCutListPath, 260, L"\\ShortCutList.ini");

	if (!SetCurrentProcessPrivilege(SE_DEBUG_NAME, true))
	{
		if (bGUIMode)
		{
			wchar_t szExePath[260];

			GetModuleFileNameW(NULL, szExePath, 260);
			
			ShellExecuteW(NULL, L"runas", szExePath, NULL, NULL, SW_SHOW);
			return 0;

		}
		else
		{
			NSudoReturnMessage(TextRes.NSudo_Error_Text1);
			return -1;
		}
	}

	//******** NSudoInitialize End ********
	
	if (bGUIMode)
	{
		FreeConsole();

		DialogBoxParamW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDD_NSudoDlg), NULL, NSudoDlgCallBack, 0L);
	}
	else
	{
		bool bUserArgEnable = true;
		bool bPrivilegeArgEnable = true;
		bool bIntegrityArgEnable = true;
		bool bCMDLineArgEnable = true;

		wchar_t *szBuffer = NULL;

		HANDLE hUserToken = INVALID_HANDLE_VALUE;

		for (int i = 1; i < argc; i++)
		{
			if (_wcsicmp(argv[i], L"-?") == 0)
			{
				NSudoReturnMessage(TextRes.NSudoC_HelpText);
				return 0;
			}
			else if (bUserArgEnable && _wcsicmp(argv[i], L"-U:T") == 0)
			{
				NSudoGetTrustedInstallerToken(&hUserToken);
				bUserArgEnable = false;
			}
			else if (bUserArgEnable && _wcsicmp(argv[i], L"-U:S") == 0)
			{
				NSudoGetSystemToken(&hUserToken);
				bUserArgEnable = false;
			}
			else if (bUserArgEnable && _wcsicmp(argv[i], L"-U:C") == 0)
			{
				NSudoGetCurrentUserToken(&hUserToken);
				bUserArgEnable = false;
			}
			else if (bUserArgEnable && _wcsicmp(argv[i], L"-U:P") == 0)
			{
				NSudoGetCurrentProcessToken(&hUserToken);
				bUserArgEnable = false;
			}
			else if (bUserArgEnable && _wcsicmp(argv[i], L"-U:D") == 0)
			{
				NSudoCreateLUAToken(&hUserToken);
				bUserArgEnable = false;
			}
			else if (bPrivilegeArgEnable && _wcsicmp(argv[i], L"-P:E") == 0)
			{
				NSudoAdjustAllTokenPrivileges(hUserToken, true);
				bPrivilegeArgEnable = false;
			}
			else if (bPrivilegeArgEnable && _wcsicmp(argv[i], L"-P:D") == 0)
			{
				NSudoAdjustAllTokenPrivileges(hUserToken, false);
				bPrivilegeArgEnable = false;
			}
			else if (bIntegrityArgEnable && _wcsicmp(argv[i], L"-M:S") == 0)
			{
				SetTokenIntegrity(hUserToken, L"S-1-16-16384");
				bIntegrityArgEnable = false;
			}
			else if (bIntegrityArgEnable && _wcsicmp(argv[i], L"-M:H") == 0)
			{
				SetTokenIntegrity(hUserToken, L"S-1-16-12288");
				bIntegrityArgEnable = false;
			}
			else if (bIntegrityArgEnable && _wcsicmp(argv[i], L"-M:M") == 0)
			{
				SetTokenIntegrity(hUserToken, L"S-1-16-8192");
				bIntegrityArgEnable = false;
			}
			else if (bIntegrityArgEnable && _wcsicmp(argv[i], L"-M:L") == 0)
			{
				SetTokenIntegrity(hUserToken, L"S-1-16-4096");
				bIntegrityArgEnable = false;
			}
			else if (bCMDLineArgEnable)
			{
				wchar_t szPath[260];

				DWORD dwLength = GetPrivateProfileStringW(argv[i], L"CommandLine", L"", szPath, 260, szShortCutListPath);

				wcscmp(szPath, L"") != 0 ? szBuffer = szPath : szBuffer = argv[i];

				if (szBuffer) bCMDLineArgEnable = false;
			}

		}

		if (bUserArgEnable || bCMDLineArgEnable)
		{
			NSudoReturnMessage(TextRes.NSudo_Error_Text4);
			return -1;
		}
		else
		{
			if (NSudoImpersonateSystemToken())
			{
				if (!NSudoCreateProcess(hUserToken, szBuffer))
				{
					NSudoReturnMessage(TextRes.NSudo_Error_Text3);
				}

				RevertToSelf();
			}
		}

		CloseHandle(hUserToken);
	}

	return 0;
}

INT_PTR CALLBACK NSudoDlgCallBack(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hUserName = GetDlgItem(hDlg, IDC_UserName);
	HWND hTokenPrivilege = GetDlgItem(hDlg, IDC_TokenPrivilege);
	HWND hMandatoryLabel = GetDlgItem(hDlg, IDC_MandatoryLabel);
	HWND hszPath = GetDlgItem(hDlg, IDC_szPath);

	wchar_t szCMDLine[260], szUser[260], szPrivilege[260], szMandatory[260], szBuffer[260];

	switch (message)
	{
	case WM_INITDIALOG:
		
		// Show NSudo Logo
		SendMessageW(
			GetDlgItem(hDlg, IDC_NSudoLogo),
			STM_SETIMAGE,
			IMAGE_ICON,
			LPARAM(LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_NSUDO), IMAGE_ICON, 0, 0, LR_COPYFROMRESOURCE)));

		//Show Warning Icon
		SendMessageW(
			GetDlgItem(hDlg, IDC_Icon),
			STM_SETIMAGE,
			IMAGE_ICON,
			LPARAM(LoadIconW(NULL, IDI_WARNING)));

		SendMessageW(hUserName, CB_INSERTSTRING, 0, (LPARAM)TextRes.NSudo_Text_TI);
		SendMessageW(hUserName, CB_INSERTSTRING, 0, (LPARAM)TextRes.NSudo_Text_Sys);
		SendMessageW(hUserName, CB_INSERTSTRING, 0, (LPARAM)TextRes.NSudo_Text_CP);
		SendMessageW(hUserName, CB_INSERTSTRING, 0, (LPARAM)TextRes.NSudo_Text_CU);
		SendMessageW(hUserName, CB_INSERTSTRING, 0, (LPARAM)TextRes.NSudo_Text_CPD);
		SendMessageW(hUserName, CB_SETCURSEL, 4, 0); //设置默认项"TrustedInstaller"
		
		SendMessageW(hTokenPrivilege, CB_INSERTSTRING, 0, (LPARAM)TextRes.NSudo_Text_Default);
		SendMessageW(hTokenPrivilege, CB_INSERTSTRING, 0, (LPARAM)TextRes.NSudo_Text_EnableAll);
		SendMessageW(hTokenPrivilege, CB_INSERTSTRING, 0, (LPARAM)TextRes.NSudo_Text_DisableAll);
		SendMessageW(hTokenPrivilege, CB_SETCURSEL, 2, 0); //设置默认项"默认"

		SendMessageW(hMandatoryLabel, CB_INSERTSTRING, 0, (LPARAM)TextRes.NSudo_Text_Low);
		SendMessageW(hMandatoryLabel, CB_INSERTSTRING, 0, (LPARAM)TextRes.NSudo_Text_Medium);
		SendMessageW(hMandatoryLabel, CB_INSERTSTRING, 0, (LPARAM)TextRes.NSudo_Text_High);
		SendMessageW(hMandatoryLabel, CB_INSERTSTRING, 0, (LPARAM)TextRes.NSudo_Text_System);
		SendMessageW(hMandatoryLabel, CB_INSERTSTRING, 0, (LPARAM)TextRes.NSudo_Text_Default);
		SendMessageW(hMandatoryLabel, CB_SETCURSEL, 0, 0); //设置默认项"默认"

		{
			wchar_t szItem[260], szBuffer[32768];
			DWORD dwLength = GetPrivateProfileSectionNamesW(szBuffer, 32768, szShortCutListPath);

			for (DWORD i = 0, j = 0; i < dwLength; i++,j++)
			{
				if (szBuffer[i] != NULL)
				{
					szItem[j] = szBuffer[i];
				}
				else
				{
					szItem[j] = NULL;
					SendMessageW(hszPath, CB_INSERTSTRING, 0, (LPARAM)szItem);
					j=-1;
				}
			}
		}
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_Run:
			GetDlgItemTextW(hDlg, IDC_UserName, szUser, sizeof(szUser));
			GetDlgItemTextW(hDlg, IDC_TokenPrivilege, szPrivilege, sizeof(szPrivilege));
			GetDlgItemTextW(hDlg, IDC_MandatoryLabel, szMandatory, sizeof(szMandatory));
			GetDlgItemTextW(hDlg, IDC_szPath, szCMDLine, sizeof(szCMDLine));

			NSudo_Run(hDlg,szUser, szPrivilege, szMandatory, szCMDLine);
			break;
		case IDC_About:
			NSudoReturnMessage(TextRes.NSudo_AboutText);
			break;
		case IDC_Browse:
			wcscpy_s(szBuffer, 260, L"");
			NSudoBrowseDialog(hDlg, szBuffer);
			SetDlgItemTextW(hDlg, IDC_szPath, szBuffer);
			break;
		}
		break;
	case WM_SYSCOMMAND:
		switch (LOWORD(wParam))
		{
		case SC_CLOSE:
			PostQuitMessage(0);
			break;
		}
		break;
	}

	return 0;
}