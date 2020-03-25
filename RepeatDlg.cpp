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
DWORD	GetCurrentDate(void);
DWORD	DwordtoBit(DWORD);
DWORD	GetRepeatStart(DWORD);
DWORD	ConvDateToWeekDay(DWORD);
DWORD	ConvDateToWeekNum(DWORD);
DWORD	GetFinalDay(DWORD);
LPCTSTR	GetDateString(DWORD);
BOOL	CALLBACK OyajiRepeatDateEditProc(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK OyajiDayByteEditProc(HWND, UINT, WPARAM, LPARAM);
BOOL	ConvDateToDword(LPDWORD, LPTSTR);
void	RepeatError(HWND, DWORD);
BOOL	DelRecord(BOOL);
void	GetNextDate(LPDWORD);
void	GetPrevDate(LPDWORD);
void	SetDateString(LPCTSTR);
BOOL	MoveNextRepeat(LPDWORD);
BOOL	OyajiAddRecord(FILEWORKS *, DWORD);
BOOL	OyajiDeleteRecord(FILEWORKS *, DWORD);
BOOL	OyajiMakeRecord(FILEWORKS *);
BOOL	OyajiReleaseRecord(FILEWORKS *);
BOOL	OyajiCopyFile(FILEWORKS *, FILEWORKS *, DWORD);
BOOL	OyajiSaveFile(FILEWORKS *, FILEWORKS *, DWORD);
BOOL	OyajiGetNextRecord(FILEWORKS *);
BOOL	OyajiGetRecord(FILEWORKS *);
DWORD	OyajiSearchRecord(FILEWORKS *);
void	*OyajiGetFieldPos(FILEWORKS *, DWORD);
void	SetupMainScreen(HDC);
void	SetAlarm(void);
//
static void	InitRepeatDlg(HWND);
static void SetOrgDlgData(void);
static BOOL AddRecordSub(BOOL);
static BOOL DelRecordSub(BOOL);
static DWORD WINAPI AddRecordThread(BOOL);
static DWORD WINAPI	DelRecordThread(BOOL);
static BOOL	RemoveRecordSub(void);
void	SetupBackVars(void);
//
//	Works
//
static	DWORD	Mode;	// Repeat Mode
//
//	Button ID Table
//
static	DWORD ButtonID[5] = {IDC_NONE, IDC_DAILY, IDC_WEEKLY, IDC_MONTHLY, IDC_YEARLY};
static	DWORD RepeatID[5] = {IDC_FREQ, IDC_STARTDATE, IDC_ENDDATE,
							IDC_BYDAY, IDC_DATE};
static	DWORD WeekID[5] =	{IDC_1ST, IDC_2ND, IDC_3RD, IDC_4TH, IDC_5TH};
static	DWORD MonthID[12] = {IDC_JAN, IDC_FEB, IDC_MAR, IDC_APL, IDC_MAY, IDC_JUN,
							IDC_JUL, IDC_AUG, IDC_SEP, IDC_OCT, IDC_NOV, IDC_DEC};
static	BOOL RepeatEnable[5][8] = {
	0, 0, 0, 0,	0, 0, 0, 0,	// None
	1, 1, 1, 0, 0, 0, 0, 0,	// Daily
	1, 1, 1, 0, 0, 0, 1, 0,	// Weekly
	1, 1, 1, 1, 1, 1, 1, 0,	// Mothly
	1, 1, 1, 1, 1, 1, 1, 1};// Yearly
//
//	Repeat Dialog Proc
//
BOOL CALLBACK RepeatDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
DWORD	i, j;	// Counter
TCHAR	DateBuf[16];	// Date String Buffer
DWORD	TempDate;	// Temp Date
	switch(message)
	{
//	Init Repeat Dialog
	case WM_INITDIALOG:
//	Setup Dialog Control Text
//	Get Mode
		Mode = dwRepeatMode;
		SendDlgItemMessage(hDlg, ButtonID[Mode], BM_SETCHECK, TRUE, 0);
		SendDlgItemMessage(hDlg, IDC_BYDAY, BM_SETCHECK, (dwRepeatByPos == 0), 0);
		SetDlgItemText(hDlg, IDC_STARTDATE, GetDateString(dwRepeatStartDate));
		SetDlgItemText(hDlg, IDC_ENDDATE, GetDateString(dwRepeatEndDate));// 3 Years
		SetDlgItemInt(hDlg, IDC_FREQ, dwRepeatFreq, 0);// 3 Years
		for(i = 0, j = 1; i < 5; i++, j *= 2)
			if(dwRepeatWeek & j)
				SendDlgItemMessage(hDlg, WeekID[i], BM_SETCHECK, TRUE, 0);
		for(i = 0, j = 1; i < 7; i++, j *= 2)
			if(dwRepeatDay & j)
				SendDlgItemMessage(hDlg, DayID[i], BM_SETCHECK, TRUE, 0);
		for(i = 0, j = 1; i < 12; i++, j *= 2)
			if(dwRepeatMonth & j)
				SendDlgItemMessage(hDlg, MonthID[i], BM_SETCHECK, TRUE, 0);
		SetWindowLong(GetDlgItem(hDlg, IDC_STARTDATE), GWL_WNDPROC, (LONG )OyajiRepeatDateEditProc);		
		SetWindowLong(GetDlgItem(hDlg, IDC_ENDDATE), GWL_WNDPROC, (LONG )OyajiRepeatDateEditProc);		
		SetWindowLong(GetDlgItem(hDlg, IDC_FREQ), GWL_WNDPROC, (LONG )OyajiDayByteEditProc);		
		if(Mode == 4)
			SetWindowLong(GetDlgItem(hDlg, IDC_DATE), GWL_WNDPROC, (LONG )OyajiRepeatDateEditProc);		
		else
			SetWindowLong(GetDlgItem(hDlg, IDC_DATE), GWL_WNDPROC, (LONG )OyajiDayByteEditProc);		
		InitRepeatDlg(hDlg);
		return TRUE;
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDC_NONE:
			Mode = 0;
			goto SetMode;
		case IDC_DAILY:
			Mode = 1;
			goto SetMode;
		case IDC_WEEKLY:
			Mode = 2;
			goto SetMode;
		case IDC_MONTHLY:
			Mode = 3;
			goto SetMode;
		case IDC_YEARLY:
			Mode = 4;
			goto SetMode;
		case IDC_BYDAY:
SetMode:
			InitRepeatDlg(hDlg);	// Enable & Disenable Controls
			return TRUE;	// Message Processed
//	Ok Button
		case IDOK:
			for(i = 0; i < 6; i++)
				if(SendDlgItemMessage(hDlg, ButtonID[i], BM_GETCHECK, 0, 0))
					break;
			dwRepeatMode = i;
			dwRepeatByPos = (SendDlgItemMessage(hDlg, IDC_BYDAY, BM_GETCHECK, 0, 0) == FALSE);
			GetDlgItemText(hDlg, IDC_DATE, DateBuf, sizeof(DateBuf) / sizeof(TCHAR) - 1);
			if(ConvDateToDword(&TempDate, DateBuf) == FALSE)
			{
				RepeatError(hDlg, 0);	// Date Error
				return TRUE;
			}
			if((TempDate / 32) == 0)	// Month = 0?
				dwRepeatDate = (dwRepeatDate & 0x1e0) | TempDate;
			else
				dwRepeatDate = TempDate;
			GetDlgItemText(hDlg, IDC_STARTDATE, DateBuf, sizeof(DateBuf) / sizeof(TCHAR) - 1);
			if(ConvDateToDword(&dwRepeatStartDate, DateBuf) == FALSE)
			{
				RepeatError(hDlg, 1);	// Date Error
				return TRUE;
			}
			GetDlgItemText(hDlg, IDC_ENDDATE, DateBuf, sizeof(DateBuf) / sizeof(TCHAR) - 1);
			if(ConvDateToDword(&dwRepeatEndDate, DateBuf) == FALSE)
			{
				RepeatError(hDlg, 2);	// Date Error
				return TRUE;
			}
			dwRepeatFreq = 	GetDlgItemInt(hDlg, IDC_FREQ, 0, 0);
			if(dwRepeatFreq == 0)
			{
				RepeatError(hDlg, 3);	// Date Error
				return TRUE;
			}
			for(i = 0, j= 1; i < 7; i++, j *= 2)
				if(SendDlgItemMessage(hDlg, DayID[i], BM_GETCHECK, 0, 0))
					dwRepeatDay |= j;
				else
					dwRepeatDay &= j ^ MAXDWORD;
			for(i = 0, j= 1; i < 5; i++, j *= 2)
				if(SendDlgItemMessage(hDlg, WeekID[i], BM_GETCHECK, 0, 0))
					dwRepeatWeek |= j;
				else
					dwRepeatWeek &= j ^ MAXDWORD;
			for(i = 0, j= 1; i < 12; i++, j *= 2)
				if(SendDlgItemMessage(hDlg, MonthID[i], BM_GETCHECK, 0, 0))
					dwRepeatMonth |= j;
				else
					dwRepeatMonth &= j ^ MAXDWORD;
//	Check Check Box Set Right
			if(
			   (dwRepeatMode == 2 && dwRepeatDay == 0)
			|| (dwRepeatMode == 3 && dwRepeatByPos && (dwRepeatDay == 0 || dwRepeatWeek == 0))
			|| (dwRepeatMode == 4 && dwRepeatByPos && (dwRepeatDay == 0 || dwRepeatWeek == 0 || dwRepeatMonth == 0))
			|| (dwRepeatMode == 5 && dwRepeatMonth == 0)
			|| (dwRepeatMode == 5 && dwRepeatByPos && (dwRepeatDay == 0 || dwRepeatWeek == 0)))
			{
				RepeatError(hDlg, 4);
				return TRUE;
			}
			TempDate = GetRepeatStart(dwRepeatStartDate);
			dwRepeatStartDate = TempDate;
			SetDateString(GetDateString(TempDate));
			// Set 1 st Date
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
//
//	Disp Error Message
//
void RepeatError(HWND hWnd, DWORD MsgCode)
{
LPCTSTR	ErrorMsg[] = {	TEXT("Invalid Day/Date"),
						TEXT("Invalid Starting Date"),
						TEXT("Invalid Ending Date"),
						TEXT("Invalid Frequency"),
						TEXT("Set Check Box at least one")};
	MessageBox(hWnd, ErrorMsg[MsgCode], TEXT("Repeat Error"), MB_OK);
}
//
//	Init Repeat Dialog
//
void InitRepeatDlg(HWND hDlg)
{
DWORD	ByPos;	// By Pos
DWORD	i;
//	Get ByPos
	ByPos = (SendDlgItemMessage(hDlg, IDC_BYDAY, BM_GETCHECK, 0, 0) == FALSE);
//	Setup Static Text & Custom Class
	if(Mode == 2)
		//SetDlgItemText(hDlg, IDC_FREQTEXT, TEXT("week(s)"));
		SetDlgItemText(hDlg, IDC_FREQTEXT, TEXT("週毎")); // by uminchu
	else if(Mode == 3)
		//SetDlgItemText(hDlg, IDC_FREQTEXT, TEXT("month(s)"));
		SetDlgItemText(hDlg, IDC_FREQTEXT, TEXT("月毎")); // by uminchu
	else if(Mode == 4)
		//SetDlgItemText(hDlg, IDC_FREQTEXT, TEXT("year(s)"));
		SetDlgItemText(hDlg, IDC_FREQTEXT, TEXT("年毎"));	// by uminchu
	else
		//SetDlgItemText(hDlg, IDC_FREQTEXT, TEXT("day(s)"));
		SetDlgItemText(hDlg, IDC_FREQTEXT, TEXT("日毎"));	// by uminchu
	if(Mode == 4)
	{
		//SetDlgItemText(hDlg, IDC_BYDAY, TEXT("By D&ate"));
		SetDlgItemText(hDlg, IDC_BYDAY, TEXT("月/日指定(&A)"));
		SetDlgItemText(hDlg, IDC_DATE, GetDateString(dwRepeatDate));
		SetWindowLong(GetDlgItem(hDlg, IDC_DATE), GWL_WNDPROC, (LONG )OyajiRepeatDateEditProc);		
	}
	else
	{
		SetDlgItemText(hDlg, IDC_BYDAY, TEXT("日付指定(&B)"));
		SetDlgItemInt(hDlg, IDC_DATE, dwRepeatDate & 31, 0);
		SetWindowLong(GetDlgItem(hDlg, IDC_DATE), GWL_WNDPROC, (LONG )OyajiDayByteEditProc);		
	}
//	Enable General Controls
	for(i = 0; i < 5; i++)
		EnableWindow(GetDlgItem(hDlg, RepeatID[i]), RepeatEnable[Mode][i]);
	for(i = 0; i < 5; i++)
		EnableWindow(GetDlgItem(hDlg, WeekID[i]), RepeatEnable[Mode][5] && ByPos);
	for(i = 0; i < 7; i++)
		EnableWindow(GetDlgItem(hDlg, DayID[i]), RepeatEnable[Mode][6] && ByPos || Mode == 2);
	for(i = 0; i < 12; i++)
		EnableWindow(GetDlgItem(hDlg, MonthID[i]), RepeatEnable[Mode][7] && ByPos || Mode == 5);
	if(Mode > 2)
		EnableWindow(GetDlgItem(hDlg, IDC_DATE), (ByPos == 0));
}
//
//	Convert DWORD to Bit (Arg 1 to 32)
//
DWORD DwordToBit(DWORD dword)
{
	return 1 << (dword - 1);
}
//
//	Setup File Works for BackGround
//
void SetupBackWorks(void)
{
	memcpy(&BackWork, &MainWork, sizeof(FILEWORKS));	// Copy Works
	BackWork.FileHandle = NULL;	// FileHandle
	BackWork.IndexList = NULL;	// Sort Index Table
	BackWork.FieldList = BackField;	// Data Base Field Definition List for BackGround
}
//
//	Add Record
//
BOOL AddRecord(BOOL AllDelete, BOOL MultiMode)
{
DWORD	ThreadID;
DWORD	HideSave;
	dwFileMode = 1;	// データ破壊問題対策、キャッシュ使用を強制的に有効にする(iwad 2007.01.22修正)
	FileLock = 0;
	HideGenID = 0;	// Clear Hide Current Opr Record
	BackDlgMode = dwDlgMode;	// New or Change?
	HideSave = dwGenID;	// Hide Current Opr Record
	dwGenID = MainWork.FileAge;	// Set New File Age
	if(OyajiMakeRecord(&MainWork) == FALSE)	// Make Record in Memory
		return FALSE;
	SetupBackWorks();	// Setup Works for BackGround
	if(OyajiCopyFile(&MainWork, &BackWork, dwFileMode) == FALSE)// Create Temp File 1st
		return FALSE;
	if(dwMulti && MultiMode)
	{
		FileLock = 1;
		HideGenID = HideSave;
		BackHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE )AddRecordThread, (LPVOID )AllDelete, 0, &ThreadID);
		if(dwFileMode == 0)
			SetThreadPriority(BackHandle, THREAD_PRIORITY_BELOW_NORMAL);
	}
	else
	{
#ifdef	_WIN32_WCE
		SetCursor((HCURSOR )IDC_WAIT);
#endif
		if(AddRecordThread(AllDelete) == FALSE)
		{
#ifdef	_WIN32_WCE
			SetCursor(NULL);
#endif
			return FALSE;
		}
		OyajiSaveFile(&MainWork, &BackWork, dwFileMode);
		if(MultiMode)
		{
			SetupMainScreen(hMemDC);
			InvalidateRect(hMainWnd, NULL, 0);
			SetAlarm();
		}
#ifdef	_WIN32_WCE
		SetCursor(NULL);
#endif
	}
	return TRUE;
}
//
//	Del Record
//
BOOL DelRecord(BOOL AllDelete)
{
DWORD	ThreadID;
	dwFileMode = 1;	// データ破壊問題対策、キャッシュ使用を強制的に有効にする(iwad 2007.01.22修正)
	FileLock = 0;
	HideGenID = 0;	// Clear Hide ID
	SetupBackWorks();	// Setup Works for BackGround
	if(OyajiCopyFile(&MainWork, &BackWork, dwFileMode) == FALSE)// Create Temp File 1st
		return FALSE;
	SetupBackVars();	// Setup Vars of Backgruound
	if(dwMulti)
	{
		FileLock = 1;
		HideGenID = dwGenID;	// Hide Current Opr Record
		BackHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE )DelRecordThread, (LPVOID )AllDelete, 0, &ThreadID);
		if(dwFileMode == 0)// File Mode?
			SetThreadPriority(BackHandle, THREAD_PRIORITY_BELOW_NORMAL);
	}
	else
	{
#ifdef	_WIN32_WCE
		SetCursor((HCURSOR )IDC_WAIT);
#endif
		if(DelRecordThread(AllDelete) == FALSE)	// Success?
		{
#ifdef	_WIN32_WCE
			SetCursor(NULL);
#endif
			return FALSE;
		}
		OyajiSaveFile(&MainWork, &BackWork, dwFileMode);// Change Result
		SetupMainScreen(hMemDC);
		InvalidateRect(hMainWnd, NULL, 0);
		SetAlarm();
#ifdef	_WIN32_WCE
		SetCursor(NULL);
#endif
	}
	return TRUE;
}
//
//	Setup BackGround Vars
//
void SetupBackVars(void)
{
DWORD	i;
	BackOrgDate = dwOrgDate;	// Set Vars for Back Ground
	BackDate = dwDlgDate;
	BackStartTime = dwDlgStartTime;
	BackConsDay = dwDlgConsDay;	// ConsDays
	BackAllDay = dwAllDay;		// AllDay Flag
	BackGenID = dwGenID;		// Generation ID
	for(i = 0; i < 14; i++)
		BackRepeat[i] = lpDlgRepeat[i];
}
//
//	Restore BackGround Vars From Record
//
static void RestoreBackVars(void)
{
DWORD	i;
unsigned char *TempPtr;
unsigned char *Rep = (unsigned char *)OyajiGetFieldPos(&BackWork, 20);// 20 = Repeat
	TempPtr = (unsigned char *)OyajiGetFieldPos(&BackWork, 16);// 16 = dwOrgDate;
	BackOrgDate = *TempPtr + *(TempPtr + 1) * 256;
	BackDate = BackOrgDate;
	TempPtr = (unsigned char *)OyajiGetFieldPos(&BackWork, 3);// Start Time
	BackStartTime = *TempPtr + *(TempPtr + 1) * 256;// Start Time
	BackConsDay = *(unsigned char *)OyajiGetFieldPos(&BackWork, 18);// Cons Days
	BackAllDay = *(unsigned char *)OyajiGetFieldPos(&BackWork, 34);	// All Day Flag
	TempPtr = (unsigned char *)OyajiGetFieldPos(&BackWork, 36);// Generation ID
	BackGenID = *TempPtr + *(TempPtr + 1) * 0x100
				+ *(TempPtr + 2) * 0x10000 + *(TempPtr + 3) * 0x1000000;// Generation ID
	if(*Rep == 0)	// No Repeat Info?
		BackRepeat[0] = 0;	// Clear Only 1 byte
	else
		for(i = 0; i < 14; i++)	// Copy 14 bytes
			BackRepeat[i] = Rep[i];
}
//
//	Add Record Thread
//
static DWORD WINAPI AddRecordThread(BOOL AllDelete)
{
DWORD	ScanDate;	// Repeat Scan Date
	FileResult = TRUE;
	if(BackDlgMode)	// Replace Mode?
	{	// Delete Existing Record First
//	Get Record to be Deleted
		SetOrgDlgData();	// Set Original Data to be Deleted
		if(BackRepeat[0] && AllDelete)	// Repeat Record?
		{
			ScanDate = BackOrgDate;
			while(TRUE)	// Forever
			{
				BackDate = ScanDate;	// Set Repeat Scan Date
				if(DelRecordSub(TRUE) == FALSE || Abort)	// Fail?
				{// AllDelete Mode
					FileResult = FALSE;
					return FALSE;
				}
				if(MoveNextRepeat(&ScanDate) == FALSE)	// Move to Next Repeat Date
					break;	// Finish
			}
		}
		else
		{
			if(DelRecordSub(AllDelete) == FALSE)	// All Delete Mode
			{
				FileResult = FALSE;
				return FALSE;
			}
		}
	}
	RestoreBackVars();
	ScanDate = BackOrgDate;// 16 = dwOrgDate;
	if(BackRepeat[0] && AllDelete)	// Repeat Record?
	{
		while(TRUE)	// Forever
		{
			BackDate = ScanDate;	// Set Add Date
			if(AddRecordSub(TRUE) == FALSE || Abort)
			{
				FileResult = FALSE;
				return FALSE;
			}
			if(MoveNextRepeat(&ScanDate) == FALSE)	// Move to Next Repeat Date
				break;	// Finish
		}
		return TRUE;
	}
	else	// No Repeat Record
	{
		if(AddRecordSub(AllDelete) == FALSE)	// Sucess?
		{
			FileResult = FALSE;
			return FALSE;
		}
		return TRUE;
	}
}
//
//	Delete Records
//
static DWORD WINAPI DelRecordThread(BOOL AllDelete)
{
DWORD	ScanDate;
	FileResult = TRUE;	// Clear Error Flag
	if(BackRepeat[0] && AllDelete)	// Repeat Record & All Delete Mode?
	{
		ScanDate = BackOrgDate;
		while(TRUE)	// Forever
		{
			BackDate = ScanDate;	// Set Repeat Scan Date
			if(DelRecordSub(TRUE) == FALSE || Abort)	// Fail?
			{
				FileResult = FALSE;
				return FALSE;
			}
			if(MoveNextRepeat(&ScanDate) == FALSE)	// Move to Next Repeat Date
				break;	// Finish
		}
		return TRUE;
	}
	else
	{
		if(DelRecordSub(AllDelete) == FALSE)
		{
			FileResult = FALSE;
			return FALSE;
		}
		return TRUE;
	}
}
//
//	Add Record Sub
//
static BOOL AddRecordSub(BOOL AllDelete)
{
unsigned char *DatePos = (unsigned char *)OyajiGetFieldPos(&BackWork, 35);// 27 = Date
	dwFileMode = 1;	// データ破壊問題対策、キャッシュ使用を強制的に有効にする(iwad 2007.01.22修正)
	if(BackConsDay == 1 || AllDelete == FALSE)// Single Day or Single Change?
	{
		*DatePos = (unsigned char)BackDate;	// Set Date to be Inserted
		*(DatePos + 1) = (unsigned char)(BackDate / 256);
		return OyajiAddRecord(&BackWork, dwFileMode);// Add Record & Check
	}
	else
	{
	DWORD	i;// Counter
		for(i = 0; i < BackConsDay; i++)// Repeat Cons Days
		{
			*DatePos = (unsigned char)BackDate;	// Set Date to be Inserted
			*(DatePos + 1) = (unsigned char)(BackDate / 256);
			if(OyajiAddRecord(&BackWork, dwFileMode) == FALSE)
				return FALSE;// Add Cons Days Record
			if(Abort)
				return FALSE;
			GetNextDate(&BackDate);// Progress Date
		}
	}
	return TRUE;
}
//
//	Delete Record Sub
//
static BOOL DelRecordSub(BOOL AllDelete)
{
	dwFileMode = 1;	// データ破壊問題対策、キャッシュ使用を強制的に有効にする(iwad 2007.01.22修正)
	if(AllDelete == FALSE || BackConsDay == 1)			// Single Day?
		return OyajiDeleteRecord(&BackWork, dwFileMode);	// Single Delete
	else
	{	// Multi Date Delete
	DWORD	i;// Counter
		for(i = 0; i < BackConsDay; i++)
		{
			if(OyajiDeleteRecord(&BackWork, dwFileMode) == FALSE)
				return FALSE;
			if(Abort)
				return FALSE;
			GetNextDate(&BackDate);
		}
	}
	return TRUE;
}
//
//	Save & Restore Dialog Data (For Multi Change)
//	mode 0:Save 1:Restore
//
static void SetOrgDlgData(void)
{
DWORD	i;
	BackOrgDate = dwOldOrgDate;	// Set
	BackDate = BackOrgDate;
	BackStartTime = dwOldTime;
	BackConsDay = dwOldCons;
	BackAllDay = dwOldAllDay;
	BackGenID = dwOldGenID;
	for(i = 0; i < 14; i++)
		BackRepeat[i] = lpOldRepeat[i];
}
//
//	Get Next Repeat Day
//
BOOL MoveNextRepeat(LPDWORD DatePtr)
{
DWORD	i, j;	// Counter
DWORD	Freq = BackRepeat[2];
DWORD	EndDate = BackRepeat[6] * 256 + BackRepeat[5];
DWORD	WeekDay = BackRepeat[7];	// Day of Week
DWORD	ByPos = BackRepeat[8];
DWORD	Date = BackRepeat[10] * 256 + BackRepeat[9];
DWORD	WeekNum = BackRepeat[11];	// Number of Week
DWORD	Month = BackRepeat[13] * 256 + BackRepeat[12];
DWORD	CurDate, CurYear, CurMonth, CurDay, FinalDay;
	switch(BackRepeat[1])	// 1 = Mode
	{
	case 0:	// No Repeat?
		break;
	case 1:	// Daily
		for(i = 0; i < Freq; i++)	// 2 = Freq
			GetNextDate(DatePtr);
		if(*DatePtr <= EndDate)
			return TRUE;
	case 2:
		CurDay = ConvDateToWeekDay(*DatePtr);
		if(CurDay != 6)	// Not SaturDay?
		{
			for(i = CurDay; i < 6; i++)	// Seek Next Week Day
			{
				GetNextDate(DatePtr);
				if(DwordToBit(ConvDateToWeekDay(*DatePtr) + 1) & WeekDay)
					break;
			}
			if(i != 6 && *DatePtr <= EndDate)
				return TRUE;
		}
		for(i = 1; i < Freq; i++)
			for(j = 0; j < 7; j++)
				GetNextDate(DatePtr);
		for(i = 0; i < 7; i++)	// Seek Next Week Day
		{
			GetNextDate(DatePtr);
			if(DwordToBit(ConvDateToWeekDay(*DatePtr) + 1) & WeekDay)
				break;
		}
		if(*DatePtr <= EndDate)
			return TRUE;
		break;
	case 3:	// Monthly
		if(ByPos)
		{
			GetNextDate(DatePtr);
DaySeek:
			CurDate = *DatePtr;
			while(	(DwordToBit(ConvDateToWeekNum(*DatePtr) + 1) & WeekNum) == 0
				||	(DwordToBit(ConvDateToWeekDay(*DatePtr) + 1) & WeekDay) == 0)
				GetNextDate(DatePtr);
			if(*DatePtr > EndDate)
				return FALSE;
			if(((*DatePtr / 32) & 15) == ((CurDate / 32) & 15))	// Same Month?
				return TRUE;
			CurMonth = (*DatePtr / 32) & 15;	// Get Current Month
			CurYear = *DatePtr / 512;
			for(i = 1; i < Freq; i++)
			{
				if(CurMonth == 12)
				{
					CurMonth = 0;
					CurYear++;
					if(CurYear > 127)
						return FALSE;
				}
				CurMonth++;
			}
			*DatePtr = CurYear * 512 + CurMonth * 32 + 1;
			goto DaySeek;
		}
		else
		{	// By Date
			CurMonth = (*DatePtr / 32) & 15;	// Get Current Month
			CurYear = *DatePtr / 512;
			for(i = 0; i < Freq; i++)
			{
				if(CurMonth == 12)
				{
					CurMonth = 0;
					CurYear++;
					if(CurYear > 127)
						return FALSE;
				}
				CurMonth++;
			}
			FinalDay = GetFinalDay(CurYear * 512 + CurMonth * 32);
			if(FinalDay < (Date & 31))
				*DatePtr = CurYear * 512 + CurMonth * 32 + FinalDay;
			else
				*DatePtr = CurYear * 512 + CurMonth * 32 + (Date & 31);
		}
		if(*DatePtr <= EndDate)
			return TRUE;
		break;
	case 4:	// Yearly
		if(ByPos)
		{
			GetNextDate(DatePtr);
YearDaySeek:
			CurDate = *DatePtr / 512;
			while(	(DwordToBit((*DatePtr / 32) & 15) & Month) == 0
				||	(DwordToBit(ConvDateToWeekNum(*DatePtr) + 1) & WeekNum) == 0
				||	(DwordToBit(ConvDateToWeekDay(*DatePtr) + 1) & WeekDay) == 0)
				GetNextDate(DatePtr);
			if(*DatePtr > EndDate)
				return FALSE;
			if((*DatePtr / 512) == CurDate)	// Same Year?
				return TRUE;
			*DatePtr = (*DatePtr & 0xfffffe00) + (Freq - 1) * 512 + 0x21;// Set Next Year Jan 1st
			goto YearDaySeek;
		}
		else
		{	// By Date
			*DatePtr += Freq * 512;
			if(*DatePtr <= EndDate)
				return TRUE;
		}
		break;
	case 5:	// Custom
		if(ByPos)
		{
			CurDate = *DatePtr / 512;
			GetNextDate(DatePtr);
			while(	(DwordToBit((*DatePtr / 32) & 15) & Month) == 0
				||	(DwordToBit(ConvDateToWeekNum(*DatePtr) + 1) & WeekNum) == 0
				||	(DwordToBit(ConvDateToWeekDay(*DatePtr) + 1) & WeekDay) == 0)
				GetNextDate(DatePtr);
			if(*DatePtr > EndDate || (*DatePtr / 512) != CurDate)// Same Year?
				break;
			return TRUE;
		}
		else
		{	// By Day Number
			CurDate = *DatePtr / 512;
			CurMonth = (*DatePtr / 32) & 15;
CustomScan:
			CurMonth++;
			if(CurMonth == 13)
				return FALSE;	// Only One Year Scan
			if((DwordToBit(CurMonth) & Month) == 0)
				goto CustomScan;
			FinalDay = GetFinalDay(CurDate * 512 + CurMonth * 32);
			if(FinalDay < (Date & 31))
				*DatePtr = CurDate * 512 + CurMonth * 32 + FinalDay;
			else
				*DatePtr = CurDate * 512 + CurMonth * 32 + (Date & 31);
			return TRUE;
		}
	default:
		return FALSE;
	}
	return FALSE;
}
//
//	Get Repeat Start Date
//
DWORD GetRepeatStart(DWORD start)
{
	switch(dwRepeatMode)
	{
	case 0:	// None
	case 1:	// Daily
		break;	// Return Just the Day
	case 2:	// Weekly
		while((DwordToBit(ConvDateToWeekDay(start) + 1) & dwRepeatDay) == 0)
			GetNextDate(&start);
		break;
	case 3:	// Monthly
		if(dwRepeatByPos)	// By Position Mode?
		{
			start = (start & 0xffffffe0) + 1;	// Set 1st Day
			while(	(DwordToBit(ConvDateToWeekNum(start) + 1) & dwRepeatWeek) == 0
				||	(DwordToBit(ConvDateToWeekDay(start) + 1) & dwRepeatDay) == 0)
				GetNextDate(&start);
			break;		
		}
		else
		{
			if(GetFinalDay(start) < (dwRepeatDate & 31))
				return (start & 0xffffffe0) | GetFinalDay(start);
			else
				return (start & 0xffffffe0) | (dwRepeatDate & 31);
		}
		break;
	case 4:	// Yearly
		if(dwRepeatByPos)	// By Position Mode?
		{
			start = (start & 0xfffffe00) + 0x21;// Set Jan 1st of the Year
			while(	(DwordToBit(((start / 32) & 15)) & dwRepeatMonth) == 0
				||	(DwordToBit(ConvDateToWeekNum(start) + 1) & dwRepeatWeek) == 0
				||	(DwordToBit(ConvDateToWeekDay(start) + 1) & dwRepeatDay) == 0)
				GetNextDate(&start);
			break;		
		}
		else
			return (start & 0xfffffe00) | (dwRepeatDate & 511);// Set Month & Date
		break;
	case 5:	// Custom
		if(dwRepeatByPos)	// By Position Mode?
		{
			start = (start & 0xfffffe00) + 0x21;// Set Jan 1st of the Year
			while(	(DwordToBit(((start / 32) & 15)) & dwRepeatMonth) == 0
				||	(DwordToBit(ConvDateToWeekNum(start) + 1) & dwRepeatWeek) == 0
				||	(DwordToBit(ConvDateToWeekDay(start) + 1) & dwRepeatDay) == 0)
				GetNextDate(&start);
			break;		
		}
		else
		{
			start = (start & 0xfffffe00) + 0x21;// Set Jan 1st of the Year
			while((DwordToBit(((start / 32) & 15)) & dwRepeatMonth) == 0)
				GetNextDate(&start);
			if(GetFinalDay(start) < (dwRepeatDate & 31))
				return (start & 0xffffffe0) | GetFinalDay(start);
			else
				return (start & 0xffffffe0) | (dwRepeatDate & 31);
		}
		break;
	}
	return start;
}