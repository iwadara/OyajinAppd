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
//	For KCTRL
#ifdef	_KCTRL
#include	"kctrl.h"
#endif
//	For Alarm
#ifdef	_WIN32_WCE
#include	<Notify.h>
#else
#define	APP_RUN_AT_TIME	TEXT("AppRunAtTime")
#endif
//
//	Prottypes
//
LRESULT	CALLBACK	WindowProc(HWND, UINT, WPARAM, LPARAM);
BOOL 	SetupRegistry(void);
BOOL 	SetupRegistry2(void);	// [iwad] Todayソフトキー対応
BOOL 	SaveRegistry(void);
BOOL 	SaveRegistry2(void);	// [iwad] Todayソフトキー対応
void	OyajiFileClose(FILEWORKS *);
static	BOOL	InitApplication(void);
static	BOOL	InitInstance(int);
static	void	CleanUp(void);
BOOL	InitCustomCtrl(void);
void	CreateToolButton(void);
void	GetFinalWinPos(void);
void	AddTaskIcon(HWND);
void	DeleteTaskIcon(HWND);
//
//	WinMain
//

int	WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst,
#ifdef	_WIN32_WCE	/* CE */
		 LPTSTR lpszArgs,
#else	/* 95,NT */
		 LPSTR lpszArgs,
#endif
		 int nWinMode)
{
//	Local Vars
MSG	msg;	// Message structure
HWND	hWndPrev;
//	Save Instance for DialogBox Proc
	hInst = hThisInst;	// Save Inst for DialogBox
	szArg = lpszArgs;	// Copy Argments Pointer
//	Check double execution

	hWndPrev = FindWindow(szWinName, NULL);
	if(hWndPrev != NULL)
	{
#ifdef	_WIN32_WCE
		ShowWindow(hWndPrev, SW_SHOWNORMAL );
#else	/* 95,NT */
		ShowWindow(hWndPrev, SW_RESTORE);
#endif
		SetForegroundWindow(hWndPrev);
//		InvalidateRect(hWndPrev, NULL, 0);
		UpdateWindow(hWndPrev);
		if(_tcscmp(lpszArgs, APP_RUN_AT_TIME) == 0)	// Check Alarm?
			PostMessage(hWndPrev, WM_ALARM, 0, 0);
		else if(_tcscmp(lpszArgs, TEXT("Update")) == 0)	// Check Alarm?
			PostMessage(hWndPrev, WM_COMMAND, IDM_EDIT_UNDO, 0);
		return FALSE;
	}

//	Initialize Accelerators
#ifdef	_WIN32_WCE	/* CE */
	hAccl = LoadAccelerators(hInst,
				MAKEINTRESOURCE(IDR_ACCELERATOR1));
#else	/* 95,NT */
	hAccl = LoadAccelerators(GetModuleHandle(NULL),
				MAKEINTRESOURCE(IDR_ACCELERATOR1));
#endif
//	Read Data from Registry
	SetupRegistry();
	SetupRegistry2();	// [iwad] Todayソフトキー対応
//	Register Window Class
	if(InitApplication() == FALSE)
		return FALSE;	// Fail to register Class

//	Create Window
	if(InitInstance(nWinMode) == FALSE)
		return FALSE;		// Fail to create Window

//	Init KCTRL
#ifdef	_KTCRL
	if(InitKanjiControls() == FALSE)
		return FALSE;
#endif
//	Event Loop
	AddTaskIcon(hMainWnd);
	while(GetMessage(&msg, NULL, 0, 0))
	{
		if(TranslateAccelerator(hMainWnd, hAccl, &msg) == FALSE)
			TranslateMessage(&msg) ;
		DispatchMessage(&msg);
	}
//	Release KCTRL
#ifdef	_KTCRL
	ReleaseKanjiControls();
#endif
//	Save Registry
	SaveRegistry();		// Save Registry
	SaveRegistry2();	// [iwad] Todayソフトキー対応
	CleanUp();	// CleanUp Reserverd Memorys
	return msg.wParam;
}// End of WinMain

