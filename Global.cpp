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
#include	"struct.h"
//
//	Defines
//
#define	countof(x)	(sizeof(x)/(sizeof((x)[0])))
#define	MAXDAYLINE	32
#define	MAXDAYBUF	128	// Max Day Display Buffer Number
#define	WM_TIMEMSG	(WM_USER + 1)
#define	WM_ALARM	(WM_USER + 2)
//
//	Global Vars
//
LPCTSTR		szHelpFileName =	TEXT("APPHELP.HLP");// Help FileName
LPCTSTR		szRegName =			TEXT("Software\\Oyajin\\Appointment");
LPCTSTR		szWinName =			TEXT("OyajinApo");	// Class name
LPCTSTR		szVerNum =			TEXT("0.41");		// szVerNum Number TEXT	[iwad]
//LPCTSTR		szVerNum =			TEXT("0.99f-u01");		// szVerNum Number TEXT	by uminchu
//LPCTSTR		szVerNum =			TEXT("0.99f");		// szVerNum Number TEXT

LPCTSTR		DayName[] = {	// DayName;
				TEXT("日"), TEXT("月"), TEXT("火"),TEXT("水"), TEXT("木"), TEXT("金"),TEXT("土")}; // by uminchu
//				TEXT("Sunday"), TEXT("Monday"), TEXT("Tuesday"),TEXT("Wednesday"), TEXT("Thursday"), TEXT("Friday"),TEXT("Saturday")};

LPCTSTR		MonthName[] = {	// DayName;
				TEXT("１月"), TEXT("２月"), TEXT("３月"),TEXT("４月"), TEXT("５月"), TEXT("６月"),TEXT("７月"), TEXT("８月"), TEXT("９月"),TEXT("10月"), TEXT("11月"), TEXT("12月")} ;	// by uminchu
//				TEXT("January"), TEXT("February"), TEXT("March"),TEXT("April"), TEXT("May"), TEXT("June"),TEXT("July"), TEXT("August"), TEXT("September"),TEXT("October"), TEXT("November"), TEXT("December")};

