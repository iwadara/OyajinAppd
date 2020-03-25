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
BOOL CALLBACK RepeatDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK OyajiEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK OyajiTimeEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK OyajiDateEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK OyajiByteEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK OyajiButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK OyajiPushButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK OyajiRepeatButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK OyajiMultiEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL	OyajiSeekGetRecord(FILEWORKS *);
LPCTSTR GetTimeString(DWORD);
LPCTSTR GetDateString(DWORD);
LPCTSTR GetDateOnlyString(DWORD);
DWORD	GetCurrentYear(DWORD);
DWORD	GetCurrentMonth(DWORD);
DWORD	GetCurrentDay(DWORD);
DWORD	GetCurrentWeekDay(void);
DWORD	GetCurrentDate(void);
DWORD	GetFinalDay(DWORD);
DWORD	DwordToBit(DWORD);
void	JumpDay(DWORD, DWORD, DWORD);
void	JumpDate(DWORD);
BOOL	ConvTimeToDword(LPDWORD, LPTSTR);
BOOL	ConvDateToDword(LPDWORD, LPTSTR);
void	SetupDlgSub(void);
void	SetupCurrentPos(void);
void	CreateInputWindows(HWND);
void	DestroyInputWindows(void);
void	SetupInputBrush(HDC);
static void	EnableInputWindows(void);
static void	ShowInputWindows(void);
static void	SetupInputDlg(void);
static void	SetupDlgNew(void);
static void	SetupDlgFile(void);
void	HideInputWindows(void);
void	SetupHolydayBrush(HDC, BOOL);
void	SetupMainScreen(HDC);
void	RestoreFocus(void);
static void	SetTopFocus(void);
void	GetNextDate(LPDWORD);
void	GetPrevDate(LPDWORD);
void	GetUniCode(char *, LPTSTR);
DWORD	GetSJIS(LPTSTR, char *, DWORD);
DWORD	GetDate(DWORD, DWORD, DWORD);
DWORD	GetTime(DWORD, DWORD);
DWORD	AddTime(DWORD, DWORD);
BOOL	AddRecord(BOOL, BOOL);
HWND	GetNextFocus(HWND, DWORD);
DWORD	ConvHalfHour(DWORD);
BOOL	FileOpenDialog(HWND, BOOL, OPENFILENAME *, LPTSTR, LPTSTR, LPTSTR, LPTSTR, LPTSTR);
//
//	Globals
//
static	HWND	hWndArray[64];	// Dialog Box Control Handler Array
static	HWND	hStaticArray[64];// Static Text Handler Array
static	HWND	hFocusArray[64];// Focus Search Table
static	BOOL	CheckBox[64];	// Check Box Flag Array
static	DWORD	WindowCount;	// Total Box Counter
static	DWORD	FocusPos;		// Curent Focus Index
static	WNDPROC	lpButton;		// Edit Class Proc Adrs
static	DWORD	MultiChange;		// Multi Change Flag
static	DWORD	dwEditDate;		// Change Target Date(for 1 Rec Change)
static	HWND	hFullWnd;		// Full Screen Edit Window Handle
static	HWND	hTitleWnd;		// Full Screen Edit Window Handle
static	HWND	hOkWnd;			// Ok Button Window Handle
static	HWND	hCancelWnd;		// Cancel Button Window Handle
//
//	Constants
//
// #define	LTGRAYCOLOR	0x00c0c0c0
DWORD	LTGRAYCOLOR = 0x00BBBBFF;	// [iwad] ãxì˙îwåiêF(ëΩêFëŒâû)
//
static LPCTSTR	InputErrorMsg[] = {	// Error Message for Dialog
	TEXT("No Description"), TEXT("Invalid Time"), TEXT("Invalid Date")};

//
//	Setup Input Screen to Memory Device
//
void SetupInputScreen(HDC hDC)
{
	SetupInputBrush(hDC);
	PatBlt(hDC, 0, 0, dwXsize, dwYsize, PATCOPY);	// Clear Memory Device
	EnableInputWindows();	// Edit Windows Activate
	SetupInputDlg();		// Setup Value
	ShowInputWindows();		// Show Windows
	SetTopFocus();			// Set Focus to 1st Windows
}
//
//	Setup Brush
//
void SetupBrush(HDC hDC)
{
LOGPEN LogPen;
	memset(&LogPen, 0, sizeof(LogPen));	// Clear LOGPEN Struct
	LogPen.lopnStyle = PS_SOLID;
	if(dwLineColor)
		LogPen.lopnColor = dwLineColor;
	else
#ifdef	_WIN32_WCE
		LogPen.lopnColor = RGB(0, 0, 0);
#else
		LogPen.lopnColor = GetSysColor(COLOR_WINDOWTEXT);
#endif
	if(hPen == NULL)	// Handle is Blank?
		hPen = CreatePenIndirect(&LogPen);
	if(hBrush == NULL)	// Not Defined Yet?
	{
		if(dwBackColor)	// Handle is Blank?
			hBrush = CreateSolidBrush(dwBackColor);
		else
#ifdef	_WIN32_WCE
			hBrush = CreateSolidBrush(RGB(255, 255, 255));
#else
			hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
#endif
	}
	if(hGrayBrush == NULL)	// Gray Handle is Blank?
	{
		if(dwGrayColor)
			hGrayBrush = CreateSolidBrush(dwGrayColor);
		else
			hGrayBrush = CreateSolidBrush(LTGRAYCOLOR);
	}
	if(dwTextColor)
		SetTextColor(hDC, dwTextColor);
	else
#ifdef	_WIN32_WCE
		SetTextColor(hDC, RGB(0, 0, 0));
#else
		SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
#endif
	SelectObject(hDC, hPen);
	SetupHolydayBrush(hDC, FALSE);	// FALSE Not Gray Back
}
//
//	Setup Holyday Brush
//
void SetupHolydayBrush(HDC hDC, BOOL GrayBack)
{
	if(GrayBack)
	{
		SelectObject(hDC, hGrayBrush);
		if(dwGrayColor)
			SetBkColor(hDC, dwGrayColor);
		else
			SetBkColor(hDC, LTGRAYCOLOR);
	}
	else
	{
		SelectObject(hDC, hBrush);
		if(dwBackColor)
			SetBkColor(hDC, dwBackColor);
		else
#ifdef	_WIN32_WCE
			SetBkColor(hDC, RGB(255, 255, 255));
#else
			SetBkColor(hDC, GetSysColor(COLOR_WINDOW));
#endif
	}
}
//
//	Setup Input Brush
//
void SetupInputBrush(HDC hDC)
{
	if(dwGrayBack)
	{
		SelectObject(hDC, (HBRUSH )GetStockObject(LTGRAY_BRUSH));
		SetBkColor(hDC, RGB(192, 192, 192));
	}
	else
	{
		SelectObject(hDC, (HBRUSH )GetStockObject(WHITE_BRUSH));
		SetBkColor(hDC, RGB(255, 255, 255));
	}
}
//
//	Setup Child Windows
//
void CreateInputWindows(HWND hParWnd)
{
FIELD	*fptr;
DWORD	Index = 0;
DWORD	Style, FieldAttr;
TCHAR	*ClassName = TEXT("");
HWND	hWnd;
TCHAR	String[256];
	//DestroyInputWindows();	// Clear Old Windows
	for(fptr = MainWork.FieldList; fptr->Attr; fptr++)
	{	// Repeat for All Field
		FieldAttr = fptr->Attr;	// Get Attr
		Style = 0;
		if(fptr->Width == 0)	// Blank Field?
			continue;		// Skip
		if(FieldAttr > FLREPEAT)	// Invisible Record?
			continue;		// Skip
		CheckBox[Index] = FALSE;// Clear CheckBox Flag
		hFocusArray[Index] = NULL;	// Clear Focus Search Buffer
		hStaticArray[Index] = NULL;	// Clear Alt+? Search Array
		switch(FieldAttr)
		{
		case FLCHECK:
			ClassName = TEXT("BUTTON");
			Style = WS_GROUP | BS_AUTOCHECKBOX | WS_TABSTOP;
			goto SkipNextStatic;
		case FLRADIO:
			ClassName = TEXT("BUTTON");
			Style = WS_GROUP | BS_AUTORADIOBUTTON | WS_TABSTOP;
SkipNextStatic:
			if((fptr + 1)->Attr == FLSTATIC)	// Next Field is Static?
				fptr++;	// Yes then Set Ptr to Static (to Get Caption)
			CheckBox[Index] = TRUE;	// Set Check Box Flag (For TAB Search)
			break;
		case FLBYTE:
		case FLSHORT:
		case FLDATE:
		case FLTIME:
		case FLDWORD:
		case FLSINGLE:
		case FLQWORD:
		case FLDOUBLE:
		case FLCURRENCY:
		case FLSTRING:
			ClassName = TEXT("EDIT");
			Style = WS_GROUP | ES_AUTOHSCROLL | WS_BORDER | WS_TABSTOP;
			break;
//		case FLCOMBOBOX:
//			ClassName = TEXT("COMBOBOX");
//			Style = CBS_AUTOHSCROLL;
//			break;
		case FLSTATIC:
			ClassName = TEXT("STATIC");			
			Style = SS_LEFT;
			break;
		case FLGROUPBOX:
			ClassName = TEXT("BUTTON");
			Style = BS_GROUPBOX;
			break;
		case FLREPEAT:
			ClassName = TEXT("BUTTON");
			Style = WS_GROUP | WS_TABSTOP;
			break;
		case FLMULTIEDIT:
			ClassName = TEXT("EDIT");
			Style = ES_AUTOHSCROLL | ES_AUTOVSCROLL
				| WS_GROUP | WS_BORDER | ES_MULTILINE | WS_TABSTOP;
		}
		hWnd = CreateWindowEx(
		0,								// Extra style
		ClassName,						// Class Name
		NULL,			// No Caption
		WS_DISABLED | WS_CHILD | Style,	// Window style
		fptr->Left * dwXsize / 256,
		fptr->Top * dwYsize / 128 + dwWinTop,
		fptr->Width * dwXsize / 256,
		fptr->Hight * dwYsize / 128,
		hParWnd,						// Parent is Main Window
		(HMENU )Index,					// Window Uni ID
		hInst,							// This program's Inst
		NULL);							// No Extra Args
		switch(FieldAttr)
		{
		case FLCHECK:
		case FLRADIO:
			if(lpButton == NULL)
				lpButton = (WNDPROC )GetWindowLong(hWnd, GWL_WNDPROC);	// Store Button Handler Adrs
			SetWindowLong(hWnd, GWL_WNDPROC, (LONG )OyajiButtonProc);	// Set Hook Handler Adrs
			hStaticArray[Index] = hWnd;	// Save Handle for ALT Search
			goto SetCaption;
		case FLBYTE:
			if(lpEdit == NULL)	// Not Set Yet?
				lpEdit = (WNDPROC )GetWindowLong(hWnd, GWL_WNDPROC);// Store Edit Handler Adrs
			SendMessage(hWnd, EM_SETLIMITTEXT, (WPARAM )fptr->MaxLen, 0);// Set Text Limit
			SetWindowLong(hWnd, GWL_WNDPROC, (LONG )OyajiByteEditProc);	// Set Hook Handler Adrs
			break;
		case FLDATE:
			if(lpEdit == NULL)	// Not Set Yet?
				lpEdit = (WNDPROC )GetWindowLong(hWnd, GWL_WNDPROC);// Store Edit Handler Adrs
			SendMessage(hWnd, EM_SETLIMITTEXT, (WPARAM )fptr->MaxLen, 0);
			SetWindowLong(hWnd, GWL_WNDPROC, (LONG )OyajiDateEditProc);	// Set Hook Handler Adrs
			break;
		case FLTIME:
			if(lpEdit == NULL)	// Not Set Yet?
				lpEdit = (WNDPROC )GetWindowLong(hWnd, GWL_WNDPROC);// Store Edit Handler Adrs
			SendMessage(hWnd, EM_SETLIMITTEXT, (WPARAM )fptr->MaxLen, 0);
			SetWindowLong(hWnd, GWL_WNDPROC, (LONG )OyajiTimeEditProc);	// Set Hook Handler Adrs
			break;
		case FLSHORT:
		case FLDWORD:
		case FLSTRING:
			if(lpEdit == NULL)	// Not Set Yet?
				lpEdit = (WNDPROC )GetWindowLong(hWnd, GWL_WNDPROC);// Store Edit Handler Adrs
			SendMessage(hWnd, EM_SETLIMITTEXT, (WPARAM )fptr->MaxLen, 0);
			SetWindowLong(hWnd, GWL_WNDPROC, (LONG )OyajiEditProc);	// Set Hook Handler Adrs
			break;
		case FLSTATIC:	// Static Text
		case FLGROUPBOX:
SetCaption:
			GetUniCode((char *)fptr->Ptr, String);
			SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM )String);// Set Static Text
			hStaticArray[Index] = hWnd;	// Save Handle for ALT Search
			break;
		case FLREPEAT:
			if(lpEdit == NULL)	// Not Set Yet?
				lpEdit = (WNDPROC )GetWindowLong(hWnd, GWL_WNDPROC);// Store Edit Handler Adrs
			SetWindowLong(hWnd, GWL_WNDPROC, (LONG )OyajiRepeatButtonProc);	// Set Hook Handler Adrs
			break;
		case FLMULTIEDIT:
			if(lpEdit == NULL)	// Not Set Yet?
				lpEdit = (WNDPROC )GetWindowLong(hWnd, GWL_WNDPROC);// Store Edit Handler Adrs
			SendMessage(hWnd, EM_SETLIMITTEXT, (WPARAM )fptr->MaxLen, 0);
			SetWindowLong(hWnd, GWL_WNDPROC, (LONG )OyajiMultiEditProc);	// Set Hook Handler Adrs
			break;
		}
		if(FieldAttr != FLSTATIC && FieldAttr != FLGROUPBOX)
			hFocusArray[Index] = hWnd;// Set TAB Search Array
		hWndArray[Index] = hWnd;	// Save Window Handle for Redraw & Destroy
		Index++;	// Next
	}
