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
#include	"aygshell.h"	// [iwad] タップ&ホールドメニュー用
//	For Alarm
#ifdef		_WIN32_WCE
#include	<Notify.h>
#else
#define	APP_RUN_AT_TIME	TEXT("AppRunAtTime")
#endif
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
BOOL	CALLBACK ImportDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK SyncDlgProc(HWND, UINT, WPARAM, LPARAM);
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
void	AlarmExec(HWND);
BOOL	FindRecord(DWORD, FILEWORKS *);
static void	SetupFont(HDC);
static void GetSystemFont(void);
BOOL	DelRecord(BOOL);
BOOL	AddRecord(BOOL, BOOL);
void	RestoreFocus(void);
BOOL	OyajiSeekGetRecord(FILEWORKS *);
BOOL	OyajiFileOpen(LPTSTR, FILEWORKS *, DWORD);
void	OyajiFileClose(FILEWORKS *);
BOOL	ImportFile(HWND);
BOOL	ExportFile(HWND);
BOOL	RemoveFile();
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
void	InsertTextFromFile(HWND);
void	WriteTextToFile(HWND);
void	SyncStart(HWND);
void	SyncFile(HWND);
void	SyncClear();
//
//	Global Vars
//

//
//	Scales
//
#define	SMALLSIZE	1 / 18
#define	MIDSIZE		1 / 18
#define	BIGSIZE		1 / 12
//
//	Main Window Proc
//
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
PAINTSTRUCT	Pst;
HDC	hDC;
TCHAR	FileName[MAX_PATH];
static	DWORD	TimerCount;
BOOL	DeleteMode;
DWORD	DlgResult;
SHRGINFO RGesture;	// [iwad] タップ&ホールドメニュー用
	switch(message)
	{
//	Initialize Current Date;
	case WM_CREATE:
//	Load File
		if(szCurentFileName[0] != TEXT('\0'))
		{
			_tcscpy(FileName, szCurentDir);
			_tcscat(FileName, szCurentFileName);
			if(OyajiFileOpen(FileName, &MainWork, 0))// Open Mode
				DispMode = 1;	// Month Mode
			else
				_tcscpy(szCurentFileName, TEXT(""));// Clear Current File
		}
		StartTime = dwStartTime * 2;	// Set Day Screen Start Time
		JumpToday();
		dwLastDay = dwTodayDay;			// Set Current Day
		SetTimer(hWnd, 1, 100, NULL);	// Activate Timer
		GetSystemFont();
		break;
//	Get Client Area
	case WM_SIZE:
#ifndef	_WIN32_WCE
		if(hTB)
			SendMessage(hTB, WM_SIZE, wParam, lParam);	// Resize Tool Bar Too
#endif
		if(LOWORD(lParam) == 0 || HIWORD(lParam) == 0)	// Minimized?
			break;
		dwXsize = LOWORD(lParam);
		dwYsize = HIWORD(lParam);
		if(dwWinWidth == dwXsize && dwWinHight == dwYsize)// Same Size
		{
			dwYsize -= dwWinTop;
			break;	// No operation
		}
		dwWinWidth = dwXsize;
		dwWinHight = dwYsize;	// Set Current Size
		dwYsize -= dwWinTop;
//	Clear Garbage
		if(hMemDC)	// Check Already Reserved?
			DeleteDC(hMemDC);	// Release Last Memory
		if(hBitBuf)	// BitMap Exists?
			DeleteObject(hBitBuf);	// Release BitMap Buf
		hDC = GetDC(hWnd);	// Get Device context		
		hMemDC = CreateCompatibleDC(hDC);	// Create Memory DC
		hBitBuf = CreateCompatibleBitmap(hDC, dwXsize, dwYsize + dwWinTop);// Reserve BitBuf
		ReleaseDC(hWnd, hDC);// Release Screen DC
		SelectObject(hMemDC, hBitBuf);	// Set BitBuf
		SetupFont(hMemDC);
//	Setup Brush and Pen
		SetupBrush(hMemDC);
//	Setup Screen
#ifdef	_WIN32_WCE
		if(dwMenuMode)
		{
			HideCmdBar();
		}
		else
		{
			ShowCmdBar();
		}
#endif
		CreateInputWindows(hWnd);
		SetupMainScreen(hMemDC);
		SetupMenu(TRUE);
		break;
//	Paint Window

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &Pst);
		if(hDC && hMemDC)
			BitBlt(hDC, 0, dwWinTop, dwXsize, dwYsize,
					hMemDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &Pst);
		dwDispFlag = TRUE;
		break;