//	Handlers
HINSTANCE	hInst;		// Global Instance copy
HWND		hMainWnd;	// Main Window handle
HWND		hCmdBar;	// CommandBar handle (for CE)
HWND		hTB;		// ToolBar Handle
HWND		hWndB;		// Stand Alone Menu Button Window Handle
HWND		hWndX;		// Stand Alone Exit Button Window Handle
HWND		FocusSave;	// Focus Save Buffer for Input Screen
HACCEL		hAccl;		// Accelerator handle
HDC			hMemDC;		// Memory Device Context
HFONT		hSmallFont, hMidFont, hBigFont;	// Font Handles
HFONT		hSystemFont;// System Font Handler
HBITMAP		hBitBuf;	//	BitMap Handle
HBRUSH		hBrush;		// Brush Handle
HBRUSH		hGrayBrush;	// Gray Brush for Holyday Handle
HPEN		hPen;		// Pen Handle
HANDLE		BackHandle;	//	BackGround Thread Handle
//	For File
char		FilePassWord[8], DataBaseName[64];
//	For Regitry Save
DWORD		dwStartTop = 40, dwStartLeft = 40;	// Start Window Pos
DWORD		dwStartHight = 240, dwStartWidth = 480;// Start Window Size
DWORD		dwMenuMode;	// Menu Display Mode
DWORD		dwPrivate;	// Dialy Mode Flag
DWORD		dwHourMode = 1;	// Hour Disp Mode (0:12H 1:24H) [iwad] デフォルトは24時間表記
DWORD		dwAppDispMode;	// Disp Mode (0:Normal 1: App Only)
DWORD		dwAlarmMode;	// Alarm Mode (0:Beep 2:Wave)
DWORD		dwMulti = 1;	// Multi Thread Flag
DWORD		dwFileMode = 1;	// File Mode (0:File 1:Memory)
DWORD		dwGraphMode = 1;// Graph Display Mode (1:On)
DWORD		dwDefEnable;	// Default Enable
DWORD		dwDefWeek = 1;	// Default Week View
DWORD		dwDefMonth = 1;	// Default Month View
DWORD		dwDefDay = 1;	// Default Day View
DWORD		dwDefGray;		// Default Gray View
DWORD		dwHolyDay;		// WeekDay of Gray
DWORD		dwStartTime = 8;	// Day Display Start Time
DWORD		dwFinalTime = 23;	// Day Display Final Time
DWORD		dwDefLeadTime = 15;	// Default Lead Time
DWORD		dwDefLength = 64;		// Default Duration
//	Private Defaults
DWORD		dwPDefEnable;	// Default Enable
DWORD		dwPDefWeek = 1;	// Default Week View
DWORD		dwPDefMonth = 1;// Default Month View
DWORD		dwPDefDay = 1;	// Default Day View
DWORD		dwPDefGray;		// Default Gray View
DWORD		dwPHolyDay;		// WeekDay of Gray
DWORD		dwPDefLeadTime = 15;	// Default Lead Time
DWORD		dwPDefLength = 64;	// Default Duration
//	ToDo Defaults
DWORD		dwTDefWeek;		// Default Week View
DWORD		dwTDefMonth = 1;// Default Month View
DWORD		dwTDefDay;		// Default Day View
DWORD		dwTDefGray;		// Default Gray View
DWORD		dwToDoDay;		// Disp ToDo on Day Screen
DWORD		dwToDoHalf;		// Disp ToDo on Half Year Screen
//
DWORD		dwDispStep;			// Display Step (0:1:00 1:0:30)
DWORD		dwUnderBar = 1;	// Under Line Flag (1:On)
DWORD		dwExitMode = 1;	// ESC Exit Mode
DWORD		dwLoopCount = 3;	// Alarm Sound Loop Count
DWORD		dwVolume = 5;		// Sound Volume
DWORD		dwAlarmTime = 30;	// Alarm Dialog Display Time
DWORD		dwAlarmSnooze = 10;	// Custom Snooze Time
DWORD		dwConfirm = 1;		// Confirm When Delete
DWORD		dwMultiConfirm = 1;	// Confirm When Multi Change & Delete
//	User Defineable Vars
DWORD		dwHourStep = 15;	// Hour Inc/Dec Step
DWORD		dwTextColor;		// Text Color 
DWORD		dwLineColor;		// Text Color 
DWORD		dwBackColor;		// BackGround Color 
DWORD		dwGrayColor;	// Gray BackGround Color 
DWORD		dwTopDayLine = 3;	// Max AllDay Scedule Lines(Day Screen)
DWORD		dwTopWeekLine = 1;	// Max AllDay Scedule Lines(Week Screen)
//
DWORD		dwWeekSync = 1;		// Week Sync other Day
DWORD		dwWeekFloat = 1;	// Week Time Line floating Time
DWORD		dwDateFormat;	// English Format Flag [iwad] USA OFF
TCHAR		szExecFileName[MAX_PATH];// Current FileName Buffer
TCHAR		szDefNameL[MAX_PATH];	// [iwad] 左ソフトキー対応
TCHAR		szOpenNameL[MAX_PATH];	// [iwad] 左ソフトキー対応
TCHAR		szDefNameR[MAX_PATH];	// [iwad] 右ソフトキー対応
TCHAR		szOpenNameR[MAX_PATH];	// [iwad] 右ソフトキー対応
#ifndef	_WIN32_WCE
TCHAR		szNotifyFileName[MAX_PATH];// Current FileName Buffer
#endif
TCHAR		szCurentDir[MAX_PATH];	// Path Name Buffer		
TCHAR		szCurentFileName[MAX_PATH];// Current FileName Buffer
TCHAR		szWaveDir[MAX_PATH];	// Path Name Buffer		
TCHAR		szWaveFileName[MAX_PATH];// Current FileName Buffer
//	For Display Screen
DWORD		dwWinTop, dwWinWidth, dwWinHight;	// Window dimensions (for WM_SIZE comp)
DWORD		dwTodayYear, dwTodayMonth, dwTodayDay;// Today Date
DWORD		dwTodayHour, dwTodayMinute;	// Today Hour & Min
DWORD		dwLastDay;			// Last Day Number
DWORD		dwYear, dwMonth;	// Current Year & Month
DWORD		dwCurX, dwCurY;		// Black Box Cursors
DWORD		dwOldCurX, dwOldCurY;	// Old Cursor Pos.
DWORD		dwXsize,dwYsize;	// Client Area Dimensions
DWORD		CurrentTime;	// Current Select Time
DWORD		StartTime;		// Current Day Screen Start Time
DWORD		DispMode;		// Main Screen Display Mode
DWORD		OldDispMode;	// Last Disp Mode (For Return)
DWORD		ToDoBackMode;	// ToDo Back Mode (For Return)
DWORD		dwToDoRange = 15;	// ToDo View Range
DWORD		MonthBuf[6][42][3];	// Month Buffer
DWORD		WeekBuf[7][3];	// Week Buffer
//	For Day Screen
DWORD		DayOrgTimeBuf[MAXDAYLINE];	// Original Time List(14 Lines)
DWORD		DayOrgIDBuf[MAXDAYLINE];		// Day Seek Ptr List(14 Lines)
DWORD		DayOrgAllDayBuf[MAXDAYLINE];	// All Day Flag List
DWORD		DayOrgDateBuf[MAXDAYLINE];		// Date Buffer (For ToDo Screen)
DWORD		DayLineCount = 16;		// Day Line Count
LPTSTR		ClipBuf;	// Clip Board Buffer
LPTSTR		CsvBuf;	// CSV Buffer
//
DWORD		dwSmallFontHight, dwMidFontHight, dwSmallFontWidth;	// Small Font Metrics
DWORD		dwBigFontHight, dwMidFontWidth, dwBigFontWidth;	// Big Font Metrics
DWORD		dwDayLine;	// Day Display Screen Line Cursor Pos
DWORD		dwDispFlag = 1;	// Display Flag of Main Screen
//	For Input Dialog Box
#ifdef	_WIN32_WCE
DWORD		dwGrayBack;			// Gray Back Switch(WinCE Default Off)
#else	// Win95
DWORD		dwGrayBack = 1;		// Gray Back Switch(Win95 Default On)
#endif
DWORD		dwToDoSort;		// ToDo Time Sort Flag
DWORD		dwToDoPopUp;		// ToDo Time Sort Flag
DWORD		dwDlgStartTime, dwDlgEndTime, dwDlgDate;// Start End Time
DWORD		dwDlgEnable,dwDlgLeadTime, dwDlgConsDay;// Alarm Info
DWORD		dwDlgWeekView, dwDlgMonthView;// Check Box Info
DWORD		dwDlgDayView, dwDlgGrayView;// Check Box Info
DWORD		dwDlgToDo;	// ToDo Record Flag
DWORD		dwDlgPrivate;	// Diary Flag
DWORD		dwDlgMode;// Dialog Mode(0:New 1:Edit), Seek Ptr
DWORD		dwGenID, dwOldGenID, dwOrgDate;// Work for Multi Date Records)
DWORD		dwOldOrgDate, dwOldTime, dwOldCons;// Old Dialog Data
DWORD		dwAllDay, dwOldAllDay;	// All Day Flag
unsigned char lpOldRepeat[14];	// Old Repeat Data
DWORD		dwToDoMode;			// ToDo Mode
DWORD		dwPrivateMode;		// Diary Mode
DWORD		dwFullMode;			// Full Screen Mode Flag
//	For Repeat Dialog
DWORD		dwRepeatMode;	// Repeat Mode (0:Off 1:Daily 2:Weekly...)
DWORD		dwRepeatFreq;	// Repeat Frequency
DWORD		dwRepeatStartDate;	// Repeat Start Date
DWORD		dwRepeatEndDate;	// Repeat End Date
DWORD		dwRepeatByPos;	// By Date or By Pos Mode
DWORD		dwRepeatDate;	// Day Number or Date
DWORD		dwRepeatWeek;	// 1st 2nd 3rd Week
DWORD		dwRepeatDay;	// Repeat Day (Mon-Sun)
DWORD		dwRepeatMonth;	// Repeat Month (Jan-Dec)
DWORD		DayID[7] =	{IDC_SUN, IDC_MON, IDC_TUE, IDC_WED, IDC_THU, IDC_FRI, IDC_SAT};
TCHAR		TitleBuf[256];	// Title Buf
char		lpDlgTitle[65], lpDlgNote[3 + 4096];	// Title & Note
char		lpDlgLocation[33];			// Location & Repeat Info
unsigned char	lpDlgRepeat[14];		// Location & Repeat Info
//	For Background Add & Delete
DWORD		BackDate, BackAllDay, BackStartTime, BackGenID;
DWORD		BackPrivate, BackToDo;	// BackGround Private Flag
DWORD		BackOrgDate, BackConsDay, BackRepeat[14], BackDlgMode;
DWORD		HideGenID;	// GenID now Delete/Add
DWORD		FileLock;	// File Lock Flag
DWORD		FileResult;	// File Result of BackGround Thread
DWORD		Abort;		// Thread Abort Flag
//	For Font Settings
TCHAR		szBigFontName[LF_FULLFACESIZE];	// Large Font Name
TCHAR		szMidFontName[LF_FULLFACESIZE];	// Mid Font Name
TCHAR		szSmallFontName[LF_FULLFACESIZE];	// Small Font Name
DWORD		dwBigFontSize = 16, dwMidFontSize = 16, dwSmallFontSize = 16;// Font Size
//	For Alarm
LPTSTR		szArg;	// Arg Pointer
char		BackTitle[65];				// Alarm Title
char		BackLocation[33];			// Alarm Location
DWORD		BackEnable, BackLeadTime;	// Alarm Works
TCHAR		AlarmTitle[256];			// Alarm Dialog Text
DWORD		AlarmDate;		// Current Alarm Date
DWORD		AlarmTime;		// Current Alarm Time
BOOL		dwFindTitle = 1;		// Find Title Check Flag
BOOL		dwFindLocation = 1;	// Find Location Check Flag
BOOL		dwFindNote = 1;		// Find Note Check Flag
TCHAR		szFindText[256];	// Find Text
//	For Range Dialog
DWORD		dwCategory;	// Category Switch
DWORD		dwToDoCategory;	// ToDo Category Switch
DWORD		dwStartDate;	// After Day
DWORD		dwEndDate;	// Before Day
DWORD		RangeMode;	// Range Mode (BetWeen or Split)
//	For Week Screen
DWORD		WeekTimeBuf[7][MAXDAYLINE];	// Original Time List(14 Lines)
DWORD		WeekGenIDBuf[7][MAXDAYLINE];	// Day Seek Ptr List(14 Lines)
DWORD		WeekAllDayBuf[7][MAXDAYLINE];	// All Day Flag List
DWORD		WeekCurY;					// Week Screen Cursor Y
//	For Day Screen;
DWORD		BarBuf[96];				// Diagram Bar Buffer (0 to 31)
DWORD		BlacketBitPat[] = {0x07, 0x38, 0x1c0, 0xe00, 0x0};
//	For One Day Array
DWORD		DayStartTime[7][MAXDAYBUF];	// Start Time Buffer
DWORD		DayEndTime[7][MAXDAYBUF];	// End Time Buf
DWORD		DayGenID[7][MAXDAYBUF];		// GenID Buffer
DWORD		DayAllDay[7][MAXDAYBUF];	// AllDay Flag Buffer
TCHAR		DayTitle[7][MAXDAYBUF][64];	// Title Buf
DWORD		DayBlacket[7][MAXDAYBUF];		// Blacket Buffer 000:Blank 001:- 010:Start 011:| 100:End 101:T
DWORD		DayAlarm[MAXDAYBUF];		// Alarm Flag Array
DWORD		DayToDo[MAXDAYBUF];			// ToDo Mark Array
DWORD		ToDoDate[MAXDAYBUF];		// Date of ToDo Array
DWORD		DayAllNum[7];	// All DEay Schedule Number (Includes Blank & Non Blank)
DWORD		DayOneNum[7];	// One Shot Schedule Number
DWORD		ActualNum[7];	// Actual Record Number (Not Blank)
DWORD		StartIndex;		// Start Index of One Shot Schedule
DWORD		AllStartIndex;	// Start Index of All Day Schedule
DWORD		HideSkip;		// Hide Mode Start Line
DWORD		AllDayHight;	// All Day Scedule Lines
DWORD		DispHour;		// Display Hour
BOOL		TransDir;		// Sync Direction
TCHAR		APMark[] = {TEXT('a'), TEXT(' '), TEXT('p'), TEXT(' '), TEXT('m'), TEXT(' ')};
HANDLE		hSyncHandle;		// Data Sync Thread ID
DWORD		SyncLock;		// Sync Lock Flag
DWORD		SyncTimerCount;	// Sync Timer Count
DWORD		SyncThreadID;	// Thread Identifier
WNDPROC		lpEdit;			// Edit Class Proc Adrs
LPCTSTR	ShortDayName[] = {TEXT("日"), TEXT("月"), TEXT("火"), TEXT("水"),TEXT("木"), TEXT("金"), TEXT("土")};	// by uminchu
//LPCTSTR	ShortDayName[] = {TEXT("Sun"), TEXT("Mon"), TEXT("Tue"), TEXT("Wed"),TEXT("Thu"), TEXT("Fri"), TEXT("Sat")};

