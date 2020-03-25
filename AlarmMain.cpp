#include <windows.h>
#include <commctrl.h>
#include "AlarmRes.h"

BOOL AddIcon(HWND, UINT, HICON, LPTSTR);
BOOL DeleteIcon(HWND, UINT, HICON);

#define MYWM_NOTIFYICON	(WM_USER + 1000)
#define ICONSTART   0x1234
#define	APP_RUN_AT_TIME	"AppRunAtTime"

TCHAR	szAppName[] = "AppAlarm";
TCHAR	szExecName[MAX_PATH];
HANDLE	hIcon1;
HMENU	hMenu;
HWND	hWnd, hwndPrev;
SYSTEMTIME	SetTime, CurTime;
STARTUPINFO	Info;
PROCESS_INFORMATION	Stat;
DWORD	Count;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
POINT	MousePos;
TCHAR	Arg[MAX_PATH];
	switch (message)
	{
	case WM_CREATE:
		SetTimer(hWnd, 1, 10000, NULL);
		break;
	case WM_DESTROY:
	    PostQuitMessage(0);
	    break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDM_DISP:
			break;
		case IDM_EXIT:
			DeleteIcon(hWnd, ICONSTART, hIcon1);
			KillTimer(hWnd, 1);
			DestroyWindow(hWnd);
			break;
		default:
			return (DefWindowProc(hWnd, message, wParam, lParam));
		}
		break;
	case MYWM_NOTIFYICON:
		{
		UINT uID;
		UINT uMouseMsg;
			uID = (UINT) wParam;
			uMouseMsg = (UINT)lParam;
			if(uID != ICONSTART)
				break;
			switch(uMouseMsg)
			{
			char	Temp[64];
			case WM_LBUTTONDOWN:
				GetCursorPos(&MousePos);	// Get Mouse Pos
				hMenu = CreatePopupMenu();
				wsprintf(Temp, "%d/%d/%d %2.2d:%2.2d",
					SetTime.wYear, SetTime.wMonth, SetTime.wDay,
					SetTime.wHour, SetTime.wMinute);
				AppendMenu(hMenu, MF_STRING, IDM_DISP, Temp);// Add Item
				SetForegroundWindow(hWnd);
				TrackPopupMenuEx(hMenu,
					TPM_RIGHTALIGN | TPM_BOTTOMALIGN | TPM_LEFTBUTTON,
					MousePos.x, MousePos.y, hWnd, NULL);
			    PostMessage(hWnd, WM_USER, 0, 0);
				DestroyMenu(hMenu);
				break;
			case WM_RBUTTONDOWN:
				GetCursorPos(&MousePos);	// Get Mouse Pos
				hMenu = CreatePopupMenu();
				AppendMenu(hMenu, MF_STRING, IDM_EXIT, "Stop Alarm");// Add Item
				SetForegroundWindow(hWnd);
				TrackPopupMenuEx(hMenu,
					TPM_RIGHTALIGN | TPM_BOTTOMALIGN | TPM_LEFTBUTTON,
					MousePos.x, MousePos.y, hWnd, NULL);
			    PostMessage(hWnd, WM_USER, 0, 0);
				DestroyMenu(hMenu);
				break;
			}
		}
		break;
	case WM_TIMER:
//		Count++;
//		if(Count < 5)
//			break;
//		Count = 0;
		GetLocalTime(&CurTime);
		if(SetTime.wYear == CurTime.wYear
		&& SetTime.wMonth == CurTime.wMonth
		&& SetTime.wDay == CurTime.wDay
		&& SetTime.wHour == CurTime.wHour
		&& SetTime.wMinute == CurTime.wMinute)
		{
			strcpy(Arg, szExecName);
			strcat(Arg, " ");
			strcat(Arg, APP_RUN_AT_TIME);
			Info.cb = sizeof(STARTUPINFO);
			CreateProcess(NULL, Arg, NULL, NULL, FALSE,
				CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS, NULL, NULL,
				&Info, &Stat);
			CloseHandle(Stat.hProcess);
			CloseHandle(Stat.hThread);
			SendMessage(hWnd, WM_COMMAND, IDM_EXIT, 0);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return FALSE;
}

BOOL
InitApplication (HINSTANCE hInstance)
{
WNDCLASS  wc;
	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC) WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName;
    return RegisterClass(&wc);
}

BOOL InitInstance(HINSTANCE hInstance, DWORD CmdShow)
{
	hWnd = CreateWindowEx(0, szAppName, NULL, 0,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	if(!hWnd)
		return FALSE;
	hIcon1 = LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON, 16, 16, 0);
	DeleteIcon(hWnd, ICONSTART, hIcon1);
	AddIcon(hWnd, ICONSTART, (HICON)hIcon1, "AppAlarm");
	DestroyIcon(hIcon1);
	return TRUE;
}
//
//	Get Arguments
//
void GetArg(LPTSTR Str)
{
LPTSTR	StrPtr;
LPTSTR	StartPtr;
int	i, j, k;
unsigned char *TempPtr;
	TempPtr = (unsigned char *)&SetTime;
	StartPtr = strchr(Str, '\"');
	if(StartPtr == NULL)
		return;
	StartPtr++;
	StrPtr = strchr(StartPtr, '\"');
	if(StrPtr == NULL)
		return;
	*StrPtr++ = '\0';
	strcpy(szExecName, StartPtr);
	while(*StrPtr == ' ')
		StrPtr++;
	for(i = 0; i < sizeof(SYSTEMTIME); i++)
	{
		if(!*StrPtr)
			break;
		j = toupper(*StrPtr++) - '0';
		if(j > 9)
			j -= 7;
		if(!*StrPtr)
			break;
		k = toupper(*StrPtr++) - '0';
		if(k > 9)
			k -= 7;
		*TempPtr++ = j * 16 + k;
	}
}
//
//	WinMain
//
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine,
	int nCmdShow)
{
MSG msg;
	while(hwndPrev = FindWindow(szAppName, NULL))
		SendMessage(hwndPrev, WM_COMMAND, IDM_EXIT, 0);// Force End Old One
	if(!lpCmdLine[0])	// No Args?
		return	FALSE;	// Exit
	GetArg(lpCmdLine);
	if(!InitApplication(hInstance))
		return FALSE;
	if(!InitInstance(hInstance, nCmdShow))
		return FALSE;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg);
		DispatchMessage(&msg);
	}
	return	msg.wParam;
}
//
//	Add Icon to TaskBar
//
BOOL AddIcon(HWND hwnd, UINT uID, HICON hicon, LPTSTR lpszTip)
{ 
NOTIFYICONDATA tnid;
	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = hwnd;
	tnid.uID = uID;
	tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnid.uCallbackMessage = MYWM_NOTIFYICON;
	tnid.hIcon = hicon;
	lstrcpy(tnid.szTip, lpszTip);
	return Shell_NotifyIcon(NIM_ADD, &tnid);
} 
//
//	Delete Icon From Task Bar
//
BOOL DeleteIcon(HWND hwnd, UINT uID, HICON hIconOld)
{ 
NOTIFYICONDATA tnid;
	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = hwnd;
	tnid.uID = uID;
	while(Shell_NotifyIcon(NIM_DELETE, &tnid));
	return TRUE;
} 
