//NSudo 3.2
//NSudo Team. All rights reserved.

#pragma once

#pragma comment(lib,"NSudoAPI.lib")

#define NSudoAPI_Import __declspec(dllimport)

extern "C"
{
	//获取一个进程的PID
	NSudoAPI_Import DWORD NSudoGetProcessID(
		LPCWSTR lpProcessName,
		bool bUnderCurrentSessionID
		);

	//根据令牌创建进程（对CreateProcess和CreateEnvironmentBlock的封装，可能需要SE_ASSIGNPRIMARYTOKEN_NAME特权）
	NSudoAPI_Import bool NSudoCreateProcess(
		HANDLE hToken,
		LPCWSTR lpCommandLine
		);

	//一键开启或关闭所有特权
	NSudoAPI_Import void NSudoAdjustAllTokenPrivileges(
		HANDLE TokenHandle,
		bool bEnable
		);

	//模拟当前进程的权限令牌为System权限(如果想取消模拟，可以调用RevertToSelf)
	NSudoAPI_Import bool NSudoImpersonateSystemToken();

	//获取System权限令牌(需要SE_DEBUG_NAME特权)
	NSudoAPI_Import bool NSudoGetSystemToken(
		PHANDLE hNewToken
		);

	//获取TrustedInstaller权限令牌(需要SE_DEBUG_NAME特权)
	NSudoAPI_Import bool NSudoGetTrustedInstallerToken(
		PHANDLE hNewToken
		);

	//设置令牌权限
	NSudoAPI_Import bool SetTokenPrivilege(
		HANDLE TokenHandle,
		LPCWSTR lpName,
		bool bEnable
		);

	//设置当前进程令牌权限
	NSudoAPI_Import bool SetCurrentProcessPrivilege(
		LPCWSTR lpName,
		bool bEnable
		);

	//获取当前用户的令牌(需要SE_DEBUG_NAME特权)
	NSudoAPI_Import bool NSudoGetCurrentUserToken(
		PHANDLE hNewToken
		);

	//设置令牌完整性
	NSudoAPI_Import bool SetTokenIntegrity(
		HANDLE hToken,
		LPCWSTR szIntegritySID
		);

	//获取当前进程的令牌
	NSudoAPI_Import bool NSudoGetCurrentProcessToken(
		PHANDLE hNewToken
		);

	//对当前进程进行降权并获取令牌
	NSudoAPI_Import bool NSudoCreateLUAToken(
		PHANDLE hNewToken
		);
}