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
#ifdef	_KCTRL
#include	"kctrl.h"
#endif
//
//	Macros
//
#define	countof(x)	(sizeof(x)/(sizeof((x)[0])))

//
//	ProtTypes
//
void	DrawFrame(HDC, DWORD, DWORD, DWORD, DWORD);
void	DispToDoCursor(HDC);
void	SetupBrush(HDC);
void	GetToday(void);
DWORD	GetCurrentYear(DWORD);
DWORD	GetCurrentMonth(DWORD);
DWORD	GetCurrentDay(DWORD);
DWORD	GetCurrentWeekDay(void);
DWORD	GetCurrentDate(void);
DWORD	GetTime(DWORD, DWORD);
BOOL	OyajiSeekGetRecord(FILEWORKS *);
DWORD	OyajiSearchRecord(FILEWORKS *);
BOOL	OyajiGetNextRecord(FILEWORKS *);
void	GetNextDate(LPDWORD);
void	GetPrevDate(LPDWORD);
DWORD	GetSJIS(LPTSTR, char *, DWORD);
void	GetUniCode(char *, LPTSTR);
void	SetupMenu(BOOL);
BOOL	OyajiUndoFile(FILEWORKS *);
void	SetupMainScreen(HDC);
void	SetupToDoArray(void);
void	DrawFrame(HDC, DWORD, DWORD, DWORD, DWORD);
//
//	Global Vars
//
static DWORD	LineHight, LineWidth;	// Day Line Cursor Size
static DWORD	FrameWidth;				// Day Screen Frame Width
static DWORD	dwOldLine;				// Old Line Cursor Pos
static DWORD	LineLeft;				// Line Cursor Left Pos
static DWORD	ToDoLineCount;			// Line Cursor Left Pos
static DWORD	MaxToDoLine;			// Max ToDo Lines
static DWORD	ToDoShift;				// ToDo Start Line Skip Number
//
//	Setup Day Display Screen to Memory Device
//
void ToDoScreen(HDC hDC, BOOL Setup)
{
POINT	LinePoint[4];	// Point Structure for Line
RECT	txRect;		// Rect for DrawText
TCHAR	TimeStr[8];	//	Time String Buffer
DWORD	Today;		// Today
DWORD	TargetDay;	// Display Limit Date
DWORD	DispMin = 0;// Start Time Work
DWORD	DispYear, DispMonth, DispDay;	// Temp Date Number
DWORD	i, j;	// Temp Counter
TCHAR	TempStr[256];// Title Buffer
int		PastDay;	// ToDo Past Day
	FrameWidth = dwXsize - 2;		// Get LineBox Width
//	Setup Dimensions
	SetupBrush(hDC);
	PatBlt(hDC,	0, 0, dwXsize, dwYsize, PATCOPY);
//	Calc Cursor Size
	ToDoLineCount = (dwYsize - dwSmallFontHight - 6) / dwBigFontHight;// Get Lines
	if(ToDoLineCount >= MAXDAYLINE)
		ToDoLineCount = MAXDAYLINE;
	LineHight = dwBigFontHight;// Get LineBox Height
	LineLeft = dwBigFontWidth * 29 + 1;	// Get Left Pos
	LineWidth = FrameWidth - LineLeft;	// Get LineBox Width
//	Draw Today's Date in English
	DrawFrame(hDC, 0, 0, FrameWidth, 4 + dwSmallFontHight + LineHight * ToDoLineCount);
	LinePoint[0].x = 0;				// Frame Left Top Axis
	LinePoint[1].x = FrameWidth;	// Frame Left Top Axis
	// [iwad] LinePoint[0].y = LinePoint[1].y = dwSmallFontHight + 1;
	LinePoint[0].y = LinePoint[1].y = dwSmallFontHight + 5;
	Polyline(hDC, LinePoint, 2);	// Draw Todays Line
	SelectObject(hDC, hSmallFont);		// Set Font (Small)
	// [iwad] txRect.top = 1;
	txRect.top = 5;
	// [iwad] txRect.bottom = dwSmallFontHight ;
	txRect.bottom = dwSmallFontHight + 4;
	txRect.left = 1;
	txRect.right = FrameWidth - 2;
	if(dwDateFormat)
		wsprintf(TempStr, TEXT("%s %s %d, %d"), 
		DayName[GetCurrentWeekDay()],
		MonthName[GetCurrentMonth(0) - 1],
		GetCurrentDay(0),
		GetCurrentYear(0));
	else
		wsprintf(TempStr, TEXT("%d / %d / %d (%s)"), 
		GetCurrentYear(0),
		GetCurrentMonth(0),
		GetCurrentDay(0),
		ShortDayName[GetCurrentWeekDay()]);
	DrawText(hDC, TempStr, -1, &txRect, DT_CENTER | DT_VCENTER);
	if(dwPrivate)
		// [iwad] PatBlt(hDC, 1, 1, FrameWidth - 1, dwSmallFontHight + 1, PATINVERT);
		PatBlt(hDC, 1, 1, FrameWidth - 1, dwSmallFontHight + 5, PATINVERT);
	//
	SelectObject(hDC, hBigFont);		// Set Font (Small)
	for(i = 0; i < ToDoLineCount; i++)
		DayOrgIDBuf[i] = 0;	// Clear Buffer
	GetToday();
	Today = (dwTodayYear - 1950) * 512 + dwTodayMonth * 32 + dwTodayDay;
	if(Setup)
		SetupToDoArray();
	if(MaxToDoLine == 0)	// No Record?
	{
		SetupMenu(FALSE);	// Button Set
		return;
	}
	for(i = 0; i < ToDoLineCount; i++)	// Repeat for Day Lines (Horizontal Line)
	{
		j = i + ToDoShift;
		if(j == MAXDAYBUF)
			break;
		if(DayGenID[0][j] == 0)
			break;
//	Set Buffer for Cursor Pickup
		DayOrgTimeBuf[i] = DayStartTime[0][j];	// Original Time List(14 Lines)
		DayOrgIDBuf[i] = DayGenID[0][j];		// Day Seek Ptr List(14 Lines)
		DayOrgAllDayBuf[i] = DayAllDay[0][j];	// All Day Flag List
		DayOrgDateBuf[i] = ToDoDate[j];			// Date Buffer (For ToDo Screen)
//	Draw Date
		TargetDay = DayOrgDateBuf[i];				// Get Target Date
		DispYear = TargetDay / 512 + 1950;
		DispMonth = TargetDay / 32 & 15;
		DispDay = TargetDay & 31;
		// [iwad] txRect.top = dwSmallFontHight + 2 + i * LineHight;
		txRect.top = dwSmallFontHight + 2 + i * LineHight + 4;
		// [iwad] txRect.bottom = txRect.top + LineHight;
		txRect.bottom = txRect.top + LineHight + 4;
		txRect.left = dwBigFontWidth;
		txRect.right = txRect.left + 14 * dwBigFontWidth;
		if(dwDateFormat)// US Format?
			wsprintf(TempStr, TEXT("%s %d %d"),	// Make Month & Year String
				ShortMonthStr[DispMonth - 1], DispDay, DispYear);
		else	// Japanese Format
			wsprintf(TempStr, TEXT("%d/%d/%d"),		// Make Month & Year String
				DispYear, DispMonth, DispDay);
		DrawText(hDC, TempStr, -1, &txRect, DT_LEFT | DT_VCENTER);
//	Draw Time
		DispHour = DayOrgTimeBuf[i];
		DispMin = DispHour & 63;
		DispHour /= 64;
		if(DayOrgAllDayBuf[i])
		{
			if(dwHourMode)	// 24H Mode?
			{
				wsprintf(TimeStr, TEXT("%2d:%2.2d"), DispHour, DispMin);
			}
			else
			{
				if(DispHour > 24)
					wsprintf(TimeStr, TEXT("%2d:%2.2dm"), DispHour - 24, DispMin);
				else if(DispHour > 12)
					wsprintf(TimeStr, TEXT("%2d:%2.2dp"), DispHour - 12, DispMin);
				else
					wsprintf(TimeStr, TEXT("%2d:%2.2da"), DispHour, DispMin);
			}
			txRect.left = 15 * dwBigFontWidth;
			txRect.right = txRect.left + 6 * dwBigFontWidth;
			DrawText(hDC, TimeStr, -1, &txRect, DT_RIGHT | DT_VCENTER);
		}
		PastDay = 0;
		if(TargetDay < Today)	// Past ToDo?
			while(TargetDay < Today)
			{
				GetNextDate(&TargetDay);
				PastDay--;
			}
		else
			while(TargetDay > Today)
			{
				GetPrevDate(&TargetDay);
				PastDay++;
			}
		wsprintf(TempStr, TEXT("%d"), PastDay);		// Make Month & Year String
		txRect.left = 21 * dwBigFontWidth;
		txRect.right = txRect.left + 7 * dwBigFontWidth;
		DrawText(hDC, TempStr, -1, &txRect, DT_RIGHT | DT_VCENTER);
//	Draw Title of the Reecord
		txRect.left = LineLeft;
		txRect.right = FrameWidth - 1;
		DrawText(hDC, DayTitle[0][j], -1, &txRect, DT_LEFT | DT_VCENTER | DT_NOPREFIX);
	}
	dwOldLine = ToDoLineCount;
	DispToDoCursor(hDC);
}
//
//	Xor ToDo Line Cursor
//
void ReverseToDoBox(HDC hDC, DWORD y)
{
	PatBlt(hDC,
			LineLeft,
			// [iwad] y * LineHight + dwSmallFontHight + 2,
			y * LineHight + dwSmallFontHight + 8,
			LineWidth,
			dwBigFontHight, PATINVERT);
}
//
//	Reverse ToDo Cursor
//
void DispToDoCursor(HDC hDC)
{
	if(dwOldLine != ToDoLineCount)
		ReverseToDoBox(hDC, dwOldLine);
	ReverseToDoBox(hDC, dwDayLine);
	SetupMenu(FALSE);
	dwOldLine = dwDayLine;
}
//
//	Setup ToDo Array
//
void SetupToDoArray(void)
{
DWORD	RangeDay, ToDay, TempDay;
int		i, j, FullRange;
TCHAR	TempStr[256];
	FullRange = (GetAsyncKeyState(VK_SHIFT) < 0);
	GetToday();
	RangeDay = (dwTodayYear - 1950) * 512 + dwTodayMonth * 32 + dwTodayDay;
	ToDay = RangeDay;
	for(i = 0; i < MAXDAYBUF; i++)
		DayGenID[0][i] = 0;	// Clear Buffer
//	For Blank ToDo Input
	dwDayLine = 0;		// Set Cursor Top
	DayOrgTimeBuf[0] = GetTime(dwStartTime, 0);// Set Time
	DayOrgIDBuf[0] = 0;	// Set Record ID
	DayOrgAllDayBuf[0] = 1;// Set All Day Flag
	for(i = 0; i < (int )dwToDoRange; i++)
		GetNextDate(&RangeDay);	// Get ToDo Range
	ToDoShift = 0;
	MaxToDoLine = 0;	// Clear Max ToDo Line
//	Setup ToDo Array
	dwDlgDate = 0;
	dwDlgStartTime = 0;
	dwGenID = 0;
	dwAllDay = 0;
	OyajiSearchRecord(&MainWork);	// Seek To Top Record
	while(OyajiGetNextRecord(&MainWork))
	{
		if(dwDlgToDo == 0)		// ToDo Checked?
			continue;			// Search Next
		if(dwGenID == HideGenID)// Now Deleting?
			continue;			// Search Next
		if(FullRange == 0 && dwDlgDate > RangeDay)	// Finish Rage Day Over
			break;
		if(dwDlgPrivate != dwPrivate)// Main & Private
			continue;
		if(FullRange == 0 && dwDlgEnable == 0 && dwDlgLeadTime < dwToDoRange)	// Custom Range
		{
			TempDay = dwDlgDate;	// Make Copy of Record Date
			for(i = 0; i < (int )dwDlgLeadTime; i++)// Get Range Date
				GetPrevDate(&TempDay);
			if(ToDay < TempDay)	// Check in Range
				continue;	// Pass
		}
//	Find Insert Position
		if(dwToDoSort)
		{
			for(i = 0; i < (int )MaxToDoLine; i++)
			{
				if(DayStartTime[0][i] > dwDlgStartTime)// Check Priority
					break;
			}
//	Move ToDo Array
			for(j = MaxToDoLine; j >= i; j--)
			{
				DayGenID[0][j + 1] = DayGenID[0][j];// Clear Current Line Seek Ptr
				DayAllDay[0][j + 1] = DayAllDay[0][j];	// Save Original AllDay Flag
				DayStartTime[0][j + 1] = DayStartTime[0][j];// Save (Hidden) Start Time
				DayEndTime[0][j + 1] = DayEndTime[0][j];// Save (Hidden) Start Time
				DayAlarm[j + 1] = DayAlarm[j];
				DayToDo[j + 1] = DayToDo[j];
				ToDoDate[j + 1] = ToDoDate[j];
				_tcscpy(DayTitle[0][j + 1], DayTitle[0][j]);
			}
		}
		else
			i = MaxToDoLine;
		DayGenID[0][i] = dwGenID;	// Clear Current Line Seek Ptr
		DayAllDay[0][i] = dwAllDay;	// Save Original AllDay Flag
		DayStartTime[0][i] = dwDlgStartTime;// Save (Hidden) Start Time
		DayEndTime[0][i] = dwDlgEndTime;// Save (Hidden) Start Time
		DayAlarm[i] = dwDlgEnable;
		DayToDo[i] = TRUE;
		ToDoDate[i] = dwDlgDate;
//	Setup Title of the Record
		GetUniCode(lpDlgTitle, TempStr);
#ifdef	_WIN32_WCE
		if(lpDlgLocation[0] != TEXT('\0'))
		{
		TCHAR	LocBuf[33];	// Location Buf
			GetUniCode(lpDlgLocation, LocBuf);
			_tcscat(TempStr, TEXT(" @"));
			_tcscat(TempStr, LocBuf);
		}
#else
		if(lpDlgLocation[0] != TEXT('\0'))
		{
			_tcscat(TempStr, TEXT(" @"));
			_tcscat(TempStr, lpDlgLocation);
		}
#endif
		if(_tcslen(TempStr) >= 64)	// Too Long?
			TempStr[63] = TEXT('\0');
		_tcscpy(DayTitle[0][i], TempStr);
		MaxToDoLine++;
		if(MaxToDoLine == MAXDAYBUF)
			break;
	}
}
//
//	Draw ToDo in the Area
//
void DrawToDo(HDC hDC, DWORD Left, DWORD Top, DWORD Width, DWORD Hight)
{
DWORD	i, Lines, Today, TargetDay;
TCHAR	Mark;
TCHAR	TempStr[256];// Title Buffer
RECT	txRect;
	GetToday();
	Today = (dwTodayYear - 1950) * 512 + dwTodayMonth * 32 + dwTodayDay;
	DrawFrame(hDC, Left, Top, Width, Hight);	// Draw Frame
	Lines = Hight / dwSmallFontHight;
	SetupToDoArray();
	if(MaxToDoLine == 0)	// No Record?
		return;
	for(i = 0; i < Lines; i++)
	{
		if(i == MAXDAYLINE)
			break;
		if(DayGenID[0][i] == 0)
			break;
//	Set Buffer for Curso6r Pickup
		TargetDay = ToDoDate[i];
		if(TargetDay < Today)	// Past ToDo?
			Mark = TEXT('!');
		else if(TargetDay == Today)
			Mark = TEXT('*');
		else
			Mark = TEXT(' ');
		wsprintf(TempStr, TEXT("%c %s"), Mark, DayTitle[0][i]);
		txRect.left = Left + 1;
		txRect.right = txRect.left + Width - 1;
		// [iwad] txRect.top = Top + i * dwSmallFontHight + 1;
		txRect.top = Top + i * dwSmallFontHight + 5;
		// [iwad] txRect.bottom = txRect.top + dwSmallFontHight;
		txRect.bottom = txRect.top + dwSmallFontHight + 4;
		DrawText(hDC, TempStr, -1, &txRect, DT_LEFT | DT_VCENTER);
	}
}
//
//	ToDo Screen Key Proc
//
void ToDoKeyProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case VK_BACK:
		break;
	case TEXT('\r'):	// Enter
		if(DayOrgIDBuf[dwDayLine])
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_INPUT, NULL);
		break;
	case TEXT('\033'):	// Change to Day Screen(ESC)
		DispMode = ToDoBackMode;
		switch(DispMode)
		{
		case 0:	// Back to Half Year
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_HALFYEAR, 0);
			break;
		case 1:	// Back to Month
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_MONTH, 0);
			break;
		case 2:	// Back to Week
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_WEEK, 0);
			break;
		case 3:	// Back to Day
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_DAY, 0);
			break;
		case 4:	// Back to Month
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_INPUT, 0);
			break;
		}
		break;
	case TEXT('\032'):	// Ctrl+Z?
		OyajiUndoFile(&MainWork);
		SetupMainScreen(hMemDC);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
}
//
//	ToDo Screen Key Down Proc
//
void ToDoKeyDownProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
	if(wParam == VK_UP)
	{
		if(dwDayLine)
			dwDayLine--;
		else if(ToDoShift)
		{
			ToDoShift--;
			goto ScrollEnd;
		}
		goto ShiftEnd;
	}
	else if(wParam == VK_DOWN)
	{
		if(dwDayLine == ToDoLineCount - 1)
		{
			if(ToDoLineCount + ToDoShift < MaxToDoLine)
			{
				ToDoShift++;
ScrollEnd:
				ToDoScreen(hDC, FALSE);// Not Re-Setup Array
				InvalidateRect(hWnd, NULL, FALSE);
				return;
			}
		}
		else
		{
			if(MaxToDoLine && dwDayLine < MaxToDoLine - 1)
				dwDayLine++;
ShiftEnd:
			DispToDoCursor(hDC);
			InvalidateRect(hWnd, NULL, FALSE);
			return;
		}
	}
}
//
//	ToDo Mouse Left Button Proc
//
void ToDoLeftProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
DWORD	Top = dwSmallFontHight + dwWinTop + 2;
	if(LOWORD(lParam) >= FrameWidth || HIWORD(lParam) <= Top)
		return;
	if(MaxToDoLine == 0)
		return;
	dwDayLine = (HIWORD(lParam) - Top) / LineHight;
	if(dwDayLine > ToDoLineCount - 1)
		dwDayLine = ToDoLineCount - 1;
	if(dwDayLine >= MaxToDoLine - ToDoShift)
		dwDayLine = MaxToDoLine - ToDoShift - 1;
	DispToDoCursor(hDC);
	InvalidateRect(hWnd, NULL, FALSE);
}
//
//	ToDo Double Click Proc
//
void ToDoDblProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
DWORD	Top = dwSmallFontHight + dwWinTop + 2;
	if(HIWORD(lParam) <= Top || LOWORD(lParam) >= FrameWidth)
		return;
	if(MaxToDoLine == 0)
		return;
	dwDayLine = (HIWORD(lParam) - Top) / LineHight;
	if(dwDayLine > ToDoLineCount - 1)
		dwDayLine = ToDoLineCount - 1;
	if(dwDayLine >= MaxToDoLine - ToDoShift)
		dwDayLine = MaxToDoLine - ToDoShift - 1;
	SendMessage(hWnd, WM_CHAR, TEXT('\r'), NULL);
}
