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
#include	<commDlg.h>
#include	"AppRes.h"
#include	"External.h"
#include	"Rapi101.h"
//
//	ProtTypes
//
void	SetupMenu(BOOL);
//
//	Definision
//

//
//	Global Vars
//
//
//	Sync Init
//
static DWORD WINAPI SyncInitThread(HWND hWnd)
{
	PegRapiInit();
	PostMessage(hWnd, WM_CONNECT, 0, 0);	// Sync End Message
	return TRUE;
}
//
//	Sync File
//
void SyncStart(HWND hWnd)
{
	if(SyncLock)	// Already Avtivate?
		return;
	SyncLock = TRUE;
	SyncTimerCount = 0;	// Clear TimeOut Counter
	SetupMenu(TRUE);
	hSyncHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE )SyncInitThread, (LPVOID )hWnd, 0, &SyncThreadID);
}
//
//	Sync Clear
//
void SyncClear(void)
{
	PegRapiUninit();
}
//
//	
//	
void SyncFile(HWND hWnd)
{
HKEY	hKey;
DWORD	dispos;
DWORD	VarSize;
DWORD	RegType;
DWORD	ByteRead;
DWORD	ByteWrote;
HANDLE	hSourceFile, hDestFile;
wchar_t	TempUniStr[MAX_PATH];
wchar_t	TempUniDir[MAX_PATH];
wchar_t	ExecName[MAX_PATH];
wchar_t	*UniDotPtr;
char	NewFileName[MAX_PATH];
char	FileName[MAX_PATH];
char	TempStr[MAX_PATH];
char	*DotPtr;
char	*DiskBuf;
PROCESS_INFORMATION	ProcInfo;
	MultiByteToWideChar(0, 0, szRegName, -1, TempUniStr, MAX_PATH);
//	Get Current FileName From Registry
	if(PegRegCreateKeyEx(HKEY_CURRENT_USER,
			TempUniStr,
			0, NULL, REG_OPTION_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hKey,
			&dispos) != ERROR_SUCCESS)
		goto NotFound;
	if(PegRegQueryValueEx(hKey, L"ExecuteName", NULL, &RegType,
			NULL, &VarSize) != ERROR_SUCCESS)
		goto NotFound;
	if(PegRegQueryValueEx(hKey, L"ExecuteName", NULL, &RegType,
			(LPBYTE )ExecName, &VarSize) != ERROR_SUCCESS)
		goto NotFound;
	if(PegRegQueryValueEx(hKey, L"AppFileDir", NULL, &RegType,
			NULL, &VarSize) != ERROR_SUCCESS)
		goto NotFound;
	if(PegRegQueryValueEx(hKey, L"AppFileDir", NULL, &RegType,
			(LPBYTE )TempUniDir, &VarSize) != ERROR_SUCCESS)
		goto NotFound;
	if(PegRegQueryValueEx(hKey, L"AppFileName", NULL, &RegType,
			NULL, &VarSize) != ERROR_SUCCESS)
		goto NotFound;
	if(PegRegQueryValueEx(hKey, L"AppFileName", NULL, &RegType,
			(LPBYTE )TempUniStr, &VarSize) != ERROR_SUCCESS)
		goto NotFound;
	if(PegRegCloseKey(hKey) != ERROR_SUCCESS)
	{
NotFound:
		MessageBox(hWnd, "Registry Can't Read", "Registry", MB_OK);
		goto TimeOut;
	}
	wcscat(TempUniDir, TempUniStr);
	WideCharToMultiByte(0, 0, TempUniStr, -1, FileName, MAX_PATH, NULL, NULL);
//	Check Same FileName?
	if((strlen(FileName) >= strlen(szCurentFileName))
	&& (strstr(FileName, szCurentFileName) != (char *)(FileName + strlen(FileName) - strlen(szCurentFileName))))
	{
		if(TransDir)
			wsprintf(TempStr, "Copy Windows:%s to WCE:%s ???", szCurentFileName, FileName);
		else
			wsprintf(TempStr, "Copy WCE:%s to Windows:%s ???", FileName, szCurentFileName);
		if(MessageBox(hWnd, TempStr, "Different Name Warning", MB_OKCANCEL) == IDCANCEL)
		{
TimeOut:
			TerminateThread(hSyncHandle, 0);
			SyncLock = 0;
			SyncClear();
			SetupMenu(TRUE);
			return;
		}
	}
//	Open & Copy File
	if(TransDir)
	{
		UniDotPtr = wcschr(TempUniDir, L'.');	// Serarch Dot
		if(UniDotPtr)
			*UniDotPtr = L'\0';			// Erase After Dot
		wcscat(TempUniDir, L".prv");	// New Extension
		if((hSourceFile = PegCreateFile(TempUniDir,
			GENERIC_WRITE,	// Read & Write
			0,	// No Share
			NULL,	// No Seculity Attributes
			CREATE_ALWAYS,	// If File not exists then Fail
			FILE_ATTRIBUTE_NORMAL,		// Normal File
			NULL)) == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, "File Can't Create", "WCE File", MB_OK);
			PegRapiUninit();
			return;
		}
		strcpy(NewFileName, szCurentDir);
		strcat(NewFileName, szCurentFileName);		// Full Temp Name
		if((hDestFile = CreateFile(NewFileName,
			GENERIC_READ,	// Write Only
			FILE_SHARE_READ,	// No Share
			NULL,	// No Seculity Attributes
			OPEN_EXISTING,		// If File exists then Delete
			FILE_ATTRIBUTE_NORMAL |		// Normal File
			FILE_FLAG_WRITE_THROUGH,	// Write to Cash Immidiate
			NULL)) == INVALID_HANDLE_VALUE)		// No Template
		{
			MessageBox(hWnd, "File Not Found", "Windows File", MB_OK);
			PegRapiUninit();
			return;
		}
	}
	else
	{
		if((hSourceFile = PegCreateFile(TempUniDir,
			GENERIC_READ,	// Read & Write
			FILE_SHARE_READ,	// No Share
			NULL,	// No Seculity Attributes
			OPEN_EXISTING,	// If File not exists then Fail
			FILE_ATTRIBUTE_NORMAL |		// Normal File
			FILE_FLAG_WRITE_THROUGH,	// Write to Cash Immidiate
			NULL)) == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, "File Not Found", "WCE File", MB_OK);
			PegRapiUninit();
			return;
		}
		strcpy(NewFileName, szCurentDir);
		strcpy(FileName, szCurentFileName);	// Create Temp FIle Name
		DotPtr = strchr(FileName, '.');	// Serarch Dot
		if(DotPtr)
			*DotPtr = '\0';			// Erase After Dot
		strcat(FileName, ".prv");	// New Extension
		strcat(NewFileName, FileName);		// Full Temp Name
		if((hDestFile = CreateFile(NewFileName,
			GENERIC_WRITE,	// Write Only
			0,	// No Share
			NULL,	// No Seculity Attributes
			CREATE_ALWAYS,		// If File exists then Delete
			FILE_ATTRIBUTE_NORMAL,		// Normal File
			NULL)) == INVALID_HANDLE_VALUE)		// No Template
		{
			MessageBox(hWnd, "File Can't Create", "Windows File", MB_OK);
			PegRapiUninit();
			return;
		}
	}
	DiskBuf = (char *)LocalAlloc(LMEM_FIXED, 4096);
	if(DiskBuf == NULL)
	{
		MessageBox(hWnd, "Memory Can't Allocate", "Alloc", MB_OK);
			goto MemoryError;
	}
	while(TRUE)
	{
		if(TransDir)
		{
			if(ReadFile(hDestFile, DiskBuf, 4096, &ByteRead, 0) == 0)
			{
				MessageBox(hWnd, "File Can't Read", "Windows File", MB_OK);
				goto DeviceError;
			}
		}
		else
		{
			if(PegReadFile(hSourceFile, DiskBuf, 4096, &ByteRead, 0) == 0)
			{
				MessageBox(hWnd, "File Can't Read", "WCE File", MB_OK);
				goto DeviceError;
			}
		}
		if(TransDir)
		{
			if(PegWriteFile(hSourceFile, DiskBuf, ByteRead, &ByteWrote, 0) == 0)
			{
				MessageBox(hWnd, "File Can't Write", "WCE File", MB_OK);
				goto DeviceError;
			}
		}
		else
		{
			if(WriteFile(hDestFile, DiskBuf, ByteRead, &ByteWrote, 0) == 0)
			{
				MessageBox(hWnd, "File Can't Write", "Windows File", MB_OK);
				goto DeviceError;
			}
		}
		if(ByteRead != 4096)
			break;	// End of Copy
	}
	LocalFree(DiskBuf);
	PegCloseHandle(hSourceFile);
	CloseHandle(hDestFile);
	if(TransDir)
		PegCreateProcess(ExecName, L"Update", NULL, NULL, FALSE, 0, NULL, NULL, NULL, &ProcInfo);
	PegRapiUninit();
	if(TransDir == FALSE)
		SendMessage(hWnd, WM_COMMAND, IDM_EDIT_UNDO, 0);
	SyncLock = 0;
	SetupMenu(TRUE);