//	Add OK Button
	hWnd = CreateWindowEx(
	0,								// Extra style
	TEXT("BUTTON"),					// Class Name
	TEXT("OK"),			// No Caption
	WS_DISABLED | WS_DISABLED | WS_GROUP | WS_TABSTOP | BS_DEFPUSHBUTTON,	// Window style
	//215 * dwXsize / 256,
	210 * dwXsize / 256,	// [iwad]
	95 * dwYsize / 128 + dwWinTop,
	//30 * dwXsize / 256,
	40 * dwXsize / 256,	// [iwad]
	14 * dwYsize / 128,
	hParWnd,						// Parent is Main Window
	(HMENU )IDOK,					// Window Uni ID
	hInst,							// This program's Inst
	NULL);							// No Extra Args
	if(lpButton == NULL)
		lpButton = (WNDPROC )GetWindowLong(hWnd, GWL_WNDPROC);	// Store Button Handler Adrs
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG )OyajiPushButtonProc);// Set Hook Handler Adrs
	hOkWnd = hWnd;
	hWndArray[Index] = hWnd;		// Save Windows Handle
	hStaticArray[Index] = hWnd;		// Save Static Caption (for ALT Search)
	hFocusArray[Index] = hWnd;		// Set TAB Search Array
	CheckBox[Index] = TRUE;			// Button Has Own Caption
	Index++;
//	Add Cancel Button
	hWnd = CreateWindowEx(
	0,								// Extra style
	TEXT("BUTTON"),					// Class Name
	TEXT("∑¨›æŸ"),			// No CaptionÅ@by uminchu
//	TEXT("Cancel"),			// No Caption
	WS_DISABLED | WS_CHILD | WS_GROUP | WS_TABSTOP,	// Window style
	//215 * dwXsize / 256,
	210 * dwXsize / 256,	// [iwad]
	113 * dwYsize / 128 + dwWinTop,
	//30 * dwXsize / 256,
	40 * dwXsize / 256,	// [iwad]
	14 * dwYsize / 128,
	hParWnd,						// Parent is Main Window
	(HMENU )IDCANCEL,				// Window Uni ID
	hInst,							// This program's Inst
	NULL);							// No Extra Args
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG )OyajiPushButtonProc);// Set Hook Handler Adrs
	hCancelWnd = hWnd;
	hWndArray[Index] = hWnd;		// Save Windows Handle
	hStaticArray[Index] = hWnd;		// Save Static Caption (for ALT Search)
	hFocusArray[Index] = hWnd;		// Set TAB Search Array
	CheckBox[Index] = TRUE;			// Button Has Own Caption
	Index++;
//	Save Total Count
	WindowCount = Index;	// Save Max Window Count
//	Create Full Screen Note
	hWnd = CreateWindowEx(
	0,								// Extra style
	TEXT("EDIT"),					// Class Name
	NULL,			// No Caption
	WS_DISABLED | WS_CHILD | ES_AUTOHSCROLL | ES_AUTOVSCROLL
				| WS_GROUP | WS_BORDER | ES_MULTILINE | WS_TABSTOP,	// Window style
	0,
	9 * dwYsize / 128 + dwWinTop,
	210 * dwXsize / 256,
	118 * dwYsize / 128,
	hParWnd,						// Parent is Main Window
	(HMENU )Index,					// Window Uni ID
	hInst,							// This program's Inst
	NULL);							// No Extra Args
	if(lpEdit == NULL)	// Not Set Yet?
		lpEdit = (WNDPROC )GetWindowLong(hWnd, GWL_WNDPROC);// Store Edit Handler Adrs
	SendMessage(hWnd, EM_SETLIMITTEXT, (WPARAM )fptr->MaxLen, 0);
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG )OyajiMultiEditProc);	// Set Hook Handler Adrs
	hFullWnd = hWnd;				// Save Windows Handle
//	Create Full Screen Title
	hTitleWnd = CreateWindowEx(
	0,								// Extra style
	TEXT("STATIC"),					// Class Name
	NULL,							// No Caption
	WS_DISABLED | WS_CHILD | SS_CENTER | SS_NOPREFIX,// Window style
	0,
	dwWinTop,
	220 * dwXsize / 256,
	9 * dwYsize / 128,
	hParWnd,						// Parent is Main Window
	(HMENU )(Index + 1),					// Window Uni ID
	hInst,							// This program's Inst
	NULL);							// No Extra Args
}
//
//	Enable Input Windows
//
void EnableInputWindows(void)
{
DWORD	i;
	for(i = 0; i < WindowCount; i++)
	{
		if(hWndArray[i])
		{
			EnableWindow(hWndArray[i], TRUE);
		}
	}
	if(dwFullMode)
	{
		EnableWindow(hFullWnd, TRUE);
		EnableWindow(hTitleWnd, TRUE);
		EnableWindow(hOkWnd, TRUE);
		EnableWindow(hCancelWnd, TRUE);
	}
}
//
//	Show Input Windows
//
void ShowInputWindows(void)
{
DWORD	i;
	if(dwFullMode)
	{
		ShowWindow(hFullWnd, SW_SHOW);
		ShowWindow(hTitleWnd, SW_SHOW);
		ShowWindow(hOkWnd, SW_SHOW);
		ShowWindow(hCancelWnd, SW_SHOW);
	}
	else
	{
		for(i = 0; i < WindowCount; i++)
		{
			if(hWndArray[i])
			{
				ShowWindow(hWndArray[i], SW_SHOW);
			}
		}
	}
}
//
//	Hide Input Windows
//
void HideInputWindows(void)
{
DWORD	i;
	if(dwFullMode)
	{
		ShowWindow(hFullWnd, SW_HIDE);
		EnableWindow(hFullWnd, FALSE);
		ShowWindow(hTitleWnd, SW_HIDE);
		EnableWindow(hTitleWnd, FALSE);
		ShowWindow(hOkWnd, SW_HIDE);
		EnableWindow(hOkWnd, FALSE);
		ShowWindow(hCancelWnd, SW_HIDE);
		EnableWindow(hCancelWnd, FALSE);
	}
	for(i = 0; i < WindowCount; i++)
	{
		if(hWndArray[i])
		{
			if(dwFullMode == 0)
				ShowWindow(hWndArray[i], SW_HIDE);
			EnableWindow(hWndArray[i], FALSE);
		}
	}
}
//
//	Destroy Input Windows
//
void DestroyInputWindows(void)
{
DWORD	i;
	for(i = 0; i < WindowCount; i++)
	{
		if(hWndArray[i])	// Defined?
			DestroyWindow(hWndArray[i]);
		hWndArray[i] = NULL;
	}
	WindowCount = 0;	// Clear Total Count
	FocusSave = NULL;
	if(hFullWnd)	// Defined?
		DestroyWindow(hFullWnd);
	hFullWnd = NULL;
	if(hTitleWnd)	// Defined?
		DestroyWindow(hTitleWnd);
	hTitleWnd = NULL;
}

