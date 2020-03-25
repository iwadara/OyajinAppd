//
//	Registry Service
//

//
//	Include files
//
#ifndef		STRICT
#define		STRICT
#endif

#include	<windows.h>
#include	<windowsx.h>
#include	<string.h>
#include	<tchar.h>
#include	<commctrl.h>
#include	"AppRes.h"
#include	"External.h"

//
//	ProtTypes
//
void ErrorMsgDlg(HWND, LPCTSTR);
BOOL SetEnumRegistry(HKEY , LPCTSTR , LPDWORD, DWORD);
BOOL SetStrRegistry(HKEY , LPCTSTR , LPTSTR, LPCTSTR);

//
//	Constants
//
LPCTSTR	szCreateError =	TEXT("Registry create error");
LPCTSTR	szWriteError =	TEXT("Registry write error");
LPCTSTR	szReadError =	TEXT("Registry read error");
LPCTSTR	szCloseError =	TEXT("Registry close error");

//
//	Save Registry Data
//
BOOL SaveRegistry(void)
{
	LONG	ret;
	HKEY	hKey;
	DWORD	dispos;
	DWORD	i;

//	Open Registry Key
	ret = RegCreateKeyEx(HKEY_CURRENT_USER,
			szRegName,
			0, NULL, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hKey,
			&dispos);
	if(ret != ERROR_SUCCESS)
	{
		ErrorMsgDlg(hMainWnd, szCreateError);
		return FALSE;
	}

//	Get Registry Data
	for(i = 0; RegTable[i][0]; i++)
	{
		if(RegTable[i][1])
		{
			ret = RegSetValueEx(hKey, RegTable[i][0], NULL, REG_BINARY,
				(LPBYTE )RegTable[i][2], (DWORD )RegTable[i][1]);
			if(ret != ERROR_SUCCESS)
			{
				ErrorMsgDlg(hMainWnd, szWriteError);
				return FALSE;
			}
		}
		else
		{
			ret = RegSetValueEx(hKey, RegTable[i][0], NULL, REG_SZ,
					(LPBYTE )RegTable[i][2], (_tcslen(RegTable[i][2])+1)*sizeof(TCHAR));
			if(ret != ERROR_SUCCESS)
			{
				ErrorMsgDlg(hMainWnd, szWriteError);
				return FALSE;
			}
		}
	}
	ret = RegCloseKey(hKey);
	if(ret != ERROR_SUCCESS)
	{
		ErrorMsgDlg(hMainWnd, szCloseError);
		return FALSE;
	}
	return TRUE;
}