//	Destry window
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
//	X Button
	case WM_CLOSE:
		if(FileLock)
		{
//			if(MessageBox(hWnd,TEXT("Writing File. Exit Anyway?"), TEXT("File Update"), MB_YESNO) != IDYES)
			if(MessageBox(hWnd,TEXT("ﾌｧｲﾙ書き込み中. 終了しますか?"), TEXT("File Update"), MB_YESNO) != IDYES) // by uminchu
				break;
		}
		KillTimer(hWnd, 1);
		Abort = 1;
		if(FileLock)
			while(WaitForSingleObject(BackHandle, 0) == WAIT_TIMEOUT);
		GetFinalWinPos();	// Get Final Window Position
#ifdef	_WIN32_WCE	/* CE */
		if (hCmdBar)
			CommandBar_Destroy(hCmdBar);
		hCmdBar = NULL;
		if(hWndB)
			DestroyWindow(hWndB);
#else	/* 95,NT */
		WinHelp(hWnd, szHelpFileName, HELP_QUIT, 0);
#endif
		DestroyInputWindows();
		OpenClipboard(NULL);
		EmptyClipboard();
		CloseClipboard();
		DestroyWindow(hWnd);
		break;
//	Char Typed
	case WM_SETFOCUS:
	case WM_SYSCHAR:
		if(DispMode == 4)	// Input Screen Hooks Alt+?
			if(InputScreenProc(hWnd, message, wParam, lParam))
				break;
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_TIMEMSG:
		if(DispMode == 4)
			InputScreenProc(hWnd, message, wParam, lParam);
		break;
	case WM_KEYDOWN:
		MainKeyDownProc(hWnd, hMemDC, wParam, lParam);
		break;
	case WM_CHAR:
		MainKeyProc(hWnd, hMemDC, wParam, lParam);
		break;
//	Left Button
	case WM_LBUTTONDOWN:
		MainLButtonProc(hWnd, hMemDC, wParam, lParam);
		// [iwad] タップ&ホールドメニュー表示 ここから
		RGesture.dwFlags = SHRG_RETURNCMD;
		RGesture.cbSize  = sizeof(SHRGINFO);
		RGesture.hwndClient = hWnd; //タップアンドホールドメニューをつけたいウインドウのハンドル
		RGesture.ptDown.x = LOWORD( lParam );	//タップされた位置のＸ座標です
		RGesture.ptDown.y = HIWORD( lParam );	//タップされた位置のＹ座標です
		if( SHRecognizeGesture(&RGesture) == GN_CONTEXTMENU ) {
			//MainRButtonProc(hWnd, hMemDC, wParam, lParam);
			HMENU		hPopMenu;
			HMENU		hWorkMenu;
			hWorkMenu = LoadMenu(hInst,MAKEINTRESOURCE(IDM_MAIN_MENU));	//リソースID IDM_MAIN_MENUのメニューをロード
			hPopMenu  = GetSubMenu(hWorkMenu, 2);	// 2番目のサブメニューを選択
			//フローティングポップアップメニューを表示し項目選択を追跡します
			//項目が選択されるとWM_COMMANDメッセージのwParamのLOWORDに
			//選択されたメニューIDが戻ります
			TrackPopupMenuEx(hPopMenu, 0, RGesture.ptDown.x, RGesture.ptDown.y, hWnd, NULL);
			DestroyMenu(hWorkMenu);	//いらなくなったメニューの消去
		}
		// [iwad] タップ&ホールドメニュー表示 ここまで
		break;
//	Double Click
	case WM_LBUTTONDBLCLK:
		MainDblClickProc(hWnd, hMemDC, wParam, lParam);
		break;