//	MessageBox(hWnd, TEXT("Transfer Completed"), TEXT("Sync"), MB_OK);
	MessageBox(hWnd, TEXT("“]‘—Š®—¹"), TEXT("Sync"), MB_OK); // by uminchu
	return;
//
DeviceError:
	LocalFree(DiskBuf);
MemoryError:
	PegCloseHandle(hSourceFile);
	CloseHandle(hDestFile);
	PegRapiUninit();
//	MessageBox(hWnd, TEXT("Transfer Failed"), TEXT("Sync"), MB_OK);
	MessageBox(hWnd, TEXT("“]‘—Ž¸”s"), TEXT("Sync"), MB_OK); // by uminchu
	return;
}
//
//	Repeat Dialog Proc
//
BOOL CALLBACK SyncDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
switch(message)
	{
//	Init Repeat Dialog
	case WM_INITDIALOG:
//	Setup Dialog Control Text
		SendDlgItemMessage(hDlg, IDC_RECV, BM_SETCHECK, !TransDir, 0);
		SendDlgItemMessage(hDlg, IDC_SEND, BM_SETCHECK, TransDir, 0);
		return TRUE;
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDOK:
			TransDir = SendDlgItemMessage(hDlg, IDC_SEND, BM_GETCHECK, 0, 0);
			EndDialog(hDlg, TRUE);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return TRUE;
		}
		break;
//	X Button
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		return TRUE;
	}
	return FALSE;
}