//
//	Save Registry Data2 [iwad]
//
BOOL SaveRegistry2(void)
{
	LONG	ret;
	HKEY	hKey;
	DWORD	dispos;

//	Open Registry Key(左ソフトキー)
	ret = RegCreateKeyEx(HKEY_CURRENT_USER,
			TEXT("Software\\Microsoft\\Today\\Keys\\112"),
			0, NULL, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hKey,
			&dispos);
	if(ret != ERROR_SUCCESS)
	{
		ErrorMsgDlg(hMainWnd, szCreateError);
		return FALSE;
	}

// [iwad] TodayのソフトウェアキーにOyajin Appdを割り当てる
	// [iwad] szDefNameL (Default)
	if (_tcslen(RegTable2[0][2]) == 0) {
		ret = RegDeleteValue(hKey, RegTable2[0][0]);
	}
	else {
		ret = RegSetValueEx(hKey, RegTable2[0][0], NULL, REG_SZ,
			(LPBYTE )RegTable2[0][2], (_tcslen(RegTable2[0][2])+1)*sizeof(TCHAR));
	}
	if(ret != ERROR_SUCCESS)
	{
		ErrorMsgDlg(hMainWnd, szWriteError);
		return FALSE;
	}
	// [iwad] szOpenNameL (Open)
	if (_tcslen(RegTable2[1][2]) == 0) {
		ret = RegDeleteValue(hKey, RegTable2[1][0]);
	}
	else {
		ret = RegSetValueEx(hKey, RegTable2[1][0], NULL, REG_SZ,
			(LPBYTE )RegTable2[1][2], (_tcslen(RegTable2[1][2])+1)*sizeof(TCHAR));
	}
	if(ret != ERROR_SUCCESS)
	{
		ErrorMsgDlg(hMainWnd, szWriteError);
		return FALSE;
	}

	ret = RegCloseKey(hKey);
	if(ret != ERROR_SUCCESS)
	{
		ErrorMsgDlg(hMainWnd, szCloseError);
		return FALSE;
	}
//	Open Registry Key(右ソフトキー)
	ret = RegCreateKeyEx(HKEY_CURRENT_USER,
			TEXT("Software\\Microsoft\\Today\\Keys\\113"),
			0, NULL, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hKey,
			&dispos);
	if(ret != ERROR_SUCCESS)
	{
		ErrorMsgDlg(hMainWnd, szCreateError);
		return FALSE;
	}

// [iwad] TodayのソフトウェアキーにOyajin Appdを割り当てる
	// [iwad] szDefNameR (Default)
	if (_tcslen(RegTable2[2][2]) == 0) {
		ret = RegDeleteValue(hKey, RegTable2[2][0]);
	}
	else {
		ret = RegSetValueEx(hKey, RegTable2[2][0], NULL, REG_SZ,
			(LPBYTE )RegTable2[2][2], (_tcslen(RegTable2[2][2])+1)*sizeof(TCHAR));
	}
	if(ret != ERROR_SUCCESS)
	{
		ErrorMsgDlg(hMainWnd, szWriteError);
		return FALSE;
	}
	// [iwad] szOpenNameR (Open)
	if (_tcslen(RegTable2[3][2]) == 0) {
		ret = RegDeleteValue(hKey, RegTable2[3][0]);
	}
	else {
		ret = RegSetValueEx(hKey, RegTable2[3][0], NULL, REG_SZ,
			(LPBYTE )RegTable2[3][2], (_tcslen(RegTable2[3][2])+1)*sizeof(TCHAR));
	}
	if(ret != ERROR_SUCCESS)
	{
		ErrorMsgDlg(hMainWnd, szWriteError);
		return FALSE;
	}

	ret = RegCloseKey(hKey);
	if(ret != ERROR_SUCCESS)
	{
		ErrorMsgDlg(hMainWnd, szCloseError);
		return FALSE;
	}

	return TRUE;
}

//
//	Restore Registry Data
//
BOOL SetupRegistry(void)
{
	LONG	ret;
	HKEY	hKey;
	DWORD	dispos;
	DWORD	i;

//	Open Registry Key
	ret = RegCreateKeyEx(HKEY_CURRENT_USER,
			szRegName,
			0, NULL, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hKey,
			&dispos);
	if(ret != ERROR_SUCCESS)
	{
		ErrorMsgDlg(hMainWnd, szCreateError);
		return FALSE;
	}

//	Get Registry Data
	for(i = 0; RegTable[i][0]; i++)
	{
		if(RegTable[i][1])
			SetEnumRegistry(hKey, RegTable[i][0],
					(LPDWORD )RegTable[i][2], (DWORD )RegTable[i][1]);
		else
			SetStrRegistry(hKey, RegTable[i][0],
					(LPTSTR )RegTable[i][2], RegTable[i][3]);
	}

	ret = RegCloseKey(hKey);
	if(ret != ERROR_SUCCESS)
	{
		ErrorMsgDlg(hMainWnd, szCloseError);
		return FALSE;
	}
	return TRUE;
}