LPCTSTR	ShortMonthStr[] = {TEXT("1"), TEXT("2"), TEXT("3"), TEXT("4"),TEXT("5"), TEXT("6"), TEXT("7"), TEXT("8"),TEXT("9"), TEXT("10"), TEXT("11"), TEXT("12")}; // by uminchu
//LPCTSTR	ShortMonthStr[] = {TEXT("Jan"), TEXT("Feb"), TEXT("Mar"), TEXT("Apr"),TEXT("May"), TEXT("Jun"), TEXT("Jul"), TEXT("Aug"),TEXT("Sep"), TEXT("Oct"), TEXT("Nov"), TEXT("Dec")};

LPCTSTR	RepeatName[] = {	// Repeat Status String
	TEXT("None"), TEXT("毎日"), TEXT("毎週"), TEXT("毎月"), TEXT("毎年")};	// by iwad
//	TEXT("None"), TEXT("Daily"), TEXT("Weekly"), TEXT("Monthly"), TEXT("Yearly")};

LPCTSTR	WeekPosName[] = {
	TEXT("1st"), TEXT("2nd"), TEXT("3rd"), TEXT("4th"), TEXT("5th")};
//
//	Main Data Base Struct Table [iwad]
//
FIELD	MainField[] ={
	FLSTATIC,	0,	"内容",			2,7,30,12,	// Title String Caption
	FLSTRING,	64,	(DWORD *)lpDlgTitle,33,4,217,13,// Title String
	FLSTATIC,	0,	"開始",		2,23,30,12,	// Start Time Caption
	FLTIME,		7,	&dwDlgStartTime,	33,21,40,12,// Start Time Data
	FLSTATIC,	0,	"終了",		2,39,30,12,	// End Time Caption
	FLTIME,		7,	&dwDlgEndTime,		33,37,40,12,// End Time 2
	FLSTATIC,	0,	"場所",			2,55,30,12,	// Location Caption
	FLSTRING,	32,	(DWORD *)lpDlgLocation,33,53,136,13,// Location
	FLCHECK,	1,	&dwDlgEnable,		85,30,35,12,// Enable
	FLSTATIC,	0,	"有",			83,30,35,12,// Enable Caption
	FLSTATIC,	0,	"分前",			140,31,28,12,// Lead Caption
	FLBYTE,		2,	&dwDlgLeadTime,		118,29,21,12,// Lead Time
	FLGROUPBOX,	0,	"ｱﾗｰﾑ",				78,17,92,32,// Group Box
	FLSTATIC,	0,	"ﾒﾓ",			3,95,17,24,	// Note Caption
	FLMULTIEDIT,4096,(DWORD *)lpDlgNote,20,95,185,32,// Note
	FLSTATIC,	0,	"日付",		171,23,29,12,// Date Caption
	FLDATE,		10,	&dwOrgDate,			200,21,50,12,// Original Date
	FLSTATIC,	0,	"連続日数",		171,39,54,12,// Cons Days Caption
	FLBYTE,		2,	&dwDlgConsDay,		230,37,20,12,// Cons Days
	FLSTATIC,	0,	"繰返し",	171,55,40,12,// Repeat Status Caption
	FLREPEAT,	14,	(DWORD*)lpDlgRepeat,215,53,35,12,// Repeat Status
	FLCHECK,	1,	&dwDlgMonthView,	10,77,40,12,// Week View CheckBox
	FLSTATIC,	0,	"月",		12,77,37,12,// Week View Caption
	FLCHECK,	2,	&dwDlgWeekView,		50,77,40,12,// Month View
	FLSTATIC,	0,	"週",		49,77,37,12,// Month View Caption
	FLCHECK,	4,	&dwDlgDayView,		90,77,35,12,// Month View
	FLSTATIC,	0,	"日",		86,77,37,12,// Month View Caption
	FLCHECK,	8,	&dwDlgGrayView,		127,77,35,12,// Month View
	FLSTATIC,	0,	"色",		123,77,37,12,// Month View Caption
	FLCHECK,	16,	&dwDlgToDo,			164,77,35,12,// Month View
	FLSTATIC,	0,	"To",		160,77,37,12,// Month View Caption
	FLCHECK,	32,	&dwDlgPrivate,		200,77,40,12,// Month View
	FLSTATIC,	0,	"Pv",		200,77,37,12,// Month View Caption
	FLGROUPBOX,	0,	"表示",				3,68,249,25,// Group Box
	FLBYTE,		0,	&dwAllDay,			0, 0, 0, 0,	// All Day Scheddule Flag
	FLDATE,		0,	&dwDlgDate,			0, 0, 0, 0,	// Date (For Multi Date)
	FLDWORD,	0,	&dwGenID,			0, 0, 0, 0,	// Age ID (For Multi Date) (Key 3)
	FLENDMARK,	0,	NULL, 0,0,0,0	
};
/*
FIELD	MainField[] ={
	FLSTATIC,	0,	"Desc&ription",		5,7,35,12,	// Title String Caption
	FLSTRING,	64,	(DWORD *)lpDlgTitle,40,4,205,13,// Title String
	FLSTATIC,	0,	"Start &Time",		5,23,35,12,	// Start Time Caption
	FLTIME,		7,	&dwDlgStartTime,	40,21,35,12,// Start Time Data
	FLSTATIC,	0,	"End T&ime",		5,39,35,12,	// End Time Caption
	FLTIME,		7,	&dwDlgEndTime,		40,37,35,12,// End Time 2
	FLSTATIC,	0,	"Lo&cation",		5,55,35,12,	// Location Caption
	FLSTRING,	32,	(DWORD *)lpDlgLocation,40,53,120,13,// Location
	FLCHECK,	1,	&dwDlgEnable,		85,30,35,12,// Enable
	FLSTATIC,	0,	"Ena&bled",			85,30,35,12,// Enable Caption
	FLSTATIC,	0,	"&Lead",			120,32,20,12,// Lead Caption
	FLBYTE,		2,	&dwDlgLeadTime,		140,30,15,12,// Lead Time
	FLGROUPBOX,	0,	"Alarm",			80,17,80,32,// Group Box
	FLSTATIC,	0,	"&Note",			5,95,15,12,	// Note Caption
	FLMULTIEDIT,4096,(DWORD *)lpDlgNote,20,95,185,32,// Note
	FLSTATIC,	0,	"Start &Date",		165,23,40,12,// Date Caption
	FLDATE,		10,	&dwOrgDate,			205,21,40,12,// Original Date
	FLSTATIC,	0,	"No. Consec&utive",	165,39,65,12,// Cons Days Caption
	FLBYTE,		2,	&dwDlgConsDay,		230,37,15,12,// Cons Days
	FLSTATIC,	0,	"Repeat Status",	165,55,40,12,// Repeat Status Caption
	FLREPEAT,	14,	(DWORD*)lpDlgRepeat,215,53,30,12,// Repeat Status
	FLCHECK,	1,	&dwDlgMonthView,	10,75,40,12,// Week View CheckBox
	FLSTATIC,	0,	"&Month",			10,75,40,12,// Week View Caption
	FLCHECK,	2,	&dwDlgWeekView,		50,75,40,12,// Month View
	FLSTATIC,	0,	"&Week",			50,75,40,12,// Month View Caption
	FLCHECK,	4,	&dwDlgDayView,		90,75,35,12,// Month View
	FLSTATIC,	0,	"D&ay",				90,75,35,12,// Month View Caption
	FLCHECK,	8,	&dwDlgGrayView,		125,75,35,12,// Month View
	FLSTATIC,	0,	"&Gray",			125,75,35,12,// Month View Caption
	FLCHECK,	16,	&dwDlgToDo,			160,75,40,12,// Month View
	FLSTATIC,	0,	"T&oDo",			160,75,40,12,// Month View Caption
	FLCHECK,	32,	&dwDlgPrivate,		200,75,40,12,// Month View
	FLSTATIC,	0,	"&Private",			200,75,40,12,// Month View Caption
	FLGROUPBOX,	0,	"View",				5,66,240,25,// Group Box
	FLBYTE,		0,	&dwAllDay,			0, 0, 0, 0,	// All Day Scheddule Flag
	FLDATE,		0,	&dwDlgDate,			0, 0, 0, 0,	// Date (For Multi Date)
	FLDWORD,	0,	&dwGenID,			0, 0, 0, 0,	// Age ID (For Multi Date) (Key 3)
	FLENDMARK,	0,	NULL, 0,0,0,0	
};
*/
//
//	Back Data Base Struct Table
//
FIELD	BackField[] ={
	FLSTATIC,	0,	"内容(&R)",			5,7,35,12,	// Title String Caption
	FLSTRING,	64,(DWORD *)BackTitle,	40,4,205,13,// Title String
	FLSTATIC,	0,	"開始時刻(&T)",		5,23,35,12,	// Start Time Caption
	FLTIME,		7,	&BackStartTime,		40,21,35,12,// Start Time Data
	FLSTATIC,	0,	"終了時刻(&I)",		5,39,35,12,	// End Time Caption
	FLTIME,		7,	NULL,				40,37,35,12,// End Time 2
	FLSTATIC,	0,	"場所(&C)",			5,55,35,12,	// Location Caption
	FLSTRING,	32,(DWORD *)BackLocation,	40,53,120,13,// Location
	FLCHECK,	1,	&BackEnable,		85,30,35,12,// Enable
	FLSTATIC,	0,	"有効(&B)",			85,30,35,12,// Enable Caption
	FLSTATIC,	0,	"分前(&L)",			136,33,20,12,// Lead Caption
	FLBYTE,		2,	&BackLeadTime,		120,30,15,12,// Lead Time
	FLGROUPBOX,	0,	"ｱﾗｰﾑ",				80,17,80,32,// Group Box
	FLSTATIC,	0,	"ﾒﾓ欄(&N)",			5,95,15,24,	// Note Caption
	FLMULTIEDIT,4096,NULL,				20,95,185,32,// Note
	FLSTATIC,	0,	"日付変更(&D)",		165,23,40,12,// Date Caption
	FLDATE,		10,	NULL,				205,21,40,12,// Original Date
	FLSTATIC,	0,	"連続日数(&U)",		165,39,65,12,// Cons Days Caption
	FLBYTE,		2,	NULL,				230,37,15,12,// Cons Days
	FLSTATIC,	0,	"繰返しｽﾃｰﾀｽ(&S)",	165,55,40,12,// Repeat Status Caption
	FLREPEAT,	14,	NULL,				215,53,30,12,// Repeat Status
	FLCHECK,	1,	NULL,				10,77,40,12,// Week View CheckBox
	FLSTATIC,	0,	"月表示(&M)",		10,77,40,12,// Week View Caption
	FLCHECK,	2,	NULL,				50,77,40,12,// Month View
	FLSTATIC,	0,	"週表示(&W)",		50,77,40,12,// Month View Caption
	FLCHECK,	4,	NULL,				90,77,35,12,// Month View
	FLSTATIC,	0,	"日表示(&A)",		90,77,35,12,// Month View Caption
	FLCHECK,	8,	NULL,				127,77,35,12,// Month View
	FLSTATIC,	0,	"色付き(&G)",		127,77,35,12,// Month View Caption
	FLCHECK,	16,	&BackToDo,			164,77,35,12,// Month View
	FLSTATIC,	0,	"ToDo(&O)",			164,77,35,12,// Month View Caption
	FLCHECK,	32,	&BackPrivate,		200,77,40,12,// Month View
	FLSTATIC,	0,	"ﾌﾟﾗｲﾍﾞｰﾄ(&P)",		200,77,40,12,// Month View Caption
	FLGROUPBOX,	0,	"表示",				5,66,240,25,// Group Box
	FLBYTE,		0,	&BackAllDay,		0, 0, 0, 0,	// All Day Scheddule Flag
	FLDATE,		0,	&BackDate,			0, 0, 0, 0,	// Date (For Multi Date)
	FLDWORD,	0,	&BackGenID,			0, 0, 0, 0,	// Age ID (For Multi Date) (Key 3)
	FLENDMARK,	0,	NULL, 0,0,0,0	
};
/*
FIELD	BackField[] ={
	FLSTATIC,	0,	"Desc&ription",		5,7,35,12,	// Title String Caption
	FLSTRING,	64,(DWORD *)BackTitle,	40,4,205,13,// Title String
	FLSTATIC,	0,	"Start &Time",		5,23,35,12,	// Start Time Caption
	FLTIME,		7,	&BackStartTime,		40,21,35,12,// Start Time Data
	FLSTATIC,	0,	"End T&ime",		5,39,35,12,	// End Time Caption
	FLTIME,		7,	NULL,				40,37,35,12,// End Time 2
	FLSTATIC,	0,	"Lo&cation",		5,55,35,12,	// Location Caption
	FLSTRING,	32,(DWORD *)BackLocation,	40,53,120,13,// Location
	FLCHECK,	1,	&BackEnable,		85,30,35,12,// Enable
	FLSTATIC,	0,	"Ena&bled",			85,30,35,12,// Enable Caption
	FLSTATIC,	0,	"&Lead",			120,32,20,12,// Lead Caption
	FLBYTE,		2,	&BackLeadTime,		140,30,15,12,// Lead Time
	FLGROUPBOX,	0,	"Alarm",			80,17,80,32,// Group Box
	FLSTATIC,	0,	"&Note",			5,95,15,12,	// Note Caption
	FLMULTIEDIT,4096,NULL,				20,95,185,32,// Note
	FLSTATIC,	0,	"Start &Date",		165,23,40,12,// Date Caption
	FLDATE,		10,	NULL,				205,21,40,12,// Original Date
	FLSTATIC,	0,	"No. Consec&utive",	165,39,65,12,// Cons Days Caption
	FLBYTE,		2,	NULL,				230,37,15,12,// Cons Days
	FLSTATIC,	0,	"Repeat Status",	165,55,40,12,// Repeat Status Caption
	FLREPEAT,	14,	NULL,				215,53,30,12,// Repeat Status
	FLCHECK,	1,	NULL,				10,75,40,12,// Week View CheckBox
	FLSTATIC,	0,	"&Month",			10,75,40,12,// Week View Caption
	FLCHECK,	2,	NULL,				50,75,40,12,// Month View
	FLSTATIC,	0,	"&Week",			50,75,40,12,// Month View Caption
	FLCHECK,	4,	NULL,				90,75,35,12,// Month View
	FLSTATIC,	0,	"D&ay",				90,75,35,12,// Month View Caption
	FLCHECK,	8,	NULL,				125,75,35,12,// Month View
	FLSTATIC,	0,	"&Gray",			125,75,35,12,// Month View Caption
	FLCHECK,	16,	&BackToDo,			160,75,40,12,// Month View
	FLSTATIC,	0,	"T&oDo",			160,75,40,12,// Month View Caption
	FLCHECK,	32,	&BackPrivate,		200,75,40,12,// Month View
	FLSTATIC,	0,	"&Private",			200,75,40,12,// Month View Caption
	FLGROUPBOX,	0,	"View",				5,66,240,25,// Group Box
	FLBYTE,		0,	&BackAllDay,		0, 0, 0, 0,	// All Day Scheddule Flag
	FLDATE,		0,	&BackDate,			0, 0, 0, 0,	// Date (For Multi Date)
	FLDWORD,	0,	&BackGenID,			0, 0, 0, 0,	// Age ID (For Multi Date) (Key 3)
	FLENDMARK,	0,	NULL, 0,0,0,0	
};
*/
//	Sort Key Field Number Table (Use for Get Sort Key Type)
//	Make Sure Change OyajiGetFieldPos() Same Time!
DWORD	SortKeyFieldNumTable[] = {35, 34, 3, 36};// Sort Key Field Num (Date & Time & AllDay & GenID)
//
//	Main File Works Struct
//
FILEWORKS	MainWork = {
	NULL,				// Window Handle
	0,0,0,0,0,0,0,		// 7 Zeros for Len & Pointers & Works
	"", DataBaseName,	// PassWord & DataBaseName
	MainField,			// Field Definition List
	0,NULL,				// Sort Index Table Count & Pointer
	4, SortKeyFieldNumTable,// Sort Key Infos
	NULL, 0, 0,			// Current Record Buffer Pointer (Allocate) & Length 
	szCurentDir,szCurentFileName,	// Current File Name
	TEXT("Appointment\0*.adb\0All Files\0*.*\0\0"),	// FileName Filter
	NULL};				// File Handle