//	Tool Tip Proc
#ifndef	_WIN32_WCE
		case WM_NOTIFY:
		{
		LPTOOLTIPTEXT	TTtext;	// Ptr to ToolTip
			TTtext = (LPTOOLTIPTEXT) lParam;	// Get ToolTip Struct Ptr
			if(TTtext->hdr.code == TTN_NEEDTEXT)
			{
				switch(TTtext->hdr.idFrom)
				{
				case IDM_VIEW_HALFYEAR:
					TTtext->lpszText = (LPTSTR )szTips[1]; break;
				case IDM_VIEW_MONTH:
					TTtext->lpszText = (LPTSTR )szTips[2]; break;
				case IDM_VIEW_WEEK:
					TTtext->lpszText = (LPTSTR )szTips[3]; break;
				case IDM_VIEW_DAY:
					TTtext->lpszText = (LPTSTR )szTips[4]; break;
				case IDM_VIEW_INPUT:
					TTtext->lpszText = (LPTSTR )szTips[5]; break;
				case IDM_VIEW_TODO:
					TTtext->lpszText = (LPTSTR )szTips[6]; break;
				case IDM_VIEW_PRIVATE:
					TTtext->lpszText = (LPTSTR )szTips[7]; break;
				case IDM_EDIT_DELETE:
					TTtext->lpszText = (LPTSTR )szTips[8]; break;
				case IDM_VIEW_MINIMIZE:
					TTtext->lpszText = (LPTSTR )szTips[9]; break;
				}
			}
		}
		break;
#endif
//	Menus
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
//	File Open
		case IDM_FILE_OPEN:
			if(OyajiFileOpen(NULL, &MainWork, 0))	// 0 = Open Mode
			{
				DispMode = 1;
				KillAlarm();
				SetAlarm();
			}
			SetupMenu(TRUE);
			SetupMainScreen(hMemDC);
			InvalidateRect(hWnd, NULL, FALSE);
			break;
//	Copy Current File
		case IDM_FILE_COPY:
			if(OyajiFileOpen(NULL, &MainWork, 1))	// 1 = Copy Mode
				DispMode = 1;
			break;
//	New Open
		case IDM_FILE_NEW:
			if(OyajiFileOpen(NULL, &MainWork, 2))	// 2 = New Mode
			{
				DispMode = 1;
				KillAlarm();
			}
			SetupMenu(TRUE);
			SetupMainScreen(hMemDC);
			InvalidateRect(hWnd, NULL, FALSE);
			break;
//	Close File
		case IDM_FILE_CLOSE:
			OyajiFileClose(&MainWork);
			szCurentFileName[0] = TEXT('\0');// Clear Current File Name
			KillAlarm();
			HideInputWindows();
			DispMode = 0;
			SetupMenu(TRUE);
			SetupMainScreen(hMemDC);
			InvalidateRect(hWnd, NULL, FALSE);
			break;
//	Import File
		case IDM_FILE_IMPORT:
			if(FileLock)	// Now File Working?
			{
				MessageBeep(MB_ICONASTERISK);	// Alarm
				break;		// No Operation
			}
			ImportFile(hWnd);
			SetupMainScreen(hMemDC);
			InvalidateRect(hWnd, NULL, FALSE);
			SetAlarm();
			break;
//	Export File
		case IDM_FILE_EXPORT:
			if(FileLock)	// Now File Working?
			{
				MessageBeep(MB_ICONASTERISK);	// Alarm
				break;		// No Operation
			}
			ExportFile(hWnd);
			break;
//	Remove File
		case IDM_FILE_REMOVE:
			if(FileLock)	// Now File Working?
			{
				MessageBeep(MB_ICONASTERISK);	// Alarm
				break;		// No Operation
			}
			if(DialogBox(hInst, MAKEINTRESOURCE(IDD_RANGEDLG), hWnd, (DLGPROC )RangeDlgProc))
				RemoveFile();
			SetupMainScreen(hMemDC);
			InvalidateRect(hWnd, NULL, FALSE);
			SetAlarm();
			break;
//	Insert Text
		case IDM_FILE_INSERT:
			InsertTextFromFile(hWnd);
			break ;
