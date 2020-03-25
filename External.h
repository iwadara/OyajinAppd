#include	"struct.h"	// Structure Define
//
//	Macros
//
#define	countof(x)	(sizeof(x)/(sizeof((x)[0])))
//
//	Defines
//
#define	WM_TIMEMSG		(WM_USER + 1)
#define	WM_ALARM		(WM_USER + 2)
#define MYWM_NOTIFYICON	(WM_USER + 3)
#define	WM_CONNECT		(WM_USER + 4)
#define	ICONSTART		0x4193
#define	MAXDAYLINE		32
#define	MAXDAYBUF		128	// Max Day Display Buffer Number
//
//	Extern Vars
//
extern	LPCTSTR		szVerNum;		// szVerNum Number TEXT
extern	LPCTSTR		szHelpFileName;	// Help FileName
extern	LPCTSTR		RegTable[][4];	// Registry Entry List
extern	LPCTSTR		szRegName;	//	Registry Key Name
extern	LPCTSTR		szWinName;	//	Registry Key Name
extern	LPCTSTR		DayName[];	// DayName;
extern	LPCTSTR		MonthName[];// MonthName;
//	Handlers
extern	HINSTANCE	hInst;		// Global Instance copy
extern	HWND		hMainWnd;	// Main Window handle
extern	HWND		hCmdBar;	// CommandBar handle (for CE)
extern	HWND		hTB;		// ToolBar Handle
extern	HWND		hWndB;		// Stand Alone Menu Button Window Handle
extern	HWND		hWndX;		// Stand Alone Exit Button Window Handle
extern	HWND		FocusSave;	// Focus Save Buffer for Input Screen
extern	HACCEL		hAccl;		// Accelerator handle
extern	HDC			hMemDC;		// Memory Device Context
extern	HFONT		hSmallFont, hMidFont, hBigFont;	// Font Handles
extern	HFONT		hSystemFont;// System Font Handler
extern	HBITMAP		hBitBuf;	//	BitMap Handle
extern	HBRUSH		hBrush;		// Brush Handle
extern	HBRUSH		hGrayBrush;	// Brush (Holyday) Handle
extern	HPEN		hPen;		// Pen Handle
extern	HANDLE		BackHandle;	//	BackGround Thread Handle
//	For File
extern	FILEWORKS	MainWork;	// Struct for Main File Works
extern	FILEWORKS	BackWork;	// Struct for BackGround File Works
extern	FIELD		BackField[];	// Data Base Field Definition for BackGround
extern	LPDWORD		BackKeyVarPtrTable[];// BackGround Keys
//	For Registry Save
extern	DWORD		dwStartTop, dwStartLeft;	// Start Window Pos
extern	DWORD		dwStartHight, dwStartWidth;// Start Window Size
extern	DWORD		dwMenuMode;	// Menu Display Mode
extern	DWORD		dwPrivate;	// Dialy Mode Flag
extern	DWORD		dwHourMode;	// Hour Disp Mode (0:12H 1:24H)
extern	DWORD		dwAppDispMode;	// Disp Mode (0:Normal 1: App Only)
extern	DWORD		dwAlarmMode;	// Alarm Mode (0:Mute 1:Beep 2:Wave)
extern	DWORD		dwMulti;	// Multi Thread Flag
extern	DWORD		dwFileMode;	// File Mode (0:File 1:Memory)
extern	DWORD		dwGraphMode;// Graph Display Mode (1:On)
extern	DWORD		dwLoopCount;	// Alarm Sound Loop Count
extern	DWORD		dwDefEnable;	// Default Enable
extern	DWORD		dwDefWeek;	// Default Week View
extern	DWORD		dwDefMonth;	// Default Month View
extern	DWORD		dwDefDay;	// Default Day View
extern	DWORD		dwDefGray;	// Default Gray View
extern	DWORD		dwStartTime;	// Day Display Start Time
extern	DWORD		dwFinalTime;	// Day Display Final Time
extern	DWORD		dwDefLeadTime;	// Default Lead Time
extern	DWORD		dwDefLength;	// Default Duration
extern	DWORD		dwPDefEnable;	// Default Enable
extern	DWORD		dwPDefWeek;		// Default Week View
extern	DWORD		dwPDefMonth;	// Default Month View
extern	DWORD		dwPDefDay;		// Default Day View
extern	DWORD		dwPDefGray;		// Default Gray View
extern	DWORD		dwPHolyDay;		// WeekDay of Gray
extern	DWORD		dwPDefLeadTime;	// Default Lead Time
extern	DWORD		dwPDefLength;	// Default Duration
//
extern	DWORD		dwTDefWeek;		// Default Week View
extern	DWORD		dwTDefMonth;	// Default Month View
extern	DWORD		dwTDefDay;		// Default Day View
extern	DWORD		dwTDefGray;		// Default Gray View
extern	DWORD		dwToDoDay;		// Disp ToDo on Day Screen
extern	DWORD		dwToDoHalf;		// Disp ToDo on Half Year Screen
extern	DWORD		dwDispStep;		// Display Step (0:1:00 1:0:30)
extern	DWORD		dwUnderBar;		// Under Line Flag (1:On)
extern	DWORD		dwExitMode;		// ESC Exit Mode
extern	DWORD		dwVolume;		// Sound Volume
extern	DWORD		dwAlarmTime;	// Alarm Dialog Display Time
extern	DWORD		dwAlarmSnooze;	// Custom Snooze Time
extern	DWORD		dwConfirm;		// Confirm When Delete
extern	DWORD		dwMultiConfirm;	// Confirm When Multi Change & Delete
extern	DWORD		dwHourStep;		// Hour Inc/Dec Step
extern	DWORD		dwTextColor;	// Text Color
extern	DWORD		dwLineColor;	// Line Color
extern	DWORD		dwBackColor;	// Back Ground Color
extern	DWORD		dwGrayColor;	// Back Ground Color
extern	DWORD		dwWeekSync;		// Week Sync other Day
extern	DWORD		dwWeekFloat;	// Week Time Line floating Time
extern	DWORD		dwTopDayLine;	// Max AllDay Scedule Lines(Day Screen)
extern	DWORD		dwTopWeekLine;	// Max AllDay Scedule Lines(Week Screen)
extern	DWORD		dwGrayBack;		// Gray Back Switch(WinCE Default Off)
extern	DWORD		dwToDoSort;		// ToDo Time Sort Flag
extern	DWORD		dwToDoPopUp;		// ToDo Time Sort Flag
extern	DWORD		dwHolyDay;		// WeekDay of Gray
extern	DWORD		dwDateFormat;	// English Format Flag
extern	TCHAR		szExecFileName[];// Current FileName Buffer
#ifndef	_WIN32_WCE
extern	TCHAR		szNotifyFileName[];// Current FileName Buffer
#endif
extern	TCHAR		szCurentDir[];	// Path Name Buffer		
extern	TCHAR		szCurentFileName[];// Current FileName Buffer
extern	TCHAR		szWaveDir[];	// Path Name Buffer		
extern	TCHAR		szWaveFileName[];// Current FileName Buffer
//	For Display Screen
extern	DWORD		dwWinTop, dwWinWidth, dwWinHight;// Window dimensions
extern	DWORD		dwTodayYear, dwTodayMonth, dwTodayDay;// Today's Date
extern	DWORD		dwTodayHour, dwTodayMinute;	// Today Hour & Min
extern	DWORD		dwLastDay;			// Last Day Number
extern	DWORD		dwYear, dwMonth;	// Current Year & Month
extern	DWORD		dwCurX, dwCurY;		// Black Box Cursors
extern	DWORD		dwOldCurX, dwOldCurY;	// Old Cursor Pos.
extern	DWORD		dwXsize,dwYsize;	// Screen Dimensions
extern	DWORD		StartTime;		// Day Display Start Time
extern	DWORD		CurrentTime;	// Current Select Time
extern	DWORD		DispMode;		// Main Screen Display Mode
extern	DWORD		OldDispMode;	// Old Display Mode
extern	DWORD		MonthBuf[][42][3];	// Month Buffer
extern	DWORD		WeekBuf[][3];		// Week Buffer
//	For Display Day Screen
extern	DWORD		DayOrgTimeBuf[];// Date Time
extern	DWORD		DayOrgIDBuf[];	// Day ID List
extern	DWORD		DayOrgAllDayBuf[];	// AllDay Flag List
extern	DWORD		DayOrgDateBuf[];	// Date Buffer (For ToDo Screen)
extern	DWORD		DayLineCount;		// Day Line Count
extern	DWORD		dwSmallFontHight, dwMidFontHight, dwSmallFontWidth;	// Small Font Metrics
extern	DWORD		dwBigFontHight, dwMidFontWidth, dwBigFontWidth;	// Big Font Metrics
extern	DWORD		dwDayLine;	// Day Display Screen Line Cursor Pos
extern	DWORD		dwDispFlag;	// Display Flag of Main Screen
extern	LPTSTR		ClipBuf;	// Clip Board Buffer
extern	LPTSTR		CsvBuf;	// CSV Board Buffer
//	For Input Dialog Box
extern	DWORD		dwDlgStartTime, dwDlgEndTime, dwDlgDate;// Start End Time
extern	DWORD		dwDlgEnable,dwDlgLeadTime, dwDlgConsDay;// Alarm Info
extern	DWORD		dwDlgWeekView, dwDlgMonthView;	// Check Box Info
extern	DWORD		dwDlgDayView, dwDlgGrayView;	// Check Box Info
extern	DWORD		dwDlgToDo;	// ToDo Record Flag
extern	DWORD		dwDlgMode;// Dialog Mode(0:New 1:Edit), Seek Ptr
extern	DWORD		dwGenID, dwOldGenID, dwOrgDate;// Generation ID (For Multi Date Records)
extern	DWORD		dwOldOrgDate, dwOldTime, dwOldCons;// Old Dialog Data
extern	DWORD		dwAllDay, dwOldAllDay;	// All Day Flag
extern	unsigned char lpOldRepeat[];	// Old Repeat Data
extern	DWORD		OldDispMode;	// Old Display Mode
extern	DWORD		ToDoBackMode;	// ToDo Back Mode (For Return)
extern	DWORD		dwToDoRange;	// ToDo View Range
extern	DWORD		dwToDoMode;		// ToDo Mode
extern	DWORD		dwPrivateMode;	// Diary Mode
extern	DWORD		dwFullMode;			// Full Screen Mode Flag
extern	DWORD		dwDlgPrivate;		// Diary Flag
//	For Repeat Dialog
extern	DWORD		dwRepeatMode;	// Repeat Mode (0:Off 1:Daily 2:Weekly...)
extern	DWORD		dwRepeatFreq;	// Repeat Frequency
extern	DWORD		dwRepeatStartDate;	// Repeat Start Date
extern	DWORD		dwRepeatEndDate;	// Repeat End Date
extern	DWORD		dwRepeatByPos;	// By Date or By Pos Mode
extern	DWORD		dwRepeatDate;	// Day Number or Date
extern	DWORD		dwRepeatWeek;	// 1st 2nd 3rd Week
extern	DWORD		dwRepeatDay;	// Repeat Day (Mon-Sun)
extern	DWORD		dwRepeatMonth;	// Repeat Month (Jan-Dec)
extern	DWORD		DayID[];		// WeekDay Button ID Array
extern	TCHAR		TitleBuf[256];	// Title Buf
extern	char		lpDlgTitle[], lpDlgNote[];		// Title & Note
extern	char		lpDlgLocation[];	// Location & Repeat Info
extern	unsigned char		lpDlgRepeat[14];	// Repeat Info
extern	DWORD		BackDate, BackAllDay, BackStartTime, BackGenID;
extern	DWORD		BackPrivate, BackToDo;	// BackGround Private Flag
extern	DWORD		BackOrgDate, BackConsDay, BackRepeat[], BackDlgMode;
extern	DWORD		HideGenID;	// GenID now Delete/Add
extern	DWORD		FileLock;	// File Lock Flag
extern	DWORD		FileResult;	// File Result of BackGround Thread
extern	DWORD		Abort;		// Thread Abort Flag
//	For Font Settings
extern	TCHAR		szBigFontName[];	// Large Font Name
extern	TCHAR		szMidFontName[];	// Mid Font Name
extern	TCHAR		szSmallFontName[];	// Small Font Name
extern	DWORD		dwBigFontSize, dwMidFontSize, dwSmallFontSize;// Font Size
//	For Registry Entry
extern	LPCTSTR		RegTable[][4];
extern	LPCTSTR		szTips[];		// Tool Tip Text
extern	TBBUTTON	lpButtons[];	// Tool Buttons
extern	LPDWORD		KeyPtrTable[];		// Var Ptr Table for ForeGround
extern	LPDWORD		BackKeyPtrTable[];	// Var Ptr Table for Background
extern	DWORD		numButtons;		// Count of lpButtons
extern	DWORD		numTips;		// Count of lpButtons
//	For Alarm
extern	LPTSTR		szArg;			// Argment Pointer
extern	char		BackTitle[];	// Alarm Title
extern	char		BackLocation[];	// Alarm Location
extern	DWORD		BackEnable, BackLeadTime;	// Alarm Works
extern	TCHAR		AlarmTitle[];	// Alarm Dialog Display Text
extern	DWORD		AlarmDate;		// Current Alarm Date
extern	DWORD		AlarmTime;		// Current Alarm Time
//	For Find Dialog
extern	BOOL		dwFindTitle;	// Find Title Check Flag
extern	BOOL		dwFindLocation;	// Find Location Check Flag
extern	BOOL		dwFindNote;		// Find Note Check Flag
extern	TCHAR		szFindText[];
//	For Range Dialog
extern	DWORD		dwCategory;		// Category Switch
extern	DWORD		dwToDoCategory;	// ToDo Category Switch
extern	DWORD		dwStartDate;		// After Day
extern	DWORD		dwEndDate;		// Before Day
extern	DWORD		RangeMode;		// Range Mode (BetWeen or Split)
//	For Week Screen
extern	DWORD		WeekTimeBuf[][MAXDAYLINE];	// Original Time List(14 Lines)
extern	DWORD		WeekGenIDBuf[][MAXDAYLINE];	// Day Seek Ptr List(14 Lines)
extern	DWORD		WeekAllDayBuf[][MAXDAYLINE];// All Day Flag List
extern	DWORD		WeekCurY;					// Week Screen Cursor Y
//	For Day Screen
extern	DWORD		BarBuf[];				// Diagram Bar Buffer (0 to 31)
extern	DWORD		BlacketBitPat[];
//	For One Day Array
extern	DWORD		DayStartTime[][MAXDAYBUF];	// Start Time Buffer
extern	DWORD		DayEndTime[][MAXDAYBUF];	// End Time Buf
extern	DWORD		DayGenID[][MAXDAYBUF];		// GenID Buffer
extern	DWORD		DayAllDay[][MAXDAYBUF];		// AllDay Flag Buffer
extern	TCHAR		DayTitle[][MAXDAYBUF][64];	// Title Buf
extern	DWORD		DayBlacket[][MAXDAYBUF];	// Blacket Buffer 000:Blank 001:- 010:Start 011:| 100:End 101:T
extern	DWORD		DayAlarm[MAXDAYBUF];		// Alarm Flag Array
extern	DWORD		DayToDo[MAXDAYBUF];			// ToDo Flag Array
extern	DWORD		ToDoDate[MAXDAYBUF];		// Date of ToDo Array
extern	DWORD		DayAllNum[];	// All DEay Schedule Number (Includes Blank & Non Blank)
extern	DWORD		DayOneNum[];	// One Shot Schedule Number
extern	DWORD		ActualNum[];	// Actual Record Number (Not Blank)
extern	DWORD		StartIndex;		// Start Index of One Shot Schedule
extern	DWORD		AllStartIndex;	// Start Index of All Day Schedule
extern	DWORD		HideSkip;		// Hide Mode Start Line
extern	DWORD		AllDayHight;	// All Day Scedule Lines
extern	DWORD		DispHour;		// Display Hour
extern	BOOL		TransDir;		// Sync Direction
extern	TCHAR		APMark[];		// Am Pm Mark
extern	HANDLE		hSyncHandle;	// Sync Handle
extern	DWORD		SyncLock;		// Sync Lock Flag
extern	DWORD		SyncTimerCount;	// Sync Timer Count
extern	DWORD		SyncThreadID;	// Thread Identifier
extern	WNDPROC		lpEdit;			// Edit Class Proc Adrs
extern	LPCTSTR		ShortDayName[7];	// Short WeekDay Name
extern	LPCTSTR		ShortMonthStr[12];	// Short Month Name
extern	LPCTSTR		RepeatName[5];	// Repeat Status String
extern	LPCTSTR		WeekPosName[5];	// Week Pos Name (1st, 2nd..)