//
//	Restore Registry Data2 [iwad]
//
BOOL SetupRegistry2(void)
{
	LONG	ret;
	HKEY	hKey;
	DWORD	dispos;

//	Open Registry Key(左ソフトキー)
	ret = RegCreateKeyEx(HKEY_CURRENT_USER,
			TEXT("Software\\Microsoft\\Today\\Keys\\112"),
			0, NULL, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hKey,
			&dispos);
	if(ret != ERROR_SUCCESS)
	{
		ErrorMsgDlg(hMainWnd, szCreateError);
		return FALSE;
	}

//	Get Registry Data
	// [iwad] Todayソフトキー対応
	// [iwad] 左ソフトキーDefault
	SetStrRegistry(hKey, RegTable2[0][0],(LPTSTR )RegTable2[0][2], RegTable2[0][3]);
	// [iwad] 左ソフトキーOpen
	SetStrRegistry(hKey, RegTable2[1][0],(LPTSTR )RegTable2[1][2], RegTable2[1][3]);

	ret = RegCloseKey(hKey);
	if(ret != ERROR_SUCCESS)
	{
		ErrorMsgDlg(hMainWnd, szCloseError);
		return FALSE;
	}

//	Open Registry Key(右ソフトキー)
	ret = RegCreateKeyEx(HKEY_CURRENT_USER,
			TEXT("Software\\Microsoft\\Today\\Keys\\113"),
			0, NULL, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hKey,
			&dispos);
	if(ret != ERROR_SUCCESS)
	{
		ErrorMsgDlg(hMainWnd, szCreateError);
		return FALSE;
	}

//	Get Registry Data
	// [iwad] 右ソフトキーDefault
	SetStrRegistry(hKey, RegTable2[2][0],(LPTSTR )RegTable2[2][2], RegTable2[2][3]);
	// [iwad] 右ソフトキーOpen
	SetStrRegistry(hKey, RegTable2[3][0],(LPTSTR )RegTable2[3][2], RegTable2[3][3]);
	ret = RegCloseKey(hKey);
	if(ret != ERROR_SUCCESS)
	{
		ErrorMsgDlg(hMainWnd, szCloseError);
		return FALSE;
	}
	return TRUE;
}

void ErrorMsgDlg(HWND hWnd, LPCTSTR ErrStr)
{
	MessageBeep(MB_ICONASTERISK);
	MessageBox(hWnd, ErrStr, TEXT("Registry Error"), MB_OK);
}

//
//	Get Registry Data Sub (For DWORD Data)
//
BOOL SetEnumRegistry(HKEY hKey, LPCTSTR RegName,
						LPDWORD VarAdrs, DWORD DefVal)
{
	LONG	ret;
	DWORD	VarSize;
	DWORD	RegType;
	DWORD	VarValue = 0;// Clear High Byte

	ret = RegQueryValueEx(hKey, RegName, NULL, &RegType,
			NULL, &VarSize);
	if(ret == ERROR_SUCCESS)
	{
		ret = RegQueryValueEx(hKey, RegName, NULL, &RegType,
				(LPBYTE )&VarValue, &VarSize);
		if(RegType != REG_BINARY || ret != ERROR_SUCCESS)
		{
			ErrorMsgDlg(hMainWnd, szReadError);
			return FALSE;
		}
		*VarAdrs = VarValue;// Only DWORD Case
	}
	else
	{
		ret = RegSetValueEx(hKey, RegName, NULL, REG_BINARY,
				(LPBYTE )VarAdrs, DefVal);
		if(ret != ERROR_SUCCESS)
		{
			ErrorMsgDlg(hMainWnd, szWriteError);
			return FALSE;
		}
	}
	return TRUE;
}

//
//	Get Registry Data Sub (For LPTSTR Data)
//
BOOL SetStrRegistry(HKEY hKey, LPCTSTR RegName,
						LPTSTR VarAdrs, LPCTSTR DefVal)
{
	LONG	ret;
	DWORD	VarSize;
	DWORD	RegType;

	ret = RegQueryValueEx(hKey, RegName, NULL, &RegType,
			NULL, &VarSize);
	if(ret == ERROR_SUCCESS)
	{
		ret = RegQueryValueEx(hKey, RegName, NULL, &RegType,
				(LPBYTE )VarAdrs, &VarSize);
		if(ret != ERROR_SUCCESS || RegType != REG_SZ)
		{
			ErrorMsgDlg(hMainWnd, szReadError);
			return FALSE;
		}
	}
	else
	{
		_tcscpy(VarAdrs, DefVal);
		ret = RegSetValueEx(hKey, RegName, NULL, REG_SZ,
				(LPBYTE )DefVal, (_tcslen(DefVal)+1)*sizeof(TCHAR));
		if(ret != ERROR_SUCCESS)
		{
			ErrorMsgDlg(hMainWnd, szWriteError);
			return FALSE;
		}
	}
	return TRUE;
}