//
// InitApplication (Register Windows Class)
//
BOOL InitApplication(void)
{
#ifdef	_WIN32_WCE	/* CE */

	WNDCLASS	wc;
	wc.hInstance = hInst;
	wc.lpszClassName = szWinName;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	if(dwGrayBack)
		wc.hbrBackground = (HBRUSH )GetStockObject(LTGRAY_BRUSH);
	else
		wc.hbrBackground = (HBRUSH )GetStockObject(WHITE_BRUSH);
	wc.style = CS_DBLCLKS;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.lpszMenuName = NULL;
	return(RegisterClass(&wc));

#else	/* Win95,NT */

	WNDCLASSEX	wc;
	wc.hInstance = hInst;
	wc.lpszClassName = szWinName;
	wc.lpfnWndProc = WindowProc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	if(dwGrayBack)
		wc.hbrBackground = (HBRUSH )GetStockObject(LTGRAY_BRUSH);
	else
		wc.hbrBackground = (HBRUSH )GetStockObject(WHITE_BRUSH);
	wc.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
	wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	wc.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1) ;
	return(RegisterClassEx(&wc));

#endif
}// End of InitApplication

//
//	Create Window
//
BOOL InitInstance(int nCmdShow)
{
//	Create Main Window
	hMainWnd = CreateWindowEx(
		0,								// Extra style
		szWinName,						// Class Name
#ifdef	_WIN32_WCE	/* CE */
		TEXT("Oyajin Appd"),			// Caption
		WS_VISIBLE,						// Window Style
		0, 0,							// Window Left top AXIS
		CW_USEDEFAULT, CW_USEDEFAULT,	// Window Size
#else	/* 95,NT */
		TEXT("Oyajin Appointment"),		// Caption
		WS_OVERLAPPEDWINDOW,			// Window style
		dwStartLeft, dwStartTop,		// Window Left Top AXIS
		dwStartWidth, dwStartHight,		// Window Size
#endif
		NULL,							// No PrevWindow
		NULL,							// No Menu
		hInst,							// This program's Inst
		NULL);							// No Extra Args
//	Check Create result
	if(hMainWnd == NULL)
		return FALSE;
	MainWork.WindowHandle = hMainWnd;
//	Set Small Icon
#ifdef	_WIN32_WCE	/* Only for CE */
	SendMessage(hMainWnd, WM_SETICON,0,
					(LPARAM )LoadImage(hInst,MAKEINTRESOURCE(IDI_ICON2),
						IMAGE_ICON, 16, 16, 0));
#endif
	InitCommonControls();
	CreateToolButton();
// Show MainWindow
	ShowWindow(hMainWnd, nCmdShow) ;
	UpdateWindow(hMainWnd) ;
	return TRUE ;
}
//
//	Cleau Up Reserve Memory
//
void CleanUp(void)
{
	OyajiFileClose(&MainWork);	// Close File & Release Memory
	DeleteTaskIcon(hMainWnd);
	if(hMemDC)	// Check Already Reserved?
		DeleteDC(hMemDC);	// Release Last Memory
	if(hBitBuf)	// BitMap Exists?
		DeleteObject(hBitBuf);	// Release BitMap Buf
	if(hSmallFont)	// Small Font Exists?
		DeleteObject(hSmallFont);	// Release SmallFont
	if(hMidFont)	// Mid Font Exists?
		DeleteObject(hMidFont);	// Release MidFont
	if(hBigFont)	// Big Font Exists?
		DeleteObject(hBigFont);	// Release BigFont
	if(hSystemFont)	// System Font Exists?
		DeleteObject(hSystemFont);	// Release System Font
 	if(hPen)		// Pen Exists?
		DeleteObject(hPen);
	if(hBrush)		// Brush Exists?
		DeleteObject(hBrush);
	if(hGrayBrush)	// Brush Exists?
		DeleteObject(hGrayBrush);
}