//	Write Text
		case IDM_FILE_WRITE:
			WriteTextToFile(hWnd);
			break ;
#ifdef	_SYNC
//	Sync File
		case IDM_FILE_SYNC:
			if(szCurentFileName[0] && (dwMulti == 0 || (dwMulti == 1 && FileLock == 0))
			&& DialogBox(hInst, MAKEINTRESOURCE(IDD_SYNCDLG), hWnd, (DLGPROC )SyncDlgProc))
				SyncStart(hWnd);
			break;
#endif
//	Exit Menu
		case IDM_FILE_EXIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break ;
//	Edit Cut
		case IDM_EDIT_CUT:
			SendMessage(hWnd, WM_CHAR, TEXT('\030'), 0);	// Ctrl+X
			break;
//	Edit Copy
		case IDM_EDIT_COPY:
			SendMessage(hWnd, WM_CHAR, TEXT('\003'), 0);	// Ctrl+C
			break;
//	Edit Paste
		case IDM_EDIT_PASTE:
			SendMessage(hWnd, WM_CHAR, TEXT('\026'), 0);	// Ctrl+V
			break;
//	Edit Cut
		case IDM_EDIT_UNDO:
			SendMessage(hWnd, WM_CHAR, TEXT('\032'), 0);	// Ctrl+Z
			break;
//	Delete Menu
		case IDM_EDIT_DELETE:	// Blank Record?
			if(FileLock)		// Now File Working?
			{
				MessageBeep(MB_ICONASTERISK);	// Alarm
				break;		// No Operation
			}
			if(DispMode == 0 || DispMode == 3 || DispMode == 5)
			{
				if(DayOrgIDBuf[dwDayLine] == 0)
					break;	// No Delete Action
			}
			else if(DispMode == 2)
			{
				if(WeekGenIDBuf[dwCurX][WeekCurY] == 0)
					break;	// No Delete Action
			}
			else
				break;
			if(DispMode == 5)	// ToDo Delete?(Change Title Only)
			{	// ToDo Mode?
//				if(dwConfirm && MessageBox(hWnd, TEXT("Mark Done?"), TEXT("ToDo"),MB_OKCANCEL) != IDOK)
				if(dwConfirm && MessageBox(hWnd, TEXT("ﾏｰｸ完了?"), TEXT("ToDo"),MB_OKCANCEL) != IDOK)	// by uminchu
					break;		// No Operation
			}
			else
			{	// Other Screen Mode?
//				if(dwConfirm && MessageBox(hWnd, TEXT("Delete Highlighted Item?"), TEXT("Delete"),
				if(dwConfirm && MessageBox(hWnd, TEXT("ﾊｲﾗｲﾄされたｱｲﾃﾑを削除しますか?"), TEXT("Delete"),	// by uminchu
							MB_OKCANCEL) != IDOK)
				break;
			}
			SetupCurrentPos();
			OyajiSeekGetRecord(&MainWork);// Get Record Data
			DeleteMode = (DispMode != 5);	// ToDo Screen Always Single Change
			if(DeleteMode && dwMultiConfirm && (lpDlgRepeat[0] || dwDlgConsDay != 1))
			{
//				DlgResult = MessageBox(hWnd, TEXT("Delete All Item?"), TEXT("Repeat Delete"),
				DlgResult = MessageBox(hWnd, TEXT("全てのｱｲﾃﾑを削除しますか?"), TEXT("Repeat Delete"),	// by uminchue
							MB_YESNOCANCEL);
				if(DlgResult == IDCANCEL)
					break;
				else if(DlgResult == IDNO)
					DeleteMode = FALSE;
			}
			if(DeleteMode)
				dwDlgDate = dwOrgDate;	// Set Top Date
			if(DispMode == 5)	// ToDo Mode?
			{	// Just Add (Done) After Title
			DWORD	i, Today;
			char	TempDate[16];
				dwOldOrgDate = dwDlgDate;
				dwOldTime = dwDlgStartTime;
				dwOldCons = dwDlgConsDay;	// Save Dlg Data before Change
				dwOldAllDay = dwAllDay;
				dwOldGenID = dwGenID;
				for(i = 0; i < 14; i++)
					lpOldRepeat[i] = lpDlgRepeat[i];
				dwDlgMode = TRUE;	// Replace
				dwDlgConsDay = 1;	// Single Day
				dwDlgEnable = FALSE;// Clear Alarm
				dwOrgDate = dwDlgDate;
				lpDlgRepeat[0] = 0;	// No Repeat
				dwDlgToDo = 0;
				strcat(lpDlgTitle, "(Done ");
				GetToday();
				Today = (dwTodayYear - 1950) * 512 + dwTodayMonth * 32 + dwTodayDay;
				GetSJIS((LPTSTR )GetDateString(Today), TempDate, sizeof(TempDate));
				strcat(lpDlgTitle, TempDate);
				strcat(lpDlgTitle, ")");
				AddRecord(FALSE, TRUE);	// Single Add			
			}
			else
				DelRecord(DeleteMode);	// BOOL but ignore
