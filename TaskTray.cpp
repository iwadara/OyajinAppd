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
//
//	ProtTypes
//

//
//	Works
//
HICON		hIcon;		// TaskIcon Handler
//
//	Add Icon to TaskBar
//
static BOOL AddIcon(HWND hwnd, UINT uID, HICON hicon, LPTSTR lpszTip)
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
static BOOL DeleteIcon(HWND hwnd, UINT uID, HICON hIconOld)
{ 
NOTIFYICONDATA tnid;
	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = hwnd;
	tnid.uID = uID;
	while(Shell_NotifyIcon(NIM_DELETE, &tnid));
	return TRUE;
} 
//
//	Load Icon & Add Icon to TaskBar
//
void AddTaskIcon(HWND hWnd)
{
	hIcon = (HICON )LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON2),
		IMAGE_ICON, 16, 16, 0);
	DeleteIcon(hWnd, ICONSTART, hIcon);
	AddIcon(hWnd, ICONSTART, (HICON)hIcon, TEXT("Oyajin Appd"));
}
//
//	Delete TaskBar Icon
//
void DeleteTaskIcon(HWND hWnd)
{
	DeleteIcon(hWnd, ICONSTART, hIcon);
	DestroyIcon(hIcon);
}