//
//	Restore Last Focus
//
void RestoreFocus(void)
{
	if(FocusSave)
		SetFocus(FocusSave);
}
//
//	Set Focus to Top of Box
//
static void SetTopFocus(void)
{
	if(dwFullMode)
		FocusSave = hFullWnd;
	else
		FocusSave = GetNextFocus(NULL, 0);// Search Top Focus Window
	SetFocus(FocusSave);
	if(dwFullMode == 0)
		SendMessage(FocusSave, EM_SETSEL, 0, -1);// Select All Text
}
//
//	Get Next Focus
//
HWND GetNextFocus(HWND CurWnd, DWORD Dir)
{
DWORD	i, j;
	if(dwFullMode)
	{
		if(Dir)	// Reverse
		{
			if(CurWnd == hFullWnd)
				return hCancelWnd;
			else if(CurWnd == hCancelWnd)
				return hOkWnd;
			else return hFullWnd;
		}
		else
		{
			if(CurWnd == hFullWnd)
				return hOkWnd;
			else if(CurWnd == hOkWnd)
				return hCancelWnd;
			else return hFullWnd;
		}
	
	}
	if(Dir == 2)	// Search Key is Static?
	{
		for(i = 0; i < WindowCount; i++)
			if(hStaticArray[i] == CurWnd)
				break;
		if(i == WindowCount)
			return CurWnd;
		j = i;
		Dir = 0;
		goto ForSer;
	}
	if(CurWnd == NULL)
		for(i = 0; i < WindowCount; i++)
			if(hFocusArray[i])
				return hFocusArray[i];
	for(i = 0; i < WindowCount; i++)
		if(hFocusArray[i] == CurWnd)
			break;
	if(i == WindowCount)
		return CurWnd;
	j = i;	// Save Start Pos
	switch(Dir)	// Reverse Search
	{
	case 0:	// Normal Search
ForSer:
		if(j != WindowCount - 1)	// Not End?
			for(i = j + 1; i < WindowCount; i++)
				if(hFocusArray[i])
					return hFocusArray[i];
		for(i = 0; i < j; i++)
			if(hFocusArray[i])
				return hFocusArray[i];
		break;
	case 1:	// Reverse Search
		if(j)	// Not First?
			for(i = j - 1; i ; i--)
				if(hFocusArray[i])
					return hFocusArray[i];
		for(i = WindowCount - 1; i > j; i--)
			if(hFocusArray[i])
				return hFocusArray[i];
	}
	return CurWnd;
}
//
//	Setup Input Dialog Datas
//
void SetupInputDlg(void)
{
	MultiChange = FALSE;	// Clear Multi Change Flag
	SetupCurrentPos();
	if(dwGenID == 0)	// Blank Line?
		SetupDlgNew();
	else	// Record Already Exist(Edit Mode)
		SetupDlgFile();
}
//
//	Setup Dialog Data(New Record)
//
void SetupDlgNew(void)
{
DWORD	Min;
DWORD	Length;
	dwDlgMode = FALSE;	// New Insert Mode
	Length = (dwPrivate || dwPrivateMode)? dwPDefLength: dwDefLength;
	Min = (dwDlgStartTime & 63) + (Length & 63);
	if(Min >= 60)
	{
		dwDlgEndTime = (dwDlgStartTime & 0x7c0) + (Length & 0x7c0) + 64 + (Min - 60);
	}
	else
	{
		dwDlgEndTime = (dwDlgStartTime & 0x7c0) + (Length & 0x7c0) + Min;
	}
	if(dwDlgEndTime > GetTime(dwFinalTime, 0))	// Over 31:00?
		dwDlgEndTime = GetTime(dwFinalTime, 0);	// Set 31:00
	dwOrgDate = dwDlgDate;	// Get Current Date
	dwDlgConsDay = 1;	// 1 Day Schedule
	if(dwPrivate || dwPrivateMode)
	{
		dwDlgLeadTime = dwPDefLeadTime;	// Lead Time 5 Min
		dwDlgWeekView = dwPDefWeek;	// Week View On
		dwDlgDayView = dwPDefDay;		// Day View On
		dwDlgGrayView = dwPDefGray;		// Day View On
		dwDlgMonthView = dwPDefMonth;	// Month View On
		dwDlgEnable = dwPDefEnable;	// Alarm Disenable
	}
	else
	{
		dwDlgLeadTime = dwDefLeadTime;	// Lead Time 5 Min
		dwDlgWeekView = dwDefWeek;	// Week View On
		dwDlgDayView = dwDefDay;		// Day View On
		dwDlgGrayView = dwDefGray;		// Day View On
		dwDlgMonthView = dwDefMonth;	// Month View On
		dwDlgEnable = dwDefEnable;	// Alarm Disenable
	}
	if(dwPrivateMode)
		dwDlgPrivate = !dwPrivate;	// Flip Dialog Mode
	else
		dwDlgPrivate = dwPrivate;	// Set Current Diary Mode
	dwDlgToDo = dwToDoMode;	// Set ToDo Flag
	if(dwToDoMode)
	{
		dwDlgWeekView = dwTDefWeek;	// Week View On
		dwDlgDayView = dwTDefDay;		// Day View On
		dwDlgGrayView = dwTDefGray;		// Day View On
		dwDlgMonthView = dwTDefMonth;	// Month View On
	}
	lpDlgRepeat[0] = 0;	// Repeat None
	if(dwFullMode)
		strcpy(lpDlgTitle, "Memo");
	else
		strcpy(lpDlgTitle, "");
	strcpy(lpDlgLocation, "");
	lpDlgNote[0] = 0;
	lpDlgNote[1] = 0;
	strcpy(&lpDlgNote[2], "");
	SetupDlgSub();
}
//
//	Setup Dialog Data(Old Record)
//
void SetupDlgFile(void)
{
	dwDlgMode = TRUE;	// Replace Mode
	dwEditDate = dwDlgDate;	// Save Edit Date
	OyajiSeekGetRecord(&MainWork);
	if(dwDlgConsDay != 1 || lpDlgRepeat[0])
		MultiChange = TRUE;	// Set Multi Change Flag
	if(dwToDoMode)
		dwDlgToDo = TRUE;
	if(dwPrivateMode)
		dwDlgPrivate = !dwDlgPrivate;// Flip Diary Mode
//	Init Repeat Dialog Defaults
	SetupDlgSub();
}