//
//	BackGround File Works
//
FILEWORKS	BackWork;
//	For ToolButtons
TBBUTTON	lpButtons[] = {
	{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
	{0, IDM_VIEW_HALFYEAR, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
	{1, IDM_VIEW_MONTH, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
	{2, IDM_VIEW_WEEK, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
	{3, IDM_VIEW_DAY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
	{4, IDM_VIEW_INPUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
	{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
	{5, IDM_VIEW_TODO, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
	{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
	{6, IDM_VIEW_PRIVATE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
	{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
	{7, IDM_EDIT_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
	{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
	{8, IDM_VIEW_MINIMIZE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0}
};
/*
LPCTSTR	szTips[] = {NULL, TEXT("Half Year Display"),TEXT("Month Display"), TEXT("Week Display"),TEXT("Day Display"), TEXT("Input Display"),
					TEXT("ToDo Display"),
					TEXT("Personal Mode"),
					TEXT("Delete Schedule"),
#ifdef	_WIN32_WCE
					TEXT("Minimize Menu")};
#else	// 95,NT
					TEXT("Minimize Window")};
#endif
*/
// by uminchu
LPCTSTR	szTips[] = {NULL, TEXT("半年表示"),TEXT("月間表示"), TEXT("週間表示"),TEXT("日表示"), TEXT("入力画面"),
					TEXT("ToDo"),
					TEXT("ﾊﾟｰｿﾅﾙﾓｰﾄﾞ"),
					TEXT("ｽｹｼﾞｭｰﾙの削除"),
#ifdef	_WIN32_WCE
					TEXT("ﾒﾆｭｰの最小化")};
#else	/* 95,NT */
					TEXT("最小化")};
#endif

//	For Registry Entry
DWORD	numButtons = countof(lpButtons);
DWORD	numTips = countof(szTips);
LPCTSTR		RegTable[][4] = {
	TEXT("WinWidth"),	(TCHAR *)2,	(TCHAR *)&dwStartWidth,	NULL,
	TEXT("WinHight"),	(TCHAR *)2,	(TCHAR *)&dwStartHight,	NULL,
	TEXT("WinTop"),		(TCHAR *)2,	(TCHAR *)&dwStartTop,	NULL,
	TEXT("WinLeft"),	(TCHAR *)2,	(TCHAR *)&dwStartLeft,	NULL,
	TEXT("MenuMode"),	(TCHAR *)1,	(TCHAR *)&dwMenuMode,	NULL,
	TEXT("TimeFormat"),	(TCHAR *)1,	(TCHAR *)&dwHourMode,	NULL,
	TEXT("DisplayMode"),(TCHAR *)1,	(TCHAR *)&dwAppDispMode,NULL,
	TEXT("AlarmMode"),	(TCHAR *)1,	(TCHAR *)&dwAlarmMode,	NULL,
	TEXT("GraphMode"),	(TCHAR *)1,	(TCHAR *)&dwGraphMode,	NULL,
	TEXT("LoopCount"),	(TCHAR *)1,	(TCHAR *)&dwLoopCount,	NULL,
	TEXT("FileMode"),	(TCHAR *)1,	(TCHAR *)&dwFileMode,	NULL,	// データ破壊問題対策、キャッシュ使用を強制的に有効にする(iwad 2007.01.22修正)
	TEXT("Background"),	(TCHAR *)1,	(TCHAR *)&dwMulti,		NULL,
	TEXT("UnderLine"),	(TCHAR *)1,	(TCHAR *)&dwUnderBar,	NULL,
	TEXT("DisplayStep"),(TCHAR *)1,	(TCHAR *)&dwDispStep,	NULL,
	TEXT("DefEnable"),	(TCHAR *)1,	(TCHAR *)&dwDefEnable,	NULL,
	TEXT("DefWeekView"),(TCHAR *)1,	(TCHAR *)&dwDefWeek,	NULL,
	TEXT("DefMonthView"),(TCHAR *)1,(TCHAR *)&dwDefMonth,	NULL,
	TEXT("DefDayView"),	(TCHAR *)1,	(TCHAR *)&dwDefDay,		NULL,
	TEXT("DefGrayView"),(TCHAR *)1,	(TCHAR *)&dwDefGray,	NULL,
	TEXT("DefGrayDay"),	(TCHAR *)1,	(TCHAR *)&dwHolyDay,	NULL,
	TEXT("DefStartTime"),(TCHAR *)1,(TCHAR *)&dwStartTime,	NULL,
	TEXT("DefFinalTime"),(TCHAR *)1,(TCHAR *)&dwFinalTime,	NULL,
	TEXT("DefLeadTime"),(TCHAR *)1,	(TCHAR *)&dwDefLeadTime,NULL,
	TEXT("ToDoTimeSort"),(TCHAR *)1,(TCHAR *)&dwToDoSort,	NULL,
	TEXT("ToDoPopup"),	(TCHAR *)1,	(TCHAR *)&dwToDoPopUp,	NULL,
	TEXT("PrivateEnable"),	(TCHAR *)1,	(TCHAR *)&dwPDefEnable,	NULL,
	TEXT("PrivateWeekView"),(TCHAR *)1,	(TCHAR *)&dwPDefWeek,	NULL,
	TEXT("PrivateMonthView"),(TCHAR *)1,(TCHAR *)&dwPDefMonth,	NULL,
	TEXT("PrivateDayView"),	(TCHAR *)1,	(TCHAR *)&dwPDefDay,	NULL,
	TEXT("PrivateGrayView"),(TCHAR *)1,	(TCHAR *)&dwPDefGray,	NULL,
	TEXT("PrivateGrayDay"),	(TCHAR *)1,	(TCHAR *)&dwPHolyDay,	NULL,
	TEXT("PrivateLeadTime"),(TCHAR *)1,	(TCHAR *)&dwPDefLeadTime,NULL,
	TEXT("ToDoWeekView"),(TCHAR *)1,	(TCHAR *)&dwTDefWeek,	NULL,
	TEXT("ToDoMonthView"),(TCHAR *)1,	(TCHAR *)&dwTDefMonth,	NULL,
	TEXT("ToDoDayView"),	(TCHAR *)1,	(TCHAR *)&dwTDefDay,	NULL,
	TEXT("ToDoGrayView"),(TCHAR *)1,	(TCHAR *)&dwTDefGray,	NULL,
	TEXT("ToDoDayDisp"),(TCHAR *)1,	(TCHAR *)&dwToDoDay,	NULL,
	TEXT("ToDoHalfDisp"),(TCHAR *)1,(TCHAR *)&dwToDoHalf,	NULL,
	TEXT("EscapeExit"),	(TCHAR *)1,	(TCHAR *)&dwExitMode,	NULL,
	TEXT("SoundVolume"),(TCHAR *)1,	(TCHAR *)&dwVolume,		NULL,
	TEXT("AlarmDispTime"),(TCHAR *)1,(TCHAR *)&dwAlarmTime,		NULL,
	TEXT("AlarmSuspendTime"),(TCHAR *)1,(TCHAR *)&dwAlarmSnooze,NULL,
//	TEXT("BigFontSize"),(TCHAR *)1,	(TCHAR *)&dwBigFontSize,	NULL,
	TEXT("BigFontSize"),(TCHAR *)1,	(TCHAR *)&dwBigFontSize,	TEXT("16"),
//	TEXT("MidFontSize"),(TCHAR *)1,	(TCHAR *)&dwMidFontSize,	NULL,
	TEXT("MidFontSize"),(TCHAR *)1,	(TCHAR *)&dwMidFontSize,	TEXT("16"),
//	TEXT("SmallFontSize"),(TCHAR *)1,(TCHAR *)&dwSmallFontSize,	NULL,
	TEXT("SmallFontSize"),(TCHAR *)1,(TCHAR *)&dwSmallFontSize,	TEXT("16"),
	TEXT("DelConfirm"),	(TCHAR *)1,	(TCHAR *)&dwConfirm,	NULL,
	TEXT("MultiConfirm"),(TCHAR *)1,(TCHAR *)&dwMultiConfirm,NULL,
	TEXT("HourStep"),	(TCHAR *)1, (TCHAR *)&dwHourStep,	NULL,
	TEXT("TopDayLine"),	(TCHAR *)1, (TCHAR *)&dwTopDayLine,	NULL,
	TEXT("TopWeekLine"),(TCHAR *)1, (TCHAR *)&dwTopWeekLine,NULL,
	TEXT("WeekSync"),	(TCHAR *)1, (TCHAR *)&dwWeekSync,	NULL,
	TEXT("WeekFloat"),	(TCHAR *)1, (TCHAR *)&dwWeekFloat,	NULL,
	TEXT("GrayBack"),	(TCHAR *)1, (TCHAR *)&dwGrayBack,	NULL,
	TEXT("DateFormat"),	(TCHAR *)1, (TCHAR *)&dwDateFormat,	NULL,
	TEXT("FindTitle"),	(TCHAR *)1, (TCHAR *)&dwFindTitle,	NULL,
	TEXT("FindLocation"),(TCHAR *)1,(TCHAR *)&dwFindLocation,NULL,
	TEXT("DataSyncDir"),(TCHAR *)1,	(TCHAR *)&TransDir,		NULL,
	TEXT("FindNote"),	(TCHAR *)1, (TCHAR *)&dwFindNote,	NULL,
	TEXT("TextColor"),	(TCHAR *)4, (TCHAR *)&dwTextColor,	NULL,
	TEXT("LineColor"),	(TCHAR *)4, (TCHAR *)&dwLineColor,	NULL,
	TEXT("BackColor"),	(TCHAR *)4, (TCHAR *)&dwBackColor,	NULL,
	TEXT("GrayColor"),	(TCHAR *)4, (TCHAR *)&dwGrayColor,	NULL,
	TEXT("ToDoRange"),	(TCHAR *)2, (TCHAR *)&dwToDoRange,	NULL,
	TEXT("DefDuration"),(TCHAR *)2,	(TCHAR *)&dwDefLength,	NULL,
	TEXT("PrivateDuration"),(TCHAR *)2,	(TCHAR *)&dwPDefLength,	NULL,
	TEXT("AlarmDate"),	(TCHAR *)2,	(TCHAR *)&AlarmDate,	NULL,
	TEXT("AlarmTime"),	(TCHAR *)2,	(TCHAR *)&AlarmTime,	NULL,
	TEXT("ExecuteName"),(TCHAR *)0,	szExecFileName,		TEXT("\\Program Files\\Oyajin\\appd.exe"),	// [[iwad]]
#ifndef	_WIN32_WCE
	TEXT("NotifyName"),	(TCHAR *)0,	szNotifyFileName,	TEXT(""),
#endif
	TEXT("AppFileDir"),	(TCHAR *)0,	szCurentDir,		TEXT(""),
	TEXT("AppFileName"),(TCHAR *)0,	szCurentFileName,	TEXT(""),
	TEXT("WaveFileDir"),(TCHAR *)0,	szWaveDir,			TEXT(""),
	TEXT("WaveFileName"),(TCHAR *)0,szWaveFileName,		TEXT(""),

//	TEXT("BigFontName"),(TCHAR *)0,	szBigFontName,		TEXT(" Auto"),
//	TEXT("BigFontName"),(TCHAR *)0,	szBigFontName,		TEXT(" 自動"),	// by uminchu
	TEXT("BigFontName"),(TCHAR *)0,	szBigFontName,		TEXT("ＭＳ Ｐゴシック"),	// [iwad]
//	TEXT("MidFontName"),(TCHAR *)0,	szMidFontName,		TEXT(" Auto"),
//	TEXT("MidFontName"),(TCHAR *)0,	szMidFontName,		TEXT(" 自動"),	// by uminchu
	TEXT("MidFontName"),(TCHAR *)0,	szMidFontName,		TEXT("ＭＳ Ｐゴシック"),	// [iwad]
//	TEXT("SmallFontName"),(TCHAR *)0,szSmallFontName,	TEXT(" Auto"),
//	TEXT("SmallFontName"),(TCHAR *)0,szSmallFontName,	TEXT(" 自動"),	// by uminchu
	TEXT("SmallFontName"),(TCHAR *)0,szSmallFontName,	TEXT("ＭＳ Ｐゴシック"),	// [iwad]
	TEXT("AlarmText"),	(TCHAR *)0,	AlarmTitle,			TEXT(""),
	TEXT("FindText"),	(TCHAR *)0,	szFindText,			TEXT(""),
	NULL, NULL, NULL, NULL
};

// [iwad] 左ソフトキー対応
LPCTSTR		RegTable2[][4] = {
	TEXT("Default"),(TCHAR *)0,	szDefNameL,		TEXT(""),
	TEXT("Open"),(TCHAR *)0,	szOpenNameL,	TEXT(""),
	TEXT("Default"),(TCHAR *)0,	szDefNameR,		TEXT(""),
	TEXT("Open"),(TCHAR *)0,	szOpenNameR,	TEXT(""),
	NULL, NULL, NULL, NULL
};