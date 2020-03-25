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
#include	"stdafx.h"		// [iwad]

//
//	ProtTypes
//
BOOL	InputScreenProc(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK AlarmDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK SetDlgFunc(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK JumpDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK FindDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK RangeDlgProc(HWND, UINT, WPARAM, LPARAM);
void	MainKeyProc(HWND, HDC, WPARAM, LPARAM);
void	MainKeyDownProc(HWND, HDC, WPARAM, LPARAM);
void	MainLButtonProc(HWND, HDC, WPARAM, LPARAM);
void	MainRButtonProc(HWND, HDC, WPARAM, LPARAM);
void	MainDblClickProc(HWND, HDC, WPARAM, LPARAM);
void	SetupMainScreen(HDC);
void	JumpDay(DWORD, DWORD, DWORD);
void	JumpToday(void);
void	DispReverseBox(HDC);
void	SetupMenu(BOOL);
void	SetupInputScreen(HDC);
void	GetNextDate(LPDWORD);
void	SetAlarm(void);
void	KillAlarm(void);
BOOL	FindRecord(DWORD, FILEWORKS *);
static void	SetupFont(HDC);
static void GetSystemFont(void);
BOOL	DelRecord(BOOL);
BOOL	AddRecord(BOOL, BOOL);
void	RestoreFocus(void);
BOOL	OyajiSeekGetRecord(FILEWORKS *);
BOOL	OyajiFileOpen(LPTSTR, FILEWORKS *, DWORD);
void	OyajiFileClose(FILEWORKS *);
BOOL	OyajiExportFile(FILEWORKS *);
BOOL	OyajiSaveFile(FILEWORKS *, FILEWORKS *, DWORD);
BOOL	SetupRegistry(void);
BOOL	SettingSheet(void);
void	SetupBrush(HDC);
void	SetupCurrentPos(void);
void	HideCmdBar(void);
void	ShowCmdBar(void);
void	CreateToolButton(void);
void	GetFinalWinPos(void);
void	CreateInputWindows(HWND);
void	DestroyInputWindows(void);
void	HideInputWindows(void);
void	JumpCurrentDay(DWORD);
DWORD	GetCurrentDate(void);
void	GetToday(void);
DWORD	GetSJIS(LPTSTR, char *, DWORD);
LPCTSTR	GetDateString(DWORD);
static BOOL	TestOpen(void);
static BOOL	TestSync(void);
static BOOL	TestOld(void);
static BOOL	TestFullOpen(void);
static BOOL	TestEdit(void);
static BOOL	TestToDo(void);
static BOOL	TestAlt(void);
static BOOL	TestFull(void);
static BOOL	TestCopy(void);
static BOOL	TestCut(void);
static BOOL	TestDelete(void);
static BOOL	TestPaste(void);
static BOOL	TestDay(void);
static BOOL	TestWeek(void);
static BOOL	TestMonth(void);
static BOOL	TestHalf(void);
static BOOL	TestToDoView(void);
static BOOL	TestJump(void);
BOOL	SetMenuBar(UINT);			// [iwad] ソフトキーメニュー
UINT	MenuBarRes = IDR_MENUBAR1;	// [iwad] ソフトキー状態の保持
//
//	Definision
//
#define	ENABLE		(MF_ENABLED | MF_BYCOMMAND)
#define	DISABLE		(MF_GRAYED | MF_BYCOMMAND)
//
//	Global Vars
//
static	int ViewButtonID[] = {IDM_VIEW_HALFYEAR, IDM_VIEW_MONTH, IDM_VIEW_WEEK,
						 IDM_VIEW_DAY, IDM_VIEW_INPUT, IDM_VIEW_TODO};
static	DWORD	OpenFlag;
static	DWORD MenuTable[][3] = {
	0, IDM_FILE_NEW,		(DWORD )TestOld,
	0, IDM_FILE_OPEN,		(DWORD )TestOld,
	0, IDM_FILE_COPY,		(DWORD )TestEdit,
	0, IDM_FILE_CLOSE,		(DWORD )TestOpen,
	0, IDM_FILE_SYNC,		(DWORD )TestSync,
	0, IDM_FILE_IMPORT,		(DWORD )TestEdit,
	0, IDM_FILE_EXPORT,		(DWORD )TestEdit,
	0, IDM_FILE_REMOVE,		(DWORD )TestEdit,
	0, IDM_FILE_INSERT,		(DWORD )TestFullOpen,
	0, IDM_FILE_WRITE,		(DWORD )TestFullOpen,
//
	1, IDM_EDIT_TODO,		(DWORD )TestToDo,
	17, IDM_EDIT_ALT,		(DWORD )TestAlt,
	17, IDM_EDIT_FULL,		(DWORD )TestFull,
	25, IDM_EDIT_DELETE,		(DWORD )TestDelete,
	1, IDM_EDIT_UNDO,		(DWORD )TestOpen,
	17, IDM_EDIT_CUT,		(DWORD )TestCut,
	17, IDM_EDIT_COPY,		(DWORD )TestCopy,
	17, IDM_EDIT_PASTE,		(DWORD )TestPaste,
//
	10, IDM_VIEW_INPUT,	(DWORD )TestEdit,
	10, IDM_VIEW_DAY,		(DWORD )TestDay,
	10, IDM_VIEW_WEEK,	(DWORD )TestWeek,
	10, IDM_VIEW_MONTH,	(DWORD )TestMonth,
	10, IDM_VIEW_HALFYEAR,(DWORD )TestHalf,
	10, IDM_VIEW_TODO,	(DWORD )TestToDoView,
	10, IDM_VIEW_PRIVATE,	(DWORD )TestOpen,
//	2,	IDM_VIEW_OPTION,	(DWORD )TestOld,
//
	3, IDM_JUMP_DATE,		(DWORD )TestJump,
	3, IDM_JUMP_FIND,		(DWORD )TestJump
};
//
//	Test File Open Menu Enable
//
static BOOL TestOpen(void)
{
	return (OpenFlag != 0);
}
//
//	Test File Open Menu Enable
//
static BOOL TestOld(void)
{
	return (DispMode != 4);
}
//
//	Test File Sync Menu Enable
//
static BOOL TestSync(void)
{
	if(OpenFlag == 0 || SyncLock)
		return FALSE;
	else
		return TestOld();
}
//
//	Test File Insert & Write Menu Enable
//
static BOOL TestFullOpen(void)
{
	if(OpenFlag == 0)
		return FALSE;
	return (DispMode == 4 && dwFullMode);
}
//
//	Test Edit Menu Enable
//
static BOOL TestEdit(void)
{
	if(OpenFlag == 0)
		return FALSE;
	return	(DispMode != 4);
}
//
//	Test Edit ToDo Menu Enable
//
static BOOL TestToDo(void)
{
	if(OpenFlag == 0)
		return FALSE;
	return (DispMode < 4);
}
//
//	Test Edit Alt Menu Enable
//
static BOOL TestAlt(void)
{
	if(OpenFlag == 0)
		return FALSE;
	if(DispMode == 5)
		return (DayOrgIDBuf[dwDayLine] != 0);
	return (DispMode != 4);
}
//
//	Test Edit Full Menu Enable
//
static BOOL TestFull(void)
{
	if(OpenFlag == 0)
		return FALSE;
	if(DispMode == 5)
		return (DayOrgIDBuf[dwDayLine] != 0);
	return TRUE;
}
//
//	Test Edit Delete Menu Enable
//
static BOOL TestDelete(void)
{
	if(OpenFlag == 0 || DispMode == 1)	// Not Open or Month Screen
		return FALSE;
	if(DispMode == 0 || DispMode == 3 || DispMode == 5)
	{
		if(DayOrgIDBuf[dwDayLine] && DayOrgIDBuf[dwDayLine] != HideGenID)
			return TRUE;
	}
	else if(DispMode == 2)
	{
		if(WeekGenIDBuf[dwCurX][WeekCurY] && WeekGenIDBuf[dwCurX][WeekCurY] != HideGenID)
			return TRUE;
	}
	return FALSE;	// UnKnown Case
}
//
//	Test Edit Copy Menu Enable
//
static BOOL TestCopy(void)
{
	if(OpenFlag == 0)
		return FALSE;
	return (DispMode != 1 && DispMode != 5);
}
//
//	Test Edit Cut Menu Enable
//
static BOOL TestCut(void)
{
	if(TestCopy() == 0)
		return FALSE;
	return TestDelete();
}
//
//	Test Edit Paste Menu Enable
//
static BOOL TestPaste(void)
{
	if(OpenFlag == 0)
		return FALSE;
	return (ClipBuf && DispMode != 5);
}
//
//	Test View Day Menu Enable
//
static BOOL TestDay(void)
{
	if(OpenFlag == 0)
		return FALSE;
	return	(DispMode != 3);
}
//
//	Test View Week Menu Enable
//
static BOOL TestWeek(void)
{
	if(OpenFlag == 0)
		return FALSE;
	return	(DispMode != 2);
}
//
//	Test View Month Menu Enable
//
static BOOL TestMonth(void)
{
	if(OpenFlag == 0)
		return FALSE;
	return	(DispMode != 1);
}
//
//	Test View HalfYear Menu Enable
//
static BOOL TestHalf(void)
{
	if(OpenFlag == 0)
		return FALSE;
	return	(DispMode != 0);
}
//
//	Test View ToDo Menu Enable
//
static BOOL TestToDoView(void)
{
	if(OpenFlag == 0)
		return FALSE;
	return	(DispMode != 5);
}
//
//	Test View ToDo Menu Enable
//
static BOOL TestJump(void)
{
	if(OpenFlag == 0)
		return FALSE;
	return	(DispMode < 4);
}
//
// [iwad] Soft Key Menu Bar
//
BOOL SetMenuBar(UINT iRes)
{
	SHMENUBARINFO sMenu;

	if (hTB != NULL)
	{
		SendMessage(hTB, WM_CLOSE, 0, 0);
	}

	memset(&sMenu, 0, sizeof(SHMENUBARINFO));
	sMenu.cbSize = sizeof(SHMENUBARINFO);
	sMenu.hwndParent = hMainWnd;	// メニューバーを所有する(WM_COMMANDを処理する)ウィンドウのハンドル
	sMenu.dwFlags = 0;
	sMenu.nToolBarId = iRes;
	sMenu.hInstRes = hInst;		// アプリケーションのインスタンスハンドル
	sMenu.nBmpId = 0;
	sMenu.cBmpImages = 0;

	if (SHCreateMenuBar(&sMenu))
	{
		hTB = sMenu.hwndMB;
		MenuBarRes = iRes;	// [iwad] ソフトキー状態の保持(Global)
	}

	return 0;
}
//
//	Setup Menu
//
void SetupMenu(BOOL AllMenu)
{
HMENU	hMenu, hSubMenu[4];	// Menu Handler
DWORD	i, j;

//	Get Menu Handle
	if(hTB == NULL)
		return;
#ifdef	_WIN32_WCE
	// [iwad] メニューバーのセット(縦横切替対応のため、現在のソフトキー状態を判断)
	if (MenuBarRes == IDR_MENUBAR2)
		SetMenuBar(IDR_MENUBAR2);
	else
		SetMenuBar(IDR_MENUBAR1);

	if(hCmdBar == NULL)	// No Menu Mode??
		return;
	hMenu = CommandBar_GetMenu(hCmdBar, 0);	// Get Menu Handle
#else
	hMenu = GetMenu(hMainWnd);
#endif
	OpenFlag = (szCurentFileName[0] != TEXT('\0'));
//	Get SubMenu Handles
	for(i = 0; i < 4; i++)
		hSubMenu[i] = GetSubMenu(hMenu, i);
	for(i = 0; i < countof(MenuTable); i++)
	{
		j = MenuTable[i][0];
		if(AllMenu == 0 && ((j & 16) == 0))
			continue;
		if(j & 8)	// TooButton Too?
			SendMessage(hTB, TB_ENABLEBUTTON, MenuTable[i][1], (BOOL )((PROC )MenuTable[i][2])());
		EnableMenuItem(hSubMenu[j & 3], MenuTable[i][1], ((BOOL )((PROC )MenuTable[i][2])())? ENABLE: DISABLE);
	}
	if(AllMenu == 0)	// Only Del Button Mode?
		return;
//	Display Button Pushed
	for(i = 0; i < countof(ViewButtonID); i++)
		SendMessage(hTB, TB_CHECKBUTTON, ViewButtonID[i], OpenFlag && (DispMode == i));
//	Private Button Pushed & Check Menu
	SendMessage(hTB, TB_CHECKBUTTON, IDM_VIEW_PRIVATE, MAKELONG(OpenFlag && dwPrivate, 0));
	CheckMenuItem(hSubMenu[2], IDM_VIEW_PRIVATE,
		((OpenFlag && dwPrivate)? MF_CHECKED: MF_UNCHECKED) | MF_BYCOMMAND);
}
//
//	Setup Delete Button Only
//
void SetupMenuDelete(void)
{
HMENU	hMenu, hSubMenu;	// Menu Handler
	if(hTB == NULL)
		return;		// No Tool Bar
#ifdef	_WIN32_WCE
	if(hCmdBar == NULL)	// No Menu Mode??
		return;
	hMenu = CommandBar_GetMenu(hCmdBar, 0);	// Get Menu Handle
#else
	hMenu = GetMenu(hMainWnd);
#endif
	hSubMenu = GetSubMenu(hMenu, 1);
	SendMessage(hTB, TB_ENABLEBUTTON, IDM_EDIT_DELETE, TestDelete());
	EnableMenuItem(hSubMenu, IDM_EDIT_DELETE, (TestDelete())? ENABLE: DISABLE);
}
//
//	Setup ToolButton
//
void CreateToolButton(void)
{
#ifdef	_WIN32_WCE
// Create CommandBar
	hCmdBar = CommandBar_Create(hInst, hMainWnd, 1);
//	Add BitMap
	CommandBar_AddBitmap(hCmdBar, hInst, IDR_TOOLBAR_BITMAP, 9, 16, 16);
// Create Menu
	// [iwad] CommandBar_InsertMenubar(hCmdBar, hInst, IDR_MENU1, 0);
	CommandBar_InsertMenubar(hCmdBar, hInst, IDR_MENU2, 0);
// Create Buttons
	CommandBar_AddButtons(hCmdBar, numButtons, lpButtons);
	CommandBar_AddToolTips(hCmdBar, numTips, szTips);
//	Create Exit Button
	CommandBar_AddAdornments(hCmdBar, 0, IDM_FILE_EXIT) ;
//    SendMessage(m_hwndCB, TB_SETSTATE, (WPARAM)ID_VIEW_ITINERARY, MAKELONG(TBSTATE_ENABLED|TBSTATE_CHECKED,0));
	hTB = hCmdBar;
	dwWinTop = CommandBar_Height(hCmdBar);
#else	/* 95,NT */
	RECT	tbRect;	// Rect to get TooBar Rect
	hTB = CreateToolbarEx(hMainWnd,
			WS_VISIBLE | WS_CHILD | WS_BORDER | TBSTYLE_TOOLTIPS,
			IDR_TOOLBAR_BITMAP, 9, hInst, IDR_TOOLBAR_BITMAP,
			lpButtons, numButtons,
			0, 0, 16, 16, sizeof(TBBUTTON));
	GetClientRect(hTB, &tbRect);
	dwWinTop = tbRect.bottom + 3;
#endif
}
#ifdef	_WIN32_WCE
//
//	Hide Command Bar & Create Stand Alone Button
//
void HideCmdBar()
{
	if(hWndB)	// Already No Menu Mode?
		return;	// No Operation
	if(hCmdBar)
		CommandBar_Destroy(hCmdBar);
	hCmdBar = NULL;
	hTB = NULL;
	hWndX = CreateWindow(TEXT("BUTTON"), TEXT("X"),
				BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
				dwXsize - 17, 0, 14, 13, hMainWnd, (HMENU )IDM_FILE_EXIT, hInst, NULL);
	hWndB = CreateWindow(TEXT("BUTTON"), TEXT("M"),
				BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
				0, 0, 14, 13, hMainWnd, (HMENU )IDM_VIEW_MINIMIZE, hInst, NULL);
	dwYsize = dwWinHight;
	dwWinTop = 0;
	CreateInputWindows(hMainWnd);
	SetupMainScreen(hMemDC);
	InvalidateRect(hMainWnd, NULL, FALSE);
}
//
//	Show Command Bar
//
void ShowCmdBar()
{
	if(hWndB == NULL)	// Already Menu Mode?
		return;
	if(hWndB)
		DestroyWindow(hWndB);
	if(hWndX)
		DestroyWindow(hWndX);
	hWndB = NULL;	// Set Menu On 
	hWndX = NULL;
	CreateToolButton();
	SetupMenu(TRUE);
	dwYsize = dwWinHight - dwWinTop;
	CreateInputWindows(hMainWnd);
	SetupMainScreen(hMemDC);
	InvalidateRect(hMainWnd, NULL, FALSE);
}
#endif