//
//	Setup Dialog Items from Work
//
void SetupDlgSub()
{
FIELD	*fptr;
TCHAR	*String = (TCHAR *)LocalAlloc(LMEM_FIXED, (4096 + 1) * sizeof(TCHAR));
TCHAR	TempStr[256];
TCHAR	TitleStr[256];
HWND	hWnd;
DWORD	Index = 0;
DWORD	i;
	dwOldOrgDate = dwOrgDate;
	dwOldTime = dwDlgStartTime;
	dwOldCons = dwDlgConsDay;	// Save Dlg Data before Change
	dwOldAllDay = dwAllDay;
	dwOldGenID = dwGenID;
	for(i = 0; i < 14; i++)
		lpOldRepeat[i] = lpDlgRepeat[i];
//	Init Repeat Dialog Default
	if(lpDlgRepeat[0])	// Repeat Record Field Exists?
	{
		dwRepeatMode = lpDlgRepeat[1];	// Set Repeat Mode
		dwRepeatFreq = lpDlgRepeat[2];
		dwRepeatStartDate = lpDlgRepeat[4] * 256 + lpDlgRepeat[3];
		dwRepeatEndDate = lpDlgRepeat[6] * 256 + lpDlgRepeat[5];
		dwRepeatDay = lpDlgRepeat[7];
		dwRepeatByPos = lpDlgRepeat[8];
		dwRepeatDate = lpDlgRepeat[10] * 256 + lpDlgRepeat[9];
		dwRepeatWeek = lpDlgRepeat[11];
		dwRepeatMonth = lpDlgRepeat[13] * 256 + lpDlgRepeat[12];
	}
	else
	{
		dwRepeatMode = 0;	// Repeat None
		dwRepeatFreq = 1;	// Repeat Every 1 Day,Week,Month,Year
		dwRepeatStartDate = dwOrgDate;					// Repeat Start From Today
		dwRepeatEndDate = dwRepeatStartDate + 512;	// Range 1 Year
		dwRepeatDay = DwordToBit(GetCurrentWeekDay() + 1);	//	Get WeekDay (Mon-Sun)
		dwRepeatWeek = DwordToBit((GetCurrentDay(0) - 1) / 7 + 1);// Get Current Week
		dwRepeatMonth = DwordToBit(GetCurrentMonth(0));	// Current Month
		dwRepeatDate = dwOrgDate & 511;					// Get Month & Day Only
		dwRepeatByPos = 0;	// Day Number Mode
	}
	for(fptr = MainWork.FieldList; fptr->Attr; fptr++)
	{
		if(fptr->Width == 0)	// Blank Field?
			continue;		// No Disp
		if(fptr->Attr > FLREPEAT)
			continue;
		hWnd = hWndArray[Index];	// Get Control Handler
		switch(fptr->Attr)
		{
		case FLBYTE:
			wsprintf(String, TEXT("%d"), *(unsigned char *)fptr->Ptr);
			goto StrSet;
		case FLCHECK:	// Check Box
		case FLRADIO:	// Radio Button
			SendMessage(hWnd, BM_SETCHECK, *(DWORD *)fptr->Ptr, 0);
			if((fptr + 1)->Attr == FLSTATIC)
				fptr++;
			break;
		case FLSHORT:
			wsprintf(String, TEXT("%d"), *(unsigned short *)fptr->Ptr);
			goto StrSet;
		case FLDATE:
			_tcscpy(String, GetDateString(*(unsigned short *)fptr->Ptr));
			goto StrSet;
		case FLTIME:
			_tcscpy(String, GetTimeString(*(unsigned short *)fptr->Ptr));
			goto StrSet;
		case FLDWORD:
			wsprintf(String, TEXT("%d"), *(DWORD *)fptr->Ptr);
			goto StrSet;
		case FLREPEAT:
			if(*(char *)fptr->Ptr == 0)
			{
				_tcscpy(String, TEXT("None"));
				dwRepeatMode = 0;// Repeat Off
			}
			else
			{
				dwRepeatMode = *((char *)fptr->Ptr + 1);
				_tcscpy(String, RepeatName[dwRepeatMode]);
			}
			goto StrSet;
		case FLSTRING:
		case FLDROPDOWN:
		case FLLISTBOX:
			GetUniCode((char *)fptr->Ptr, String);
			goto StrSet;
		case FLMULTIEDIT:
			GetUniCode((char *)fptr->Ptr + 2, String);
			if(dwFullMode)	// Full Screen Mode?
			{	// Set Note Text to Full Screen Multi Edit Window
				SendMessage(hFullWnd, WM_SETTEXT, 0, (LPARAM)String);
				break;
			}
StrSet:
			SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)String);
			break;
		}
		Index++;
	}
	if(dwAllDay == 0)	// All Day Scedule?
	{
		SendMessage(hWndArray[3], WM_SETTEXT, 0, (LPARAM)TEXT("None"));
		SendMessage(hWndArray[5], WM_SETTEXT, 0, (LPARAM)TEXT("None"));
		EnableWindow(hWndArray[8], FALSE);	// Gray Enable Check Box
		EnableWindow(hWndArray[10], FALSE);	// Gray Enable Check Box
	}
	else
	{
		SendMessage(hWndArray[3], WM_SETTEXT, 0, (LPARAM)GetTimeString(dwDlgStartTime));
		SendMessage(hWndArray[5], WM_SETTEXT, 0, (LPARAM)GetTimeString(dwDlgEndTime));
		EnableWindow(hWndArray[8], TRUE);
		EnableWindow(hWndArray[10], TRUE);	// Gray Enable Check Box
	}
	LocalFree(String);
	if(dwFullMode)	// Set Title Static Text
	{
		if(dwDateFormat)
			wsprintf(TempStr, TEXT("%s %d, %2.2d "), 
			ShortMonthStr[((dwDlgDate / 32) & 15) - 1],
			dwDlgDate & 31,
			(dwDlgDate / 512 + 1950) % 100);
		else
			wsprintf(TempStr, TEXT("%d/%d/%d "), 
			(dwDlgDate / 512 + 1950),
			(dwDlgDate / 32) & 15,
			dwDlgDate & 31);
		if(dwAllDay)	// One Shot?
		{
			_tcscat(TempStr, GetTimeString(dwDlgStartTime));
			_tcscat(TempStr, TEXT(" "));
		}
		GetUniCode(lpDlgTitle, TitleStr);
		_tcscat(TempStr, TitleStr);
		SendMessage(hTitleWnd, WM_SETTEXT, 0, (LPARAM )TempStr);
	}
}
//
//	Convert Num to Simple Time
//
LPCTSTR GetSimpleTimeString(DWORD t)
{
static TCHAR TimeBuf[16];
	wsprintf(TimeBuf, TEXT("%d:%2.2d"), t / 64, t % 64);
	return TimeBuf;
}
//
//	Convert Num to Time Str
//
LPCTSTR GetTimeString(DWORD t)
{
static TCHAR TimeBuf[16];
	if(dwHourMode)	// 24H?
	{
		wsprintf(TimeBuf, TEXT("%d:%2.2d"), t / 64, t % 64);
	}
	else
	{
		if(t < 832)
			wsprintf(TimeBuf, TEXT("%d:%2.2dam"), t / 64, t % 64);
		else if(t < 1600)
			wsprintf(TimeBuf, TEXT("%d:%2.2dpm"), (t - 768)/ 64, t % 64);
		else
			wsprintf(TimeBuf, TEXT("%d:%2.2dmd"), (t - 1536) / 64, t % 64);
	}
	return TimeBuf;
}
//
//	Convert Num to Date Str
//
LPCTSTR GetDateString(DWORD Date)
{
static TCHAR DateBuf[16];
	if(Date / 512)	// Conv Year/Month/Day
		wsprintf(DateBuf, TEXT("%d/%d/%d"),
			Date / 512 + 1950, (Date / 32) & 15, Date & 31);
	else if(Date / 32)	// Month & Date
		wsprintf(DateBuf, TEXT("%d/%d"), (Date / 32) & 15, Date & 31);
	else
		wsprintf(DateBuf, TEXT("%d"), Date & 31);
	return DateBuf;
}
//
//	Covert Dialog Item to Buffer
//
DWORD ConvertInputText(void)
{
TCHAR	NumBuf[16];
TCHAR	DateBuf[12];
TCHAR	TimeBuf[8];
TCHAR	*String;
DWORD	TextLen;
FIELD	*fptr;
HWND	hWnd;
DWORD	Index = 0;
DWORD	Date, i;
//	Convert Dialog String to Work Data
	for(fptr = MainWork.FieldList; fptr->Attr; fptr++)
	{
		if(fptr->Width == 0)	// Blank Field?
			continue;		// No Disp
		if(fptr->Attr > FLREPEAT)
			continue;
		hWnd = hWndArray[Index];	// Get Control Handler
		switch(fptr->Attr)
		{
//	Convert Numbers
		case FLCHECK:
		case FLRADIO:
			*(char *)fptr->Ptr = (char )SendMessage(hWnd, BM_GETCHECK, 0, 0);
			if((fptr + 1)->Attr == FLSTATIC)
				fptr++;
			break;
		case FLBYTE:
			SendMessage(hWnd, WM_GETTEXT, sizeof(NumBuf)/sizeof(TCHAR), (LPARAM )NumBuf);
			*(char *)fptr->Ptr = (char )_ttoi(NumBuf);
			break;
		case FLSHORT:
			SendMessage(hWnd, WM_GETTEXT, sizeof(NumBuf)/sizeof(TCHAR), (LPARAM )NumBuf);
			*(short *)fptr->Ptr = (short )_ttoi(NumBuf);
			break;
		case FLDWORD:
			SendMessage(hWnd, WM_GETTEXT, sizeof(NumBuf)/sizeof(TCHAR), (LPARAM )NumBuf);
			*(DWORD *)fptr->Ptr = _ttoi(NumBuf);
			break;
		case FLDATE:
			SendMessage(hWnd, WM_GETTEXT, sizeof(DateBuf)/sizeof(TCHAR), (LPARAM )DateBuf);
			if(ConvDateToDword(&Date, DateBuf) == FALSE)
				return 3;	// Date Error
			if((Date / 512) == 0 || ((Date / 32) & 15) == 0)
				return 3;
			*(DWORD *)fptr->Ptr = Date;
			break;
		case FLTIME:
			SendMessage(hWnd, WM_GETTEXT, sizeof(TimeBuf)/sizeof(TCHAR), (LPARAM )TimeBuf);
			if(_tcscmp(TimeBuf, TEXT("None")) == 0)
				dwAllDay = 0;	// AllDay Mode
			else
			{
			DWORD	TempTime;
				dwAllDay = 1;	// One Shot Mode
				if(ConvTimeToDword(&TempTime, TimeBuf) == FALSE)
					return 2;	// Date Error
				if(TempTime > GetTime(31, 0))
					return 2;
				*(DWORD *)fptr->Ptr = TempTime;
			}
			break;
		case FLSTRING:
			SendMessage(hWnd, WM_GETTEXT, sizeof(TitleBuf)/sizeof(TCHAR), (LPARAM )TitleBuf);
			GetSJIS(TitleBuf, (char *)fptr->Ptr, fptr->MaxLen);
			break;
		case FLMULTIEDIT:
			String = (TCHAR *)LocalAlloc(LMEM_FIXED, (fptr->MaxLen + 1) * sizeof(TCHAR));
			if(dwFullMode)	// Full Screen Mode?
				SendMessage(hFullWnd, WM_GETTEXT, fptr->MaxLen, (LPARAM )String);
			else
				SendMessage(hWnd, WM_GETTEXT, fptr->MaxLen, (LPARAM )String);
			TextLen = GetSJIS(String, (char *)fptr->Ptr + 2, fptr->MaxLen);
			*(char *)fptr->Ptr =  LOBYTE(TextLen);	// Set Len Low
			*((char *)fptr->Ptr + 1) = HIBYTE(TextLen);// Set Len High
			LocalFree(String);	// Free Memory
		}
		Index++;
	}
	if(dwRepeatMode)
	{	// Set Repeat Data
		lpDlgRepeat[0] = 13;	// Set Length
		lpDlgRepeat[1] = LOBYTE(dwRepeatMode);	// Save Mode
		lpDlgRepeat[2] = LOBYTE(dwRepeatFreq);	// Save Frequency
		lpDlgRepeat[3] = LOBYTE(dwRepeatStartDate);	// Save Satrt Date
		lpDlgRepeat[4] = HIBYTE(dwRepeatStartDate);	// Save Satrt Date
		lpDlgRepeat[5] = LOBYTE(dwRepeatEndDate);	// Save End Date
		lpDlgRepeat[6] = HIBYTE(dwRepeatEndDate);	// Save End Date
		lpDlgRepeat[7] = LOBYTE(dwRepeatDay);	// Save Day of Week
		lpDlgRepeat[8] = (char )dwRepeatByPos;
		lpDlgRepeat[9] = LOBYTE(dwRepeatDate);
		lpDlgRepeat[10] = HIBYTE(dwRepeatDate);
		lpDlgRepeat[11] = (char )dwRepeatWeek;
		lpDlgRepeat[12] = LOBYTE(dwRepeatMonth);
		lpDlgRepeat[13] = HIBYTE(dwRepeatMonth);
	}
	else
		lpDlgRepeat[0] = 0;	// Set Length
	if(strlen(lpDlgTitle) == 0)
		return 1;	// Title Error (NULL Title)
	for(i = 0; lpDlgTitle[i]; i++)
		if(lpDlgTitle[i] != TEXT(' '))
			break;
	if(i == strlen(lpDlgTitle))
		return 1;	// Only Space
	if(dwDlgEndTime < dwDlgStartTime)	// Strange End Time?
		dwDlgEndTime = dwDlgStartTime;	// Set End Time = Start Time
	if(dwDlgStartTime > 2112 || dwDlgEndTime > 2112)	// Over 9:00md
		return 2;
	if(dwDlgConsDay == 0)	// If Not Specifyed
		dwDlgConsDay = 1;	// Force 1 Day
	dwDlgDate = dwOrgDate;
	if(dwDlgLeadTime > 240)
		dwDlgLeadTime = 240;
//	Set Current Time
	if(dwAllDay == 0)
		CurrentTime = MAXDWORD;
	else
		CurrentTime = ConvHalfHour(dwDlgStartTime);
	return 0;	// All data is OK
}
//
//	Convert Time String to DWORD
//
BOOL ConvTimeToDword(LPDWORD Result, LPTSTR Str)
{
LPTSTR	HourPtr, MinPtr;
DWORD	Shift = 0;
DWORD	APFlag = 0;
DWORD	MinVal = 0;	// Clear
DWORD	HourVal;
	if(_tcslen(Str) == 0)
		return FALSE;
	if(_tcschr(Str, TEXT('A')) || _tcschr(Str, TEXT('a')))
		APFlag = 1;	// Set Am Pm Flag
	else if(_tcschr(Str, TEXT('P')) || _tcschr(Str, TEXT('p')))
		{Shift = 12; APFlag = 1;}
	else if(_tcschr(Str, TEXT('d')) || _tcschr(Str, TEXT('D')))
		{Shift = 24; APFlag = 1;}
	HourPtr = _tcstok(Str, TEXT(":\0"));
	if(HourPtr == 0)
		return FALSE;
	MinPtr = _tcstok(NULL, TEXT("apmAPM\0"));
	if(MinPtr)
	{
		MinVal = _ttoi(MinPtr);
		if(*MinPtr >= TEXT('0') && *MinPtr <= TEXT('5') && _istdigit(*(MinPtr + 1)) == 0)// 1.3p Format (Not 1.03p)
			MinVal *= 10;	// Conv 1.3p to 1:30pm
		if(MinVal > 60)
			return FALSE;
	}
	HourVal = _ttoi(HourPtr);
	if(dwHourMode == 0 && APFlag == 0 && HourVal < 9)
		Shift = 12;
	if(HourVal < 13)
		HourVal += Shift;
	if(HourVal > 31)
		return FALSE;
	*Result = GetTime(HourVal, MinVal);
	return TRUE;
}
//
//	Convert Date String to DWORD
//
BOOL ConvDateToDword(LPDWORD Result, LPTSTR Str)
{
LPTSTR	YearPtr, MonthPtr, DayPtr;
DWORD	YearVal = 1950, MonthVal = 1, DayVal = 1;	// Clear
	if(_tcslen(Str) == 0)
		return FALSE;
	YearPtr = _tcstok(Str, TEXT("/\0"));
	MonthPtr = _tcstok(NULL, TEXT("/\0"));
	DayPtr = _tcstok(NULL, TEXT("/\0"));
	if(MonthPtr == NULL)	// No Year & Month Specified
	{
		if(YearPtr)	// It's Month
			YearVal = _ttoi(YearPtr);
		if(YearVal == 0 || YearVal > 31)
			return FALSE;
		*Result = YearVal;
		return TRUE;
	}
	if(DayPtr == NULL)	// No Year Specified
	{
		if(YearPtr)	// It's Month
			YearVal = _ttoi(YearPtr);
		if(YearVal == 0 || YearVal > 12)
			return FALSE;
		if(MonthPtr)
			MonthVal = _ttoi(MonthPtr);
		if(MonthVal == 0 || MonthVal > 31)
			return FALSE;
		if(MonthVal > GetFinalDay(YearVal * 32))
			return FALSE;	// Check Final Day
		*Result = YearVal * 32 + MonthVal;
		return TRUE;
	}
	if(YearPtr)
		YearVal = _ttoi(YearPtr);
	if(YearVal < 1951 || YearVal > 2077)
		return FALSE;
	if(MonthPtr)
		MonthVal = _ttoi(MonthPtr);
	if(MonthVal == 0 || MonthVal > 12)
		return FALSE;
	if(DayPtr)
		DayVal = _ttoi(DayPtr);
	if(DayVal == 0 || DayVal > 31)
		return FALSE;
	if(DayVal > GetFinalDay(GetDate(YearVal, MonthVal, 0)))
		return FALSE;	// Check Final Day
	*Result = GetDate(YearVal, MonthVal, DayVal);
	return TRUE;
}
//
//	Set Date String
//
void SetDateString(LPCTSTR str)
{
	SendMessage(hWndArray[15], WM_SETTEXT, 0, (LPARAM )str);
}
//
//	Input Screen Key Proc
//
void InputKeyProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
DWORD	ErrorCode;
DWORD	DlgResult;
DWORD	AddMode;
	switch(LOWORD(wParam))
	{
	case TEXT('\r'):
		ErrorCode = ConvertInputText();	// Convert and Check
		if(ErrorCode)
		{
			MessageBeep(MB_ICONASTERISK);
			MessageBox(hWnd, InputErrorMsg[ErrorCode - 1],
//			TEXT("Input Error"), MB_OK);
			TEXT("ì¸óÕ¥◊∞"), MB_OK);	// by uminchu
			break;
		}
		AddMode = TRUE;
		if(dwMultiConfirm && MultiChange)
		{
//			DlgResult = MessageBox(hWnd, TEXT("Change All Item?"), TEXT("Repeat Change"),MB_YESNOCANCEL);
			DlgResult = MessageBox(hWnd, TEXT("ëSÇƒÇÃ±≤√—ÇïœçXÇµÇ‹Ç∑Ç©?"), TEXT("Repeat Change"),MB_YESNOCANCEL);	// by uminchu
			if(DlgResult == IDCANCEL)
				break;
			else if(DlgResult == IDNO)
				AddMode = FALSE;
		}
		if(AddMode == FALSE)	// 1 Rec Chanege?
		{
			dwDlgConsDay = 1;
			dwDlgDate = dwEditDate;	// Set Top Date
			dwOldOrgDate = dwDlgDate;
			dwOrgDate = dwDlgDate;
			lpDlgRepeat[0] = 0;
		}
		AddRecord(AddMode, TRUE);// All Change Mode
		JumpDate(dwOrgDate);// Jump 1st Day
	case TEXT('\033'):	// Change to Day Screen(ESC)
		HideInputWindows();
		SetFocus(hMainWnd);
		DispMode = OldDispMode;
		switch(DispMode)
		{
		case 0:	// Back to Month
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_HALFYEAR, 0);
			break;
		case 1:	// Back to Month
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_MONTH, 0);
			break;
		case 2:	// Back to Week
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_WEEK, 0);
			break;
		case 3:	// Back to Month
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_DAY, 0);
			break;
		case 5:	// Back to Month
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_TODO, 0);
			break;
		}
		break;
	case TEXT('\t'):
		FocusSave = GetNextFocus(FocusSave, (GetKeyState(VK_SHIFT) < 0));
		SetFocus(FocusSave);
		SendMessage(FocusSave, EM_SETSEL, 0, -1);// Select All Text
		return;
	case TEXT('R'):	// Repeat Set
		if(DialogBox(hInst, MAKEINTRESOURCE(IDD_REPEATDLG), hMainWnd, (DLGPROC )RepeatDlgProc) == FALSE)
			return;// Cancel
		SendMessage(hWndArray[19], WM_SETTEXT, 0, (LPARAM )RepeatName[dwRepeatMode]);// Set Button Caption
		return;
	default:
		SendMessage(GetFocus(), WM_CHAR, wParam, lParam);
	}
}
//
//	Input Screen Proc (Focus & ALT + Char & USER)
//
BOOL InputScreenProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
DWORD	i, TempTime = dwDlgStartTime;
TCHAR	String[256], *StrPtr, TempCh;
	switch(message)
	{
	case WM_SETFOCUS:
		RestoreFocus();
		break;
	case WM_SYSCHAR:
		if(wParam < TEXT('a') || wParam > TEXT('z'))
			break;
		if(dwFullMode)
			break;
		for(i = 0; i < WindowCount; i++)
		{
			if(hStaticArray[i])
			{
				SendMessage(hStaticArray[i],
					WM_GETTEXT, sizeof(String), (LPARAM )String);// Set Static Text
				if(StrPtr = _tcschr(String, TEXT('&')))
				{
					TempCh = *++StrPtr;
					if(TempCh < TEXT('Z'))
						TempCh += TEXT(' ');
					if((TCHAR )wParam == TempCh)
					{
						if(CheckBox[i])	// Check Box?
							FocusSave = hWndArray[i];	// Set Itself
						else	// Normal Edit Box
							FocusSave = GetNextFocus(hWndArray[i], 2);// 2 Means Search Near
						SetFocus(FocusSave);
						SendMessage(FocusSave, EM_SETSEL, 0, -1);// Select All Text
						return TRUE;
					}
				}
			}
		}
		break;
	case WM_TIMEMSG:
		if(lParam == (LPARAM )hWndArray[3])	// Message from Start Time?
		{
			if(wParam == TEXT('-') || wParam == TEXT('+'))	// + -
				SendMessage(hWndArray[5], WM_CHAR, wParam, 0);	// Send +/- to EndTime Box
			else if(wParam == TEXT('E'))	// Erase? then Erase EndTime
				SendMessage(hWndArray[5], WM_SETTEXT, 0, (LPARAM )TEXT(""));	// Send End Time Box
			else if(wParam == TEXT('S'))	// Set? then Set EndTime
			{	// Set 1Hour Len to EndTime
				SendMessage(hWndArray[5], WM_GETTEXT, sizeof(String),(LPARAM )String);	// Send End Time Box
				if(String[0] == TEXT('\0'))	// EndTime is Blank?
				{
					SendMessage(hWndArray[3], WM_GETTEXT, sizeof(String), (LPARAM )String);	// Send End Time Box
					ConvTimeToDword(&TempTime, String);	// Get Start Time
					if(TempTime < GetTime(31, 0))	// Limit Time?
						TempTime = AddTime(TempTime, dwDefLength);	// 1 Hour Length
					if(TempTime >= GetTime(31,0))	// Over?
						TempTime = GetTime(31, 0);	// Force Set Limit
					SendMessage(hWndArray[5], WM_SETTEXT, 0, (LPARAM )GetTimeString(TempTime));
				}
			}
		}
		if(wParam == TEXT('V'))	// Clear Both Text (Vanish)
		{
			SendMessage(hWndArray[3], WM_SETTEXT, 0, (LPARAM )TEXT(""));	// Send End Time Box
			SendMessage(hWndArray[5], WM_SETTEXT, 0, (LPARAM )TEXT(""));	// Send End Time Box
			EnableWindow(hWndArray[8], TRUE);	// Enable Check Box
			EnableWindow(hWndArray[10], TRUE);	// Enable LeadTime
			dwAllDay = 1;
		}
		else if(wParam == TEXT('I'))	// Initialize Time Text
		{
			if(dwAllDay == 0)	// All Day Mode?
				SendMessage((HWND )lParam, WM_SETTEXT, 0, (LPARAM )TEXT("None"));
			else	// Set Original Time String
				SendMessage((HWND )lParam, WM_SETTEXT, 0,
					(LPARAM )GetTimeString(
					(lParam == (LPARAM )hWndArray[3])? dwDlgStartTime: dwDlgEndTime));
		}			
		else if(wParam == TEXT('D') && dwAllDay == 1)	// Delete?
		{
			dwAllDay = 0;	// Set All Day Mode
			SendMessage((lParam == (LPARAM )hWndArray[3])?hWndArray[5]:hWndArray[3],
							WM_KEYDOWN, VK_DELETE, 0);	// Send to End Time
			EnableWindow(hWndArray[8], FALSE);	// Gray Enable Check Box
			EnableWindow(hWndArray[10], FALSE);	// Gray LeadTime
		}
		else if(wParam == TEXT('R'))	// Restore?
		{	// Restore Noraml Time for both Box
			dwAllDay = 1;
			SendMessage(hWndArray[3], WM_SETTEXT, 0, (LPARAM )GetTimeString(dwDlgStartTime));
			SendMessage(hWndArray[5], WM_SETTEXT, 0, (LPARAM )GetTimeString(dwDlgEndTime));
			EnableWindow(hWndArray[8], TRUE);	// Enable Check Box
			EnableWindow(hWndArray[10], TRUE);	// Enable LeadTime
		}
	}
	return FALSE;
}
//
//	Edit Class Hook
//
BOOL CALLBACK OyajiEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_LBUTTONDOWN:
		FocusSave = hWnd;
		break;
	case WM_SYSCHAR:
		return SendMessage(GetParent(hWnd), message, wParam, lParam);
	case WM_CHAR:
		if(wParam == TEXT('\t') || wParam == TEXT('\r'))
			return SendMessage(GetParent(hWnd), message, wParam, lParam);
		else if(wParam == TEXT('\033'))	// ESC?
		{
			if(CallWindowProc(lpEdit, hWnd, EM_GETMODIFY, 0, 0))
			{
				CallWindowProc(lpEdit, hWnd, EM_UNDO, 0, 0);
				CallWindowProc(lpEdit, hWnd, EM_SETMODIFY, 0, 0);
				return CallWindowProc(lpEdit, hWnd, EM_SETSEL, 0, -1);// Select All Text
			}
			return SendMessage(GetParent(hWnd), message, wParam, lParam);// Pass to Parent
		}
		break;
	}
	return CallWindowProc(lpEdit, hWnd, message, wParam, lParam);
}
//
//	Time Edit Class Hook (Up & Down)
//
BOOL CALLBACK OyajiTimeEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
TCHAR	TimeBuf[16];
DWORD	TimeVal;
static	int FocusFlag;
static	int NoneFlag;
	switch(message)
	{
	case WM_SETFOCUS:	// First Focus
		FocusFlag = 1;
		break;
	case WM_LBUTTONDOWN:
		FocusSave = hWnd;
		break;
	case WM_SYSCHAR:
		return SendMessage(GetParent(hWnd), message, wParam, lParam);
	case WM_KEYDOWN:
		CallWindowProc(lpEdit, hWnd, WM_GETTEXT, 15, (LPARAM )TimeBuf);
		NoneFlag = (_tcscmp(TimeBuf, TEXT("None")) == 0);
		if(wParam == VK_DELETE || (wParam == VK_BACK && FocusFlag))
		{
			if(NoneFlag)
				SendMessage(GetParent(hWnd), WM_TIMEMSG, TEXT('R'), (LPARAM )hWnd);// Tell Parent Restore
			else
			{
				SendMessage(GetParent(hWnd), WM_TIMEMSG, TEXT('D'), (LPARAM )hWnd);// Tell Parent Delete
				CallWindowProc(lpEdit, hWnd, WM_SETTEXT, 0, (LPARAM )TEXT("None"));
				FocusFlag = 1;	// Reset Toutch Flag if None Again
			}
			SendMessage(hWnd, EM_SETSEL, 0, -1);// Select All Text
			return TRUE;
		}
		if(NoneFlag)
			return TRUE;
		if(wParam == VK_DOWN)
			goto IncVal;
		else if(wParam == VK_UP)
			goto DecVal;
		else if(wParam == VK_LEFT || wParam == VK_RIGHT)
			FocusFlag = 0;
		break;
	case WM_CHAR:
		CallWindowProc(lpEdit, hWnd, WM_GETTEXT, 15, (LPARAM )TimeBuf);
		NoneFlag = (_tcscmp(TimeBuf, TEXT("None")) == 0);
#ifdef	_WIN32_WCE
		if(wParam >= TEXT('ÇO') && wParam <= TEXT('ÇX'))
			wParam = wParam - TEXT('ÇO') + TEXT('0');
		else if(wParam >= TEXT('Ç`') && wParam <= TEXT('Çy'))
			wParam = wParam - TEXT('Ç`') + TEXT('A');
		else if(wParam >= TEXT('ÇÅ') && wParam <= TEXT('Çö'))
			wParam = wParam - TEXT('ÇÅ') + TEXT('a');
		else if(wParam == TEXT('ÅD'))
			wParam = TEXT('.');
		else if(wParam == TEXT('ÅF'))
			wParam = TEXT(':');
		else if(wParam == TEXT('Ç†'))
			wParam = TEXT('a');
#endif
		if(wParam == TEXT('\t') || wParam == TEXT('\r'))	// Enter or TAB
		{
			if(NoneFlag == 0)	// Normal Mode?
			{
				if(ConvTimeToDword(&TimeVal, TimeBuf))
					CallWindowProc(lpEdit, hWnd, WM_SETTEXT, 0, (LPARAM )GetTimeString(TimeVal));
				else
					SendMessage(GetParent(hWnd), WM_TIMEMSG, TEXT('I'), (LPARAM )hWnd);// Init Time Text
				SendMessage(GetParent(hWnd), WM_TIMEMSG, TEXT('S'), (LPARAM )hWnd);// Set Message
				CallWindowProc(lpEdit, hWnd, EM_SETSEL, 0, -1);
			}
			return SendMessage(GetParent(hWnd), message, wParam, lParam);
		}
		else if(wParam == TEXT('\033'))	// ESC?
		{
			if(CallWindowProc(lpEdit, hWnd, EM_GETMODIFY, 0, 0))
			{
				CallWindowProc(lpEdit, hWnd, EM_UNDO, 0, 0);
				CallWindowProc(lpEdit, hWnd, EM_SETMODIFY, 0, 0);
				return CallWindowProc(lpEdit, hWnd, EM_SETSEL, 0, -1);// Select All Text
			}
			return SendMessage(GetParent(hWnd), message, wParam, lParam);// Pass to Parent
		}
		else if(FocusFlag == 0 && (wParam == TEXT('a') || wParam == TEXT('A')))
		{
			CallWindowProc(lpEdit, hWnd, message, TEXT('a'), lParam);
			wParam = TEXT('m');
			break;
		}
		else if(FocusFlag == 0 && (wParam == TEXT('p') || wParam == TEXT('P')))
		{
			CallWindowProc(lpEdit, hWnd, message, TEXT('p'), lParam);
			wParam = TEXT('m');
			break;
		}
		else if(FocusFlag == 0 && (wParam == TEXT('m') || wParam == TEXT('M')))
		{
			CallWindowProc(lpEdit, hWnd, message, TEXT('m'), lParam);
			wParam = TEXT('d');
			break;
		}
		else if(wParam == TEXT('+') || wParam == TEXT(';'))
		{
IncVal:
			if(NoneFlag)
				return TRUE;
			if(ConvTimeToDword(&TimeVal, TimeBuf))
			{
				if(TimeVal < GetTime(31, 0))	// High Limit of Time 36:00am
				{
					if((TimeVal & 63) >= (60 - dwHourStep))
						TimeVal += (64 - (60 - dwHourStep));
					else
						TimeVal += dwHourStep;
				}
				if(TimeVal > GetTime(31, 0))
					TimeVal = GetTime(31, 0);
				CallWindowProc(lpEdit, hWnd, WM_SETTEXT, 0, (LPARAM )GetTimeString(TimeVal));
				SendMessage(GetParent(hWnd), WM_TIMEMSG, TEXT('+'), (LPARAM )hWnd);// Tell Parent +
			}
			SendMessage(hWnd, EM_SETSEL, 0, -1);// Select All Text
			return TRUE;
		}
		else if(wParam == TEXT('-'))
		{
DecVal:
			if(NoneFlag)
				return TRUE;
			if(ConvTimeToDword(&TimeVal, TimeBuf))// Right Format ?
			{
				if(TimeVal >= dwHourStep)
				{
					if((TimeVal & 63) < dwHourStep)
						TimeVal -= (64 - (60 - dwHourStep));
					else
						TimeVal -= dwHourStep;
				}
				CallWindowProc(lpEdit, hWnd, WM_SETTEXT, 0, (LPARAM )GetTimeString(TimeVal));
				SendMessage(GetParent(hWnd), WM_TIMEMSG, TEXT('-'), (LPARAM )hWnd);// Tell Parent -
			}
			SendMessage(hWnd, EM_SETSEL, 0, -1);// Select All Text
			return TRUE;
		}
		else if(NoneFlag == 0 && (wParam == TEXT('.') || wParam == TEXT(':')))
		{
			if(FocusFlag)
				return TRUE;
			wParam = TEXT(':');
			break;
		}
		if(NoneFlag)
		{
			if(wParam >=TEXT('0') && wParam <= TEXT('9'))
			{
				NoneFlag = 0;
				FocusFlag = 0;
				SendMessage(GetParent(hWnd), WM_TIMEMSG, TEXT('V'), (LPARAM )hWnd);// Tell Parent Erase
				return CallWindowProc(lpEdit, hWnd, message, wParam, lParam);
			}
			return TRUE;
		}
		if(wParam == VK_BACK && FocusFlag)
			return TRUE;
		else if(wParam < TEXT(' '))
			break;
		else if(wParam <TEXT('0') || wParam > TEXT('9'))
			return TRUE;
		if(FocusFlag)	// First Typed Char?
			SendMessage(GetParent(hWnd), WM_TIMEMSG, TEXT('E'), (LPARAM )hWnd);// Tell Parent Erase
		FocusFlag = 0;
	}
	return CallWindowProc(lpEdit, hWnd, message, wParam, lParam);
}
//
//	Date Edit Class Hook (Up & Down)
//
BOOL CALLBACK OyajiDateEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
TCHAR	DateBuf[16];
DWORD	DateVal, i;
	switch(message)
	{
	case WM_LBUTTONDOWN:
		FocusSave = hWnd;
		break;
	case WM_SYSCHAR:
		return SendMessage(GetParent(hWnd), message, wParam, lParam);
	case WM_KEYDOWN:
		if(GetKeyState(VK_SHIFT) < 0)
		{
			if(wParam == VK_DOWN)
				goto IncWeek;
			else if(wParam == VK_UP)
				goto DecWeek;
		}
		else
		{
			if(wParam == VK_DOWN)
				goto IncVal;
			else if(wParam == VK_UP)
				goto DecVal;
		}
		break;
	case WM_CHAR:
		if(wParam == TEXT('\t') || wParam == TEXT('\r'))
			return SendMessage(GetParent(hWnd), message, wParam, lParam);// Pass to Parent
		else if(wParam == TEXT('\033'))	// ESC?
		{
			if(CallWindowProc(lpEdit, hWnd, EM_GETMODIFY, 0, 0))
			{
				CallWindowProc(lpEdit, hWnd, EM_UNDO, 0, 0);
				CallWindowProc(lpEdit, hWnd, EM_SETMODIFY, 0, 0);
				return CallWindowProc(lpEdit, hWnd, EM_SETSEL, 0, -1);// Select All Text
			}
			return SendMessage(GetParent(hWnd), message, wParam, lParam);// Pass to Parent
		}
		else if(wParam == TEXT(';'))
		{
IncVal:
			CallWindowProc(lpEdit, hWnd, WM_GETTEXT, 15, (LPARAM )DateBuf);
			if(ConvDateToDword(&DateVal, DateBuf))	// Write Format?
			{
				GetNextDate(&DateVal);	// Get Next Date
				CallWindowProc(lpEdit, hWnd, WM_SETTEXT, 0, (LPARAM )GetDateString(DateVal));
			}
			return SendMessage(hWnd, EM_SETSEL, 0, -1);// Select All Text
		}
		else if(wParam == TEXT('-'))
		{
DecVal:
			CallWindowProc(lpEdit, hWnd, WM_GETTEXT, 15, (LPARAM )DateBuf);
			if(ConvDateToDword(&DateVal, DateBuf))	// Write Fromat?
			{
				GetPrevDate(&DateVal);	// Get Prev Date
				CallWindowProc(lpEdit, hWnd, WM_SETTEXT, 0, (LPARAM )GetDateString(DateVal));
			}
			return SendMessage(hWnd, EM_SETSEL, 0, -1);// Select All Text
		}
		else if(wParam == TEXT('+'))
		{
IncWeek:
			CallWindowProc(lpEdit, hWnd, WM_GETTEXT, 15, (LPARAM )DateBuf);
			if(ConvDateToDword(&DateVal, DateBuf))	// Write Format?
			{
				for(i = 0; i < 7; i++)
					GetNextDate(&DateVal);	// Get Next Date
				CallWindowProc(lpEdit, hWnd, WM_SETTEXT, 0, (LPARAM )GetDateString(DateVal));
			}
			return SendMessage(hWnd, EM_SETSEL, 0, -1);// Select All Text
		}
		else if(wParam == TEXT('='))
		{
DecWeek:
			CallWindowProc(lpEdit, hWnd, WM_GETTEXT, 15, (LPARAM )DateBuf);
			if(ConvDateToDword(&DateVal, DateBuf))	// Write Fromat?
			{
				for(i = 0; i < 7; i++)
					GetPrevDate(&DateVal);	// Get Prev Date
				CallWindowProc(lpEdit, hWnd, WM_SETTEXT, 0, (LPARAM )GetDateString(DateVal));
			}
			return SendMessage(hWnd, EM_SETSEL, 0, -1);// Select All Text
		}
#ifdef	_WIN32_WCE
		else if(wParam >= TEXT('ÇO') && wParam <= TEXT('ÇX'))
		{
			wParam = wParam - TEXT('ÇO') + TEXT('0');
			break;
		}
#endif
		else if(wParam == TEXT('/') || wParam < TEXT(' '))
			break;
		else if(wParam <TEXT('0') || wParam > TEXT('9'))
			return TRUE;
	}
	return CallWindowProc(lpEdit, hWnd, message, wParam, lParam);// Default Message
}
//
//	Date Edit Class Hook (Up & Down)
//
BOOL CALLBACK OyajiRepeatDateEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
TCHAR	DateBuf[16];
DWORD	DateVal;
	switch(message)
	{
	case WM_KEYDOWN:
		if(wParam == VK_DOWN)
			goto IncVal;
		else if(wParam == VK_UP)
			goto DecVal;
		break;
	case WM_CHAR:
		if(wParam == TEXT('+') || wParam == TEXT(';'))
		{
IncVal:
			CallWindowProc(lpEdit, hWnd, WM_GETTEXT, 15, (LPARAM )DateBuf);
			if(ConvDateToDword(&DateVal, DateBuf))	// Write Format?
			{
				GetNextDate(&DateVal);	// Get Next Date
				CallWindowProc(lpEdit, hWnd, WM_SETTEXT, 0, (LPARAM )GetDateString(DateVal));
			}
			return SendMessage(hWnd, EM_SETSEL, 0, -1);// Select All Text
		}
		else if(wParam == TEXT('-'))
		{
DecVal:
			CallWindowProc(lpEdit, hWnd, WM_GETTEXT, 15, (LPARAM )DateBuf);
			if(ConvDateToDword(&DateVal, DateBuf))	// Write Fromat?
			{
				GetPrevDate(&DateVal);	// Get Prev Date
				CallWindowProc(lpEdit, hWnd, WM_SETTEXT, 0, (LPARAM )GetDateString(DateVal));
			}
			SendMessage(hWnd, EM_SETSEL, 0, -1);// Select All Text
			return TRUE;
		}
#ifdef	_WIN32_WCE
		else if(wParam >= TEXT('ÇO') && wParam <= TEXT('ÇX'))
		{
			wParam = wParam - TEXT('ÇO') + TEXT('0');
			break;
		}
#endif
		else if(wParam == TEXT('/') || wParam < TEXT(' '))
			break;
		else if(wParam <TEXT('0') || wParam > TEXT('9'))
			return TRUE;
	}
	return CallWindowProc(lpEdit, hWnd, message, wParam, lParam);// Default Message
}
//
//	Byte Edit Class Hook (Up & Down Suport)
//
BOOL CALLBACK OyajiByteEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
TCHAR	DayBuf[16];
DWORD	DayVal;
	switch(message)
	{
	case WM_LBUTTONDOWN:
		FocusSave = hWnd;
		break;
	case WM_SYSCHAR:
		return SendMessage(GetParent(hWnd), message, wParam, lParam);
	case WM_KEYDOWN:
		if(wParam == VK_DOWN)
			goto IncVal;
		else if(wParam == VK_UP)
			goto DecVal;
		break;
	case WM_CHAR:
		if(wParam == TEXT('\t') || wParam == TEXT('\r'))
			return SendMessage(GetParent(hWnd), message, wParam, lParam);// Pass TAB & ESC
		else if(wParam == TEXT('\033'))	// ESC?
		{
			if(CallWindowProc(lpEdit, hWnd, EM_GETMODIFY, 0, 0))
			{
				CallWindowProc(lpEdit, hWnd, EM_UNDO, 0, 0);
				CallWindowProc(lpEdit, hWnd, EM_SETMODIFY, 0, 0);
				return CallWindowProc(lpEdit, hWnd, EM_SETSEL, 0, -1);// Select All Text
			}
			return SendMessage(GetParent(hWnd), message, wParam, lParam);// Pass to Parent
		}
		else if(wParam == TEXT('+') || wParam == TEXT(';'))
		{
IncVal:
			CallWindowProc(lpEdit, hWnd, WM_GETTEXT, 15, (LPARAM )DayBuf);
			DayVal = _ttoi(DayBuf);
			wsprintf(DayBuf, TEXT("%d"), ++DayVal);
			CallWindowProc(lpEdit, hWnd, WM_SETTEXT, 0, (LPARAM )DayBuf);
			return SendMessage(hWnd, EM_SETSEL, 0, -1);// Select All Text
		}
		else if(wParam == TEXT('-'))
		{
DecVal:
			CallWindowProc(lpEdit, hWnd, WM_GETTEXT, 15, (LPARAM )DayBuf);
			DayVal = _ttoi(DayBuf);
			if(DayVal > 0)
				DayVal--;
			wsprintf(DayBuf, TEXT("%d"), DayVal);
			CallWindowProc(lpEdit, hWnd, WM_SETTEXT, 0, (LPARAM )DayBuf);
			return SendMessage(hWnd, EM_SETSEL, 0, -1);// Select All Text
		}
#ifdef	_WIN32_WCE
		else if(wParam >= TEXT('ÇO') && wParam <= TEXT('ÇX'))
		{
			wParam = wParam - TEXT('ÇO') + TEXT('0');
			break;
		}
#endif
		else if(wParam < TEXT(' '))
			break;
		else if(wParam <TEXT('0') || wParam > TEXT('9'))
			return TRUE;
	}
	return CallWindowProc(lpEdit, hWnd, message, wParam, lParam);
}
//
//	Byte Edit Class Hook (Up & Down Suport)
//
BOOL CALLBACK OyajiDayByteEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
TCHAR	DayBuf[16];
DWORD	DayVal;
	switch(message)
	{
	case WM_KEYDOWN:
		if(wParam == VK_DOWN)
			goto IncVal;
		else if(wParam == VK_UP)
			goto DecVal;
		break;
	case WM_CHAR:
		if(wParam == TEXT('+') || wParam == TEXT(';'))
		{
IncVal:
			CallWindowProc(lpEdit, hWnd, WM_GETTEXT, 15, (LPARAM )DayBuf);
			DayVal = _ttoi(DayBuf);
			if(DayVal < 31)
				DayVal++;
			wsprintf(DayBuf, TEXT("%d"), DayVal);
			CallWindowProc(lpEdit, hWnd, WM_SETTEXT, 0, (LPARAM )DayBuf);
			return SendMessage(hWnd, EM_SETSEL, 0, -1);// Select All Text
		}
		else if(wParam == TEXT('-'))
		{
DecVal:
			CallWindowProc(lpEdit, hWnd, WM_GETTEXT, 15, (LPARAM )DayBuf);
			DayVal = _ttoi(DayBuf);
			if(DayVal > 1)
				DayVal--;
			wsprintf(DayBuf, TEXT("%d"), DayVal);
			CallWindowProc(lpEdit, hWnd, WM_SETTEXT, 0, (LPARAM )DayBuf);
			return SendMessage(hWnd, EM_SETSEL, 0, -1);// Select All Text
		}
#ifdef	_WIN32_WCE
		else if(wParam >= TEXT('ÇO') && wParam <= TEXT('ÇX'))
		{
			wParam = wParam - TEXT('ÇO') + TEXT('0');
			break;
		}
#endif
		else if(wParam < TEXT(' '))
			break;
		else if(wParam <TEXT('0') || wParam > TEXT('9'))
			return TRUE;
	}
	return CallWindowProc(lpEdit, hWnd, message, wParam, lParam);
}
//
//	Multi Edit Class Hook
//
BOOL CALLBACK OyajiMultiEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_LBUTTONDOWN:
		FocusSave = hWnd;	// Move Current Focus
		break;
	case WM_SYSCHAR:	// Pass ALT + ?
		return SendMessage(GetParent(hWnd), message, wParam, lParam);
	case WM_CHAR:	// Pass TAB & ESC
		if(wParam == TEXT('\t'))
			return SendMessage(GetParent(hWnd), message, wParam, lParam);
		else if(wParam == TEXT('\033'))	// ESC?
		{
			if(CallWindowProc(lpEdit, hWnd, EM_GETMODIFY, 0, 0))
			{
				CallWindowProc(lpEdit, hWnd, EM_UNDO, 0, 0);
				CallWindowProc(lpEdit, hWnd, EM_SETMODIFY, 0, 0);
				return CallWindowProc(lpEdit, hWnd, EM_SETSEL, 0, -1);// Select All Text
			}
			return SendMessage(GetParent(hWnd), message, wParam, lParam);// Pass to Parent
		}
		break;
	}
	return CallWindowProc(lpEdit, hWnd, message, wParam, lParam);// Default Proc
}
//
//	Button Class Hook
//
BOOL CALLBACK OyajiButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_LBUTTONDOWN:
		FocusSave = hWnd;
		break;
	case WM_SYSCHAR:	// Pass ALT + ?
		return SendMessage(GetParent(hWnd), message, wParam, lParam);
	case WM_CHAR:	// Pass TAB & ESC
		if(wParam == TEXT('\t') || wParam == TEXT('\r') || wParam == TEXT('\033'))
			return SendMessage(GetParent(hWnd), message, wParam, lParam);
		break;
	}
	return CallWindowProc(lpButton, hWnd, message, wParam, lParam);// Default Proc
}
//
//	Push Button Class Hook
//
BOOL CALLBACK OyajiPushButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
DWORD	WinID;
	switch(message)
	{
	case WM_LBUTTONDOWN:
IDCheck:
		WinID = GetWindowLong(hWnd, GWL_ID);
		CallWindowProc(lpButton, hWnd, message, wParam, lParam);// Show Push Down
		if(WinID == IDOK)
			return SendMessage(GetParent(hWnd), WM_CHAR, TEXT('\r'), 0);// Enter
		else if(WinID == IDCANCEL)
			return SendMessage(GetParent(hWnd), WM_CHAR, TEXT('\033'), 0);// ESC
		break;
	case WM_SYSCHAR:	// Pass ALT + ?
		return SendMessage(GetParent(hWnd), message, wParam, lParam);
	case WM_CHAR:	// Pass TAB & ESC
		if(wParam == TEXT('\r'))	// Enter Key?
			goto IDCheck;			// Same as Left Click
		if(wParam == TEXT('\t') || wParam == TEXT('\033'))
			return SendMessage(GetParent(hWnd), message, wParam, lParam);
		break;
	}
	return CallWindowProc(lpButton, hWnd, message, wParam, lParam);// Default Proc
}
//
//	Repeat Button Class Hook
//
BOOL CALLBACK OyajiRepeatButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_LBUTTONDOWN:
Pressed:
		CallWindowProc(lpButton, hWnd, WM_LBUTTONDOWN, wParam, lParam);// Default Proc
		return SendMessage(GetParent(hWnd), WM_CHAR, TEXT('R'), 0);// Send 'R'
	case WM_SYSCHAR:	// Pass ALT + ?
		return SendMessage(GetParent(hWnd), message, wParam, lParam);
	case WM_CHAR:	// Pass TAB & ESC
		if(wParam == TEXT('\r'))	// Enter Key?
			goto Pressed;			// Same as Left Click
		if(wParam == TEXT('\t') || wParam == TEXT('\033'))
			return SendMessage(GetParent(hWnd), message, wParam, lParam);
		break;
	}
	return CallWindowProc(lpButton, hWnd, message, wParam, lParam);// Default Proc
}
//
//	Insert Full Screen Text from File
//
void InsertTextFromFile(HWND hWnd)
{
OPENFILENAME FN;
TCHAR	FullFileName[MAX_PATH], FileName[MAX_PATH];	// File Name Buffer
HANDLE	hFile;
DWORD	ByteRead;
LPTSTR	Buffer;
char	*CharBuffer;
	if(DispMode != 4 || dwFullMode == 0 || hFullWnd == NULL)// Only Input Full Screen
		return;	// Error
	if(FileOpenDialog(hWnd, 0, &FN,
			FullFileName,
			FileName,
			szCurentDir,
			TEXT("Text File\0*.txt\0All Files\0*.*\0\0"),
			TEXT("txt")) == FALSE)	// Get Insert File Name
		return;						// Cancel Pressed
	if((hFile = CreateFile(FullFileName,
		GENERIC_READ,	// Read & Write
		FILE_SHARE_READ,	// No Share
		NULL,	// No Seculity Attributes
		OPEN_EXISTING,	// If File not exists then Fail
		FILE_ATTRIBUTE_NORMAL |		// Normal File
		FILE_FLAG_WRITE_THROUGH,	// Write to Cash Immidiate
		NULL)) == INVALID_HANDLE_VALUE)
		return;
	Buffer = (LPTSTR )LocalAlloc(LMEM_FIXED, 4096 + 2);
	if(Buffer == NULL)
		return;
	CharBuffer = (char *)LocalAlloc(LMEM_FIXED, (4096 + 2));
	if(CharBuffer == NULL)
	{
		LocalFree(Buffer);
		return;
	}
	ReadFile(hFile, CharBuffer, 4096, &ByteRead, 0);	// Read All Record Body 
	if(ByteRead != 4096)
		*(CharBuffer + ByteRead) = '\0';	// Set Terminator
	else
		*(CharBuffer + 4096) = '\0';		// Set Terminator
	CloseHandle(hFile);	// Close File
	GetUniCode(CharBuffer, Buffer);
	SendMessage(hFullWnd, WM_SETTEXT, 0, (LPARAM )Buffer);
	LocalFree(Buffer);
	LocalFree(CharBuffer);
	return;
}
//
//	Put Full Screen Text to File
//
void WriteTextToFile(HWND hWnd)
{
OPENFILENAME FN;
TCHAR	FullFileName[MAX_PATH], FileName[MAX_PATH];	// File Name Buffer
HANDLE	hFile;
DWORD	ByteRead, Length;
LPTSTR	Buffer;
char	*CharBuffer;
	if(DispMode != 4 || dwFullMode == 0 || hFullWnd == NULL)// Only Input Full Screen
		return;	// Error
	if(FileOpenDialog(hWnd, 1, &FN,
			FullFileName,
			FileName,
			szCurentDir,
			TEXT("Text File\0*.txt\0All Files\0*.*\0\0"),
			TEXT("txt")) == FALSE)				// Get Open File Name
		return;						// Cancel Pressed
	if((hFile = CreateFile(FullFileName,
		GENERIC_WRITE,	// Write Only
		0,	// No Share
		NULL,	// No Seculity Attributes
		CREATE_ALWAYS,		// If File exists then Delete
		FILE_ATTRIBUTE_NORMAL,	// Normal File
		NULL)) == INVALID_HANDLE_VALUE)
		return;
	Buffer = (LPTSTR )LocalAlloc(LMEM_FIXED, (4096 + 2) * sizeof(TCHAR));
	if(Buffer == NULL)
		return;
	CharBuffer = (char *)LocalAlloc(LMEM_FIXED, (4096 + 2));
	if(CharBuffer == NULL)
	{
		LocalFree(Buffer);
		return;
	}
	SendMessage(hFullWnd, WM_GETTEXT, 4096, (LPARAM )Buffer);
	*(Buffer + 4096) = TEXT('\0');
	GetSJIS(Buffer, CharBuffer, 4096);
	Length = strlen(CharBuffer);
	WriteFile(hFile, CharBuffer, Length + 1, &ByteRead, 0);	// Read All Record Body 
	CloseHandle(hFile);	// Close File
	LocalFree(Buffer);
	LocalFree(CharBuffer);
	return;
}