//	Set Pending GenID Here (To Erase)
			if(DispMode == 0 || DispMode == 3)
				DayOrgIDBuf[dwDayLine] = 0;// Clear Day Buffer
			else if(DispMode == 2)
				WeekGenIDBuf[dwCurX][WeekCurY] = 0;// Clear Week Buffer
			SetupMainScreen(hMemDC);
			InvalidateRect(hWnd, NULL, FALSE);
			break;
//	Minimize
		case IDM_VIEW_MINIMIZE:
#ifdef	_WIN32_WCE
			if(dwMenuMode)
			{
				ShowCmdBar();
				dwMenuMode = FALSE;
			}
			else
			{
				HideCmdBar();
				dwMenuMode = TRUE;
			}
#else
			goto HideScreen;
#endif
			break;
//	Change Display Mode
		case IDM_VIEW_TODO:
			if(DispMode == 4)
			{
				HideInputWindows();
				ToDoBackMode = OldDispMode;	// Save Current Disp Mode
			}
			else if(DispMode != 5)
				ToDoBackMode = DispMode;	// Save Current Disp Mode
			DispMode = 5;
			goto Setup;
		case IDM_VIEW_HALFYEAR:
			HideInputWindows();
			OldDispMode = DispMode;	// Save Current Disp Mode
			DispMode = 0;
			goto Setup;
		case IDM_VIEW_MONTH:
			HideInputWindows();
			OldDispMode = DispMode;	// Save Current Disp Mode
			DispMode = 1;
			goto Setup;
		case IDM_VIEW_WEEK:
			HideInputWindows();
			OldDispMode = DispMode;	// Save Current Disp Mode
			DispMode = 2;
			goto Setup;
		case IDM_VIEW_DAY:
			HideInputWindows();
			OldDispMode = DispMode;	// Save Current Disp Mode
			DispMode = 3;
Setup:
			JumpCurrentDay(dwCurY / 6);
			SetupMenu(TRUE);
			SetupMainScreen(hMemDC);
			SetFocus(hWnd);
			InvalidateRect(hWnd, NULL, FALSE);
			break;			
		case IDM_VIEW_PRIVATE:
			dwPrivate = !dwPrivate;	// Flip Flag
			SetupMenu(TRUE);
			SetupMainScreen(hMemDC);
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case IDM_EDIT_FULL:
			if(DispMode == 4)
				HideInputWindows();
			dwToDoMode = 0;
			dwPrivateMode = 0;
			dwFullMode = 1;
			goto JumpFullScreen;
		case IDM_EDIT_TODO:
			dwToDoMode = 1;
			dwPrivateMode = 0;
			goto JumpInputScreen;
		case IDM_EDIT_ALT:
			dwToDoMode = 0;
			dwPrivateMode = 1;
			goto JumpInputScreen;
		case IDM_VIEW_INPUT:
			if(dwFullMode)
				HideInputWindows();
			dwToDoMode = 0;
			dwPrivateMode = 0;
JumpInputScreen:
			dwFullMode = 0;
JumpFullScreen:
			if(FileLock)	// Now File Working?
			{
				MessageBeep(MB_ICONASTERISK);	// Alarm
				break;		// No Operation
			}
			if(DispMode != 4)	// Prev Screen isn't Input Screen?
				OldDispMode = DispMode;	// Save Current Disp Mode
			SetupInputScreen(hMemDC);
			DispMode = 4;
			SetupMenu(TRUE);
			InvalidateRect(hWnd, NULL, FALSE);
			break;			
//	Set Option Settings
		case IDM_VIEW_OPTION:
			if(FileLock)	// Can't Change BackGround Mode while it Works
				break;
			if(SettingSheet())
			{	// OK Button
				if(dwToDoHalf && DispMode == 0 && (dwCurY) / 6 == 4)// Check Half Year ToDo
					dwCurY = 8;	// Force Set Right Corner Month
				SetupFont(hMemDC);	// Setup Current Font
				if(DispMode != 4)	// if Input Screen then Save Current Screen
				{
					SetupMainScreen(hMemDC);// Update Current Screen
					InvalidateRect(hWnd, NULL, FALSE);// Show!
				}
			}
			break;
//	Jump DialogBox
		case IDM_JUMP_DATE:
			if(DialogBox(hInst, MAKEINTRESOURCE(IDD_JUMPDLG), hWnd, (DLGPROC )JumpDlgProc))
			{
				SetupMainScreen(hMemDC);
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
//	Find DialogBox
		case IDM_JUMP_FIND:
			DlgResult = DialogBox(hInst, MAKEINTRESOURCE(IDD_FINDDLG), hWnd, (DLGPROC )FindDlgProc);
			if(DlgResult)// Find or Next?
			{
				if(FindRecord(DlgResult, &MainWork))
				{	// If Record Found Then Paint Screen
					SetupMainScreen(hMemDC);
					InvalidateRect(hWnd, NULL, FALSE);
				}
			}
			break;
//	About DialogBox
		case IDM_HELP_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTDLG), hWnd, (DLGPROC )AboutDlgProc) ;
			break;
//	Help
		case IDM_HELP_TOPIC:
#ifndef	_WIN32_WCE
			WinHelp(hWnd, szHelpFileName, HELP_FINDER, 0);
#endif
			break;
		}
		break;
//	TaskBar Icon Message
	case MYWM_NOTIFYICON:
		if(wParam != ICONSTART)
			break;
		switch(lParam)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
#ifndef	_WIN32_WCE
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
HideScreen:
#endif
			if(dwDispFlag)
			{	// Current On
			HWND	CurWindow;
			HWND	NextWindow;
				CurWindow = GetActiveWindow();
				if(CurWindow == hWnd)
				{//	Current Program is Active & No Dialog
					ShowWindow(hWnd, SW_HIDE);
					dwDispFlag = FALSE;
					break;
				}
				NextWindow = CurWindow;	// Test Is Child?
				while(NextWindow = GetParent(NextWindow))	// Get Paret Until DeskTop
					if(NextWindow == hWnd)	// If Found
						break;
				if(NextWindow == hWnd)	// This Means Curent Focus Window is Child of hWnd
				{
					SendMessage(CurWindow, WM_COMMAND, (WPARAM )IDCANCEL, 0);// Close Child
					ShowWindow(hWnd, SW_HIDE);
					dwDispFlag = FALSE;
					break;
				}
			}
			// Current Off
#ifdef	_WIN32_WCE
			ShowWindow(hWnd, SW_SHOW);
#else
			ShowWindow(hWnd, SW_RESTORE);
#endif
			SetForegroundWindow(hWnd);
			if(dwToDoPopUp)	// PopUp Option?
				SendMessage(hWnd, WM_COMMAND, (WPARAM )IDM_VIEW_TODO, 0);
			dwDispFlag = TRUE;
			break;
		}
		break;
#ifdef	_SYNC
	case WM_CONNECT:
		SyncFile(hWnd);
		SetAlarm();
		break;
#endif
//	Timer Message
	case WM_TIMER:	// End Thread Message
//	Check Alarm Active
		if(_tcscmp(szArg, APP_RUN_AT_TIME) == 0)// CmdLine is Alarm String?
		{
			_tcscpy(szArg, TEXT(""));	// Clear Arg
			PostMessage(hWnd, WM_ALARM, 0, 0);	// Send Alarm Message
		}
		else if(_tcscmp(szArg, TEXT("Update")) == 0)// CmdLine is Alarm String?
		{
			_tcscpy(szArg, TEXT(""));	// Clear Arg
			PostMessage(hWnd, WM_COMMAND, IDM_EDIT_UNDO, 0);	// Send Alarm Message
		}
//	Check Day Change
		if(TimerCount++ > 100)	// Check Every 10 Second
		{
			TimerCount = 0;
			GetToday();
			if(dwTodayDay != dwLastDay)	// Day Change?
			{
				dwLastDay = dwTodayDay;	// Save New Day Number
				if(DispMode != 4)		// Not Input Mode?
				{
					SetupMainScreen(hMemDC);	// UpDate Screen
					InvalidateRect(hWnd, NULL, FALSE);	// Show!
				}
			}
		}
#ifdef	_SYNC
		if(SyncLock)
		{
			if(SyncTimerCount++ > 600)	// Ceheck Time Out
			{
				SyncLock = 0;
//				MessageBox(hWnd, TEXT("Time Out"), TEXT("Sync"), MB_OK);
				MessageBox(hWnd, TEXT("ﾀｲﾑｱｳﾄ"), TEXT("Sync"), MB_OK);	// by uminchu
				TerminateThread(hSyncHandle, 0);
				SyncClear();
				SetupMenu(TRUE);
			}
		}
#endif
		if(dwMulti == 0)	// BackGround Mode?
			break;
		if(FileLock == 0)
			break;
		if(WaitForSingleObject(BackHandle, 0) == WAIT_TIMEOUT)
			break;
		if(FileResult)	// Success?
			OyajiSaveFile(&MainWork, &BackWork, dwFileMode);
		CloseHandle(BackHandle);
		HideGenID = 0;	// Clear Hide Mode AnyWay
		SetupMainScreen(hMemDC);
		InvalidateRect(hWnd, NULL, FALSE);	// Show!
		FileLock = 0;
		SetAlarm();
		break;
	case WM_ALARM:
		if(AlarmTitle[0] == TEXT('|'))
			AlarmExec(hWnd);
		else
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ALARMDLG), hWnd, (DLGPROC )AlarmDlgProc) ;
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return FALSE;
}
//
//	About Dlg Proc
//
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
TCHAR		szVerBuf[64];			// Buffer for szVerNum
	switch(message)
	{
//	Init About DialogBox
	case WM_INITDIALOG:
		// [iwad] wsprintf(szVerBuf, TEXT("Oyajin Appointment Ver %s"), szVerNum);
		wsprintf(szVerBuf, TEXT("Oyajin Appd Ver %s"), szVerNum);
		SetDlgItemText(hDlg, IDC_VERSION, szVerBuf);
		return TRUE;
//	Ok Button
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDCANCEL:
		case IDOK:
			EndDialog(hDlg, TRUE);
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
//
//	Save Curren Window Axis
//
void GetFinalWinPos(void)
{
RECT	txRect;
	GetWindowRect(hMainWnd, &txRect);
	if(txRect.bottom - txRect.top < 64)
		return;
	dwStartTop = txRect.top;
	dwStartLeft = txRect.left;
	if(txRect.top < 0)
		dwStartTop = 0;
	if(txRect.left < 0)
		dwStartLeft = 0;
	dwStartWidth = txRect.right - txRect.left;
	dwStartHight = txRect.bottom - txRect.top;
}
//
//	Setup Font
//
static void SetupFont(HDC hDC)
{
LOGFONT	LogFont;	// Font Metrics
TEXTMETRIC	tm;
//	Font Settings
	SelectObject(hDC, hSystemFont);	// Dummy Select for Release Font Handle
	if(hSmallFont)	// Small Font Exists?
		DeleteObject(hSmallFont);	// Release Small Font
	if(hMidFont)	// Mid Font Exists?
		DeleteObject(hMidFont);	// Release MidFont
	if(hBigFont)	// Big Font Exists?
		DeleteObject(hBigFont);	// Release BigFont
//	Small Font
	memset(&LogFont, 0, sizeof(LogFont));	// Clear LOGFONT struct
#ifdef	_KCTRL
	LogFont.lfCharSet = DEFAULT_CHARSET;	// Set Font Set
#else
	LogFont.lfCharSet = SHIFTJIS_CHARSET;	// Set Font Set
#endif
	if(szSmallFontName[0] != TEXT(' '))
	{	// User Define
		LogFont.lfHeight = dwSmallFontSize;// Set Font Height(Small)
		_tcscpy(LogFont.lfFaceName, szSmallFontName);// Set Font Name
	}
	else
	{	// Auto (Small)
		LogFont.lfHeight = dwYsize * SMALLSIZE;// Set Font Height(Small)
		dwSmallFontSize = LogFont.lfHeight;	// Save Default Font Height
	}
	hSmallFont = CreateFontIndirect(&LogFont);// Get Font Object
	SelectObject(hDC, hSmallFont);		// Set Small Font
	GetTextMetrics(hDC, &tm);	// Get Semall Font Metrics
	dwSmallFontHight = tm.tmHeight;	// Get Small Font Height
	dwSmallFontWidth = tm.tmAveCharWidth;	// Get Small Font Width
//	Mid Font
	memset(&LogFont, 0, sizeof(LogFont));	// Clear LOGFONT struct
#ifdef	_KCTRL
	LogFont.lfCharSet = DEFAULT_CHARSET;	// Set Font Set
#else
	LogFont.lfCharSet = SHIFTJIS_CHARSET;	// Set Font Set
#endif
	if(szMidFontName[0] != TEXT(' '))
	{	// User Define
		LogFont.lfHeight = dwMidFontSize;// Set Font Height(Small)
		_tcscpy(LogFont.lfFaceName, szMidFontName);// Set Font Name
	}
	else
	{	// Auto (Mid)
		LogFont.lfHeight = dwYsize * MIDSIZE;// Set Font Height(Small)
		dwMidFontSize = LogFont.lfHeight;	// Save Default Font Height
	}	
	hMidFont = CreateFontIndirect(&LogFont);// Get Font Object
	SelectObject(hDC, hMidFont);		// Set Small Font
	GetTextMetrics(hDC, &tm);	// Get Semall Font Metrics
	dwMidFontHight = tm.tmHeight;	// Get Small Font Height
	dwMidFontWidth = tm.tmAveCharWidth;	// Get Small Font Width
//	Big Font
	memset(&LogFont, 0, sizeof(LogFont));	// Clear LOGFONT struct
#ifdef	_KCTRL
	LogFont.lfCharSet = DEFAULT_CHARSET;	// Set Font Set
#else
	LogFont.lfCharSet = SHIFTJIS_CHARSET;	// Set Font Set
#endif
	if(szBigFontName[0] != TEXT(' '))
	{	// User Define
		LogFont.lfHeight = dwBigFontSize;// Set Font Height(Small)
		_tcscpy(LogFont.lfFaceName, szBigFontName);// Set Font Name
	}
	else
	{	// Auto (Big)
		LogFont.lfHeight = dwYsize * BIGSIZE;// Set Font Height(Small)
		dwBigFontSize = LogFont.lfHeight;	// Save Default Font Height
	}	
	hBigFont = CreateFontIndirect(&LogFont);// Get Font Object
	SelectObject(hDC, hBigFont);		// Set Small Font
	GetTextMetrics(hDC, &tm);	// Get Semall Font Metrics
	dwBigFontHight = tm.tmHeight;	// Get Small Font Height
	dwBigFontWidth = tm.tmAveCharWidth;	// Get Small Font Width
}
//
//	Get System Font
//
static void GetSystemFont(void)
{
LOGFONT	LogFont;	// Font Metrics
	memset(&LogFont, 0, sizeof(LogFont));	// Clear LOGFONT struct
	LogFont.lfCharSet = DEFAULT_CHARSET;	// Set Font Set
	LogFont.lfHeight = 8;					// Set Font Height(Min)
	_tcscpy(LogFont.lfFaceName, TEXT("System"));// Set System Font Name
	hSystemFont = CreateFontIndirect(&LogFont);// Get Font Object
}
