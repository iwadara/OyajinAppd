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
void	SetupMonth(DWORD, DWORD);
void	JumpDay(DWORD, DWORD, DWORD);
void	JumpCurrentDay(DWORD);
void	DispHalfCalBox(HDC);
void	ReverseHalfCalBox(HDC, DWORD, DWORD);
void	DispHalfLineCursor(HDC);
void	ReverseHalfBox(HDC, DWORD);
void	DrawFrame(HDC, DWORD, DWORD, DWORD, DWORD);
void	DrawToDo(HDC, DWORD, DWORD, DWORD, DWORD);
void	HalfScreen(HDC, BOOL, BOOL, BOOL);
void	HalfKeyProc(HWND, HDC, WPARAM, LPARAM);
void	SetupMainScreen(HDC);
void	SetupMenu(BOOL);
void	GetToday(void);
void	GetUniCode(char *, LPTSTR);
void	SetupBrush(HDC);
LPCTSTR	GetDateString(DWORD);
LPCTSTR	GetTimeString(DWORD);
DWORD	GetCurrentYear(DWORD);
DWORD	GetCurrentMonth(DWORD);
DWORD	GetCurrentDay(DWORD);
DWORD	GetCurrentWeekDay(void);
DWORD	GetCurrentDate(void);
DWORD	GetDayTotal(void);
DWORD	OyajiSearchRecord(FILEWORKS *);
DWORD	ConvTimeToMin(DWORD);
DWORD	ConvMinToTime(DWORD);
DWORD	AddTime(DWORD, DWORD);
DWORD	SubTime(DWORD, DWORD);
DWORD	GetDate(DWORD, DWORD, DWORD);
DWORD	GetTime(DWORD, DWORD);
BOOL	OyajiGetRecord(FILEWORKS *);
BOOL	OyajiSeekGetRecord(FILEWORKS *);
BOOL	OyajiUndoFile(FILEWORKS *);
BOOL	ConvTimeToDword(LPDWORD, LPTSTR);
BOOL	ConvDateToDword(LPDWORD, LPTSTR);
void	AdvHalfHour(LPDWORD);
void	SetupOneDayArray(DWORD, DWORD, BOOL, DWORD);
DWORD	ConvHalfHour(DWORD);
BOOL	SetupClipData(BOOL);
//
//	Global Vars
//
static DWORD	LineHight, LineWidth;	// Day Line Cursor Size
static DWORD	FrameWidth;				// Day Screen Frame Width
static DWORD	dwOldLine;				// Old Line Cursor Pos
static DWORD	LineLeft;				// Line Cursor Left Pos
static DWORD	CalWidth;				// Cal Width to be Displayed
//
//	Setup Half Year Display Screen to Memory Device
//
void HalfScreen(HDC hDC, BOOL DrawCal, BOOL Scroll, BOOL SetCursor)
{
POINT	LinePoint[4];	// Point Structure for Line
RECT	txRect;		// Rect for DrawText
DWORD	Year, Month;	// Work for Day Count and Calender
//LPCTSTR	ShortDayStr[] = {TEXT("Su"), TEXT("Mo"), TEXT("Tu"), TEXT("We"),TEXT("Th"), TEXT("Fr"), TEXT("Sa")};
LPCTSTR	ShortDayStr[] = {TEXT("“ú"), TEXT("ŒŽ"), TEXT("‰Î"), TEXT("…"),TEXT("–Ø"), TEXT("‹à"), TEXT("“y")};	// by uminchu
DWORD	Today;
DWORD	DispMin = 0;	// Start Time Work
DWORD	APMode;		// Am Pm Dispay Mode
DWORD	i, j, k;	// Temp Counter
TCHAR	TempStr[256];// Title Buffer
TCHAR	TimeStr[8];
DWORD	CurrentPage;
//	Setup Dimensions (Get Calender Width)
	for(CalWidth = 3; CalWidth > 0; CalWidth--)
	{
		FrameWidth = dwXsize - dwSmallFontWidth * 22 * CalWidth;// Get LineBox Width
		if((int )FrameWidth > dwSmallFontWidth * 16)
			break;
	}
//	Setup Dimensions
	SetupBrush(hDC);
	if(DrawCal)
		PatBlt(hDC, 0, 0, dwXsize, dwYsize, PATCOPY);	// Clear Memory Device
	else		// Clear Only Schedule Part
		PatBlt(hDC,	1, 1, FrameWidth - 1, dwYsize - 1, PATCOPY);
//	Calc Cursor Size
	GetToday();
	DayLineCount = (dwYsize - 2) / dwSmallFontHight - 1;// Get Lines
	if(DayLineCount > dwFinalTime * (dwDispStep + 1))	// 00:00 to 31:00 ?
		DayLineCount = dwFinalTime * (dwDispStep + 1) + 1;	// Force Set 00:00 to 31:00
	if(DayLineCount >= MAXDAYLINE)
		DayLineCount = MAXDAYLINE;
	LineHight = dwSmallFontHight;	// Get LineBox Height
	LineWidth = FrameWidth - 2;// Get LineBox Width
//	Draw Day Frame
	DrawFrame(hDC, 0, 0, FrameWidth, LineHight * (DayLineCount + 1) + 2);
	SelectObject(hDC, hSmallFont);
	LinePoint[0].x = 0;				// Frame Left Top Axis
	LinePoint[1].x = FrameWidth;	// Frame Left Top Axis
	LinePoint[0].y = LinePoint[1].y = dwSmallFontHight;
	Polyline(hDC, LinePoint, 2);	// Draw Todays Line
	txRect.top = 1;
	txRect.bottom = dwSmallFontHight ;
	txRect.left = 1;
	txRect.right = FrameWidth - 2;
	CurrentPage = dwCurY / 6;
	if(dwDateFormat)
		wsprintf(TempStr, TEXT("%s %s %d, %2.2d"), 
		ShortDayName[GetCurrentWeekDay()],
		ShortMonthStr[GetCurrentMonth(CurrentPage) - 1],
		GetCurrentDay(CurrentPage),
		GetCurrentYear(CurrentPage) % 100);
	else
		wsprintf(TempStr, TEXT("%d/%d/%d (%s)"), 
		GetCurrentYear(CurrentPage),
		GetCurrentMonth(CurrentPage),
		GetCurrentDay(CurrentPage),
		ShortDayName[GetCurrentWeekDay()]);
	DrawText(hDC, TempStr, -1, &txRect, DT_CENTER | DT_VCENTER);
	if(dwPrivate)
		PatBlt(hDC, 1, 1, FrameWidth - 1, dwSmallFontHight, PATINVERT);
	if(DrawCal)
	{
//	Draw Calender TEXT
		for(j = 0; j < 6; j++)
		{
//	Get Index of Array
			if(dwToDoHalf && j == 5)
				break;
			if(j == 0)
				k = 5;	// Left Top is Prev Month
			else
				k = j - 1;
			Year = MonthBuf[k][15][0];	// Get Current Year (Center)
			Month = MonthBuf[k][15][1];	//	Get Current Month (Center)
			TempStr[2] = TEXT('\0');	// Set Null Char for Calender Date
			for(i = 0;i < 42; i++)
			{
				if(MonthBuf[k][i][1] != Month)
					continue;
				txRect.left =
					((j % 3) * 22 + (i % 7) * 3 + 1) * dwSmallFontWidth + FrameWidth;
				txRect.top = ((j / 3) * 8 + i / 7 + 2) * dwSmallFontHight;
				txRect.bottom = txRect.top + dwSmallFontHight;
				txRect.right = txRect.left + dwSmallFontWidth * 5 / 2;
				if(MonthBuf[k][i][2] < 10)
					TempStr[0] = TEXT(' ');
				else
					TempStr[0] = (TCHAR )MonthBuf[k][i][2] / 10 + TEXT('0');
				TempStr[1] = (TCHAR )MonthBuf[k][i][2] % 10 + TEXT('0');

			// [iwad] “y“úƒJƒ‰[‘Î‰ž
				if ((i % 7) == 0)	// Sunday
				{
					SetTextColor(hDC, RGB(255, 0, 0));
					DrawText(hDC, TempStr, -1, &txRect, DT_VCENTER);
					if(dwTextColor)
						SetTextColor(hDC, dwTextColor);
					else
						SetTextColor(hDC, RGB(0, 0, 0));
				}
				else if ((i % 7) == 6)	// Saturday
				{
					SetTextColor(hDC, RGB(0, 0, 255));
					DrawText(hDC, TempStr, -1, &txRect, DT_VCENTER);
					if(dwTextColor)
						SetTextColor(hDC, dwTextColor);
					else
						SetTextColor(hDC, RGB(0, 0, 0));
				}
				else
				{
					DrawText(hDC, TempStr, -1, &txRect, DT_VCENTER);
				}	

			#if 0
				DrawText(hDC, TempStr, -1, &txRect, DT_VCENTER);
			#endif

				dwDlgDate = GetDate(MonthBuf[k][i][0], MonthBuf[k][i][1], MonthBuf[k][i][2]);
				dwDlgStartTime = 0;
				dwAllDay = 0;
				dwGenID = 0;
				if(dwUnderBar && dwPrivate == 0 && OyajiSearchRecord(&MainWork))
				{	// Search Main File & Key is dwDlg Works
					LinePoint[0].x = 	// Frame Left Top Axis
						((j % 3) * 22 + (i % 7) * 3 + 1) * dwSmallFontWidth + FrameWidth;
					LinePoint[1].x = 
						LinePoint[0].x + dwSmallFontWidth * 2; 
					LinePoint[0].y = LinePoint[1].y = 
						((j / 3) * 8 + i / 7 + 3) * dwSmallFontHight - 1;
					Polyline(hDC, LinePoint, 2);	// Draw Todays Line
				}
				if(MonthBuf[k][i][0] == dwTodayYear
				&& MonthBuf[k][i][1] == dwTodayMonth
				&& MonthBuf[k][i][2] == dwTodayDay)
				DrawFrame(hDC,
					((j % 3) * 22 + (i % 7) * 3 + 1) * dwSmallFontWidth + FrameWidth - 1,
					((j / 3) * 8 + i / 7 + 2) * dwSmallFontHight,
					dwSmallFontWidth * 5 / 2,
					dwSmallFontHight - 1);
			}
//	Draw Day Indicator
			txRect.top = ((j / 3) * 8 + 1) * dwSmallFontHight;
			txRect.bottom = txRect.top + dwSmallFontHight;

		// [iwad] “y“úƒJƒ‰[‘Î‰ž
			for(i = 0;i < 7; i++)
			{
				txRect.left =
					((j % 3) * 22 + (i % 7) * 3 + 1) * dwSmallFontWidth + FrameWidth;
				txRect.right = txRect.left + dwSmallFontWidth * 3;
				if (i == 0)	// Sunday
				{
					SetTextColor(hDC, RGB(255, 0, 0));
					DrawText(hDC, ShortDayStr[i], -1, &txRect, DT_VCENTER);
					if(dwTextColor)
						SetTextColor(hDC, dwTextColor);
					else
						SetTextColor(hDC, RGB(0, 0, 0));
				}
				else if (i == 6)	// Saturday
				{
					SetTextColor(hDC, RGB(0, 0, 255));
					DrawText(hDC, ShortDayStr[i], -1, &txRect, DT_VCENTER);
					if(dwTextColor)
						SetTextColor(hDC, dwTextColor);
					else
						SetTextColor(hDC, RGB(0, 0, 0));
				}
				else
				{
					DrawText(hDC, ShortDayStr[i], -1, &txRect, DT_VCENTER);
				}	
			}
		#if 0
			for(i = 0;i < 7; i++)
			{
				txRect.left =
					((j % 3) * 22 + (i % 7) * 3 + 1) * dwSmallFontWidth + FrameWidth;
				txRect.right = txRect.left + dwSmallFontWidth * 3;
				DrawText(hDC, ShortDayStr[i], -1, &txRect, DT_VCENTER);
			}
		#endif
//	Draw Month and Year
			txRect.top = (j / 3) * dwSmallFontHight * 8;
			txRect.bottom = txRect.top + dwSmallFontHight;
			txRect.left = ((j % 3) * 22 + 1) * dwSmallFontWidth + FrameWidth;
			txRect.right = txRect.left + 21 * dwSmallFontWidth;
			if(dwDateFormat)
				wsprintf(TempStr, TEXT("%s %d"), MonthName[Month - 1], Year);
			else
				wsprintf(TempStr, TEXT("%d / %d"), Year, Month);
			DrawText(hDC, TempStr, -1, &txRect, DT_CENTER | DT_VCENTER);
//	Draw Frame
			DrawFrame(hDC,
				(j % 3) * 22 * dwSmallFontWidth + FrameWidth + dwSmallFontWidth / 4,	// Frame Left Top Axis
				(j / 3) * dwSmallFontHight * 8,
				dwSmallFontWidth * 21 + dwSmallFontWidth / 2,
				dwSmallFontHight * 8);
		}
		if(dwToDoHalf)	// ToDo on Day Screen?
			DrawToDo(hDC,
				(j % 3) * 22 * dwSmallFontWidth + FrameWidth + dwSmallFontWidth / 4,	// Frame Left Top Axis
				(j / 3) * dwSmallFontHight * 8,
				dwSmallFontWidth * 21 + dwSmallFontWidth / 2,
				dwSmallFontHight * 8);
		dwOldCurX = 7;
		DispHalfCalBox(hDC);
	}
	if(Scroll)	// Scroll Mode?
		goto DispAgain;	// Skip Array Setup
	Today = GetCurrentDate();	// Set Search Day
//
	AllDayHight = 0;// All Day Scedule Line Number
	SetupOneDayArray(Today, 0, TRUE, 0);	// Setup One Day Array Index = 0 DayMode = 0
//	Search Start Line
	for(i = DayAllNum[0]; i < (DayAllNum[0] + DayOneNum[0]); i++)// Scan Disp Buffer
		if(ConvHalfHour(DayStartTime[0][i]) >= StartTime)
			break;	// Get Display Start Time Found
	StartIndex = i - DayAllNum[0];
	AllStartIndex = 0;
//	Adjust Cur Pos
	HideSkip = 0;	// Clear if Hide Mode
	if(dwAppDispMode && dwDayLine > ActualNum[0])	// If Hide Mode & Cursor Over?
		dwDayLine = ActualNum[0];	// Set Cursor Bottom
DispAgain:
	StartTime = ConvHalfHour(DayStartTime[0][DayAllNum[0] + StartIndex]);
	LineLeft = 1;	// Left Pos of Line Cursor
	if(DayAllNum[0] > dwTopDayLine)
		AllDayHight = dwTopDayLine;	// Maximum AllDay Disp Line is 3
	else
		AllDayHight = DayAllNum[0];// All Day Scedule Line Number
	if(DayAllNum[0] && dwAppDispMode == 0)	// All Day Scedule Exists & All Display Mode?
	{
		for(i = 0; i < AllDayHight; i++)
		{
//	Draw Title of the Record
			txRect.left = dwSmallFontWidth * 8 + 1;
			txRect.right = FrameWidth - 1;
			txRect.top = (i + 1) * LineHight + 1;
			txRect.bottom = txRect.top + LineHight;
			if(DayGenID[0][i + AllStartIndex])
			{
				_tcscpy(TempStr, TEXT(""));
				if(
				(AllDayHight < DayAllNum[0])
				&&
				(((AllStartIndex + AllDayHight < DayAllNum[0]) && (i == dwTopDayLine - 1))
				||
				((AllStartIndex != 0) && (i == 0)))
				)
					_tcscat(TempStr, TEXT("..."));	// Continue Mark
				if(DayToDo[i + AllStartIndex])
					_tcscat(TempStr, TEXT("!"));
				_tcscat(TempStr, DayTitle[0][i + AllStartIndex]);
				DrawText(hDC, TempStr, -1, &txRect, DT_LEFT | DT_VCENTER | DT_NOPREFIX);
			}
			DayOrgIDBuf[i] = DayGenID[0][i + AllStartIndex];	// Clear Current Line Seek Ptr
			DayOrgAllDayBuf[i] = DayAllDay[0][i + AllStartIndex];	// Save Original AllDay Flag
			DayOrgTimeBuf[i] = DayStartTime[0][i + AllStartIndex];
		}
	}
	APMode = 0;
	if(DayStartTime[0][DayAllNum[0] + StartIndex] >= GetTime(13, 0) && dwAppDispMode == 0)
		APMode = 2;			// PM Char
	if(dwAppDispMode)	// Hide Mode?
	{
		for(i = 0; i < DayLineCount; i++)
		{
			DayOrgIDBuf[i] = 0;	// Clear Buffer
			DayOrgTimeBuf[i] = GetTime(dwStartTime, 0);	// Set Default Time
		}
		i = 0;
		k = HideSkip;
		for(j = 0; j < DayAllNum[0] + DayOneNum[0]; j++)	// Repeat for Day Lines (Horizontal Line)
		{
			if(DayGenID[0][j] == 0)
				continue;
			if(k)	// Skip Leading Record
			{
				k--;
				continue;
			}
			DispHour = DayStartTime[0][j];
			DispMin = DispHour & 63;
			DispHour /= 64;
			if(DayAllDay[0][j])
			{
				if(dwHourMode)	// 24H Mode?
				{
					wsprintf(TimeStr, TEXT("%2d:%2.2d"), DispHour, DispMin);
				}
				else
				{
					if(DispHour > 24)
						wsprintf(TimeStr, TEXT("%2d:%2.2d"), DispHour - 24, DispMin);
					else if(DispHour > 12)
						wsprintf(TimeStr, TEXT("%2d:%2.2d"), DispHour - 12, DispMin);
					else
						wsprintf(TimeStr, TEXT("%2d:%2.2d"), DispHour, DispMin);
				}
				txRect.top = LineHight * (i + 1) + 1;
				txRect.bottom = txRect.top + LineHight;
				txRect.left = 1;
				txRect.right = txRect.left + 6 * dwSmallFontWidth;
				DrawText(hDC, TimeStr, -1, &txRect, DT_RIGHT | DT_VCENTER);
				if(dwHourMode == 0)	// 12H?
				{
					if(APMode == 0 && DispHour > 12)
						APMode = 2;
					if(APMode == 2 && DispHour > 24)
						APMode = 4;
					if((APMode == 1 && DispHour > 12) ||
					(APMode == 3 && DispHour > 24))
						APMode++;//	Set AM PM Char
					txRect.left = txRect.right + 1;
					txRect.right += 2 * dwBigFontWidth;
					DrawText(hDC, &APMark[APMode], 1, &txRect, DT_LEFT | DT_VCENTER);
				}
			}
//	Draw Title of the Reecord
			txRect.left = dwSmallFontWidth * 8 + 1;
			txRect.right = FrameWidth - 1;
			txRect.top = (i + 1) * LineHight + 1;
			txRect.bottom = txRect.top + LineHight;
			_tcscpy(TempStr, TEXT(""));
			if(DayAlarm[j])
				_tcscat(TempStr, TEXT("*"));
			if(DayToDo[j])
				_tcscat(TempStr, TEXT("!"));
			_tcscat(TempStr, DayTitle[0][j]);
			DrawText(hDC, TempStr, -1, &txRect, DT_LEFT | DT_VCENTER | DT_NOPREFIX);
			DayOrgIDBuf[i] = DayGenID[0][j];	// Clear Current Line Seek Ptr
			DayOrgAllDayBuf[i] = DayAllDay[0][j];	// Save Original AllDay Flag
			DayOrgTimeBuf[i] = DayStartTime[0][j];
			if(DayAllDay[0][j] && (APMode == 0 || APMode == 2 || APMode == 4))
				APMode++;
			i++;	// Progress Line
			if(i == DayLineCount)
				break;
		}
	}
	else
	{
		for(i = AllDayHight; i < DayLineCount; i++)	// Repeat for Day Lines (Horizontal Line)
		{
		DWORD	OneIndex = StartIndex + i + DayAllNum[0] - AllDayHight;
// Write Time
			if(OneIndex >= MAXDAYBUF)	// Limit Check
				goto ReWrite;	// Clear Screen & ReWrite
			DispHour = DayStartTime[0][OneIndex];
			DispMin = DispHour & 63;
			DispHour /= 64;
			if(DispHour == dwFinalTime + 1)
				break;
			if(dwHourMode)	// 24H Mode?
			{
				wsprintf(TimeStr, TEXT("%2d:%2.2d"), DispHour, DispMin);
			}
			else
			{
				if(DispHour > 24)
					wsprintf(TimeStr, TEXT("%2d:%2.2d"), DispHour - 24, DispMin);
				else if(DispHour > 12)
					wsprintf(TimeStr, TEXT("%2d:%2.2d"), DispHour - 12, DispMin);
				else
					wsprintf(TimeStr, TEXT("%2d:%2.2d"), DispHour, DispMin);
			}
			txRect.top = (i + 1) * LineHight + 1;
			txRect.bottom = txRect.top + LineHight;
			txRect.left = 1;
			txRect.right = txRect.left + 6 * dwSmallFontWidth;
			DrawText(hDC, TimeStr, -1, &txRect, DT_RIGHT | DT_VCENTER);
			if(dwHourMode == 0)	// 12H?
			{
				txRect.left = txRect.right + 1;
				txRect.right += 2 * dwBigFontWidth;
				DrawText(hDC, &APMark[APMode], 1, &txRect, DT_LEFT | DT_VCENTER);
			}
//	Draw Title of the Reecord
			txRect.left = dwSmallFontWidth * 8 + 1;
			txRect.right = FrameWidth - 1;
			txRect.top = (i + 1) * LineHight + 1;
			txRect.bottom = txRect.top + LineHight;
			if(DayGenID[0][OneIndex])
			{
				_tcscpy(TempStr, TEXT(""));
				if(DayAlarm[OneIndex])
					_tcscat(TempStr, TEXT("*"));
				if(DayToDo[OneIndex])
					_tcscat(TempStr, TEXT("!"));
				_tcscat(TempStr, DayTitle[0][OneIndex]);
				DrawText(hDC, TempStr, -1, &txRect, DT_LEFT | DT_VCENTER | DT_NOPREFIX);
			}
			DayOrgIDBuf[i] = DayGenID[0][OneIndex];	// Clear Current Line Seek Ptr
			DayOrgAllDayBuf[i] = DayAllDay[0][OneIndex];	// Save Original AllDay Flag
			DayOrgTimeBuf[i] = DayStartTime[0][OneIndex];
//	Progress Hour and Minutes
			if(APMode == 0 && DispHour == 12)
				APMode++;//	Set AM PM Char
			if(APMode == 0 || APMode == 2 || APMode == 4 ||
			(APMode == 1 && DispHour == 12) ||
			(APMode == 3 && DispHour == 24))
				APMode++;//	Set AM PM Char
		}
		if(DispHour == dwFinalTime + 1 || (dwDispStep && (DispHour == dwFinalTime && DispMin)))
		{
ReWrite:
			StartIndex--;
			PatBlt(hDC,		// Clear Only Schedule Part
				1,
				LineHight + 1,
				FrameWidth - 1,
				LineHight * DayLineCount, PATCOPY);
			goto DispAgain;
		}
	}
	if(SetCursor)
	{
		if(CurrentTime == MAXDWORD)	// Current is All Day ?
			dwDayLine = 0;	// Set Cursor Top
		else
		{
			if(dwAppDispMode)
			{
				for(i = 0; i < DayLineCount - 1; i++)
					if(DayOrgIDBuf[i] == 0)
						break;
					else if(DayOrgAllDayBuf[i] == 0)
						continue;
					else if(ConvHalfHour(DayOrgTimeBuf[i]) >= CurrentTime)
				break;	// Get Display Start Time Found
			}
			else
			{	// Show All Mode Scan Pos
				for(i = AllDayHight; i < DayLineCount - 1; i++)
					if(ConvHalfHour(DayOrgTimeBuf[i]) >= CurrentTime)
						break;	// Get Display Start Time Found
			}
			dwDayLine = i;
		}
	}
	dwOldLine = DayLineCount;
	DispHalfLineCursor(hDC);
}
//
//	Xor Cal Box Sub
//
void ReverseHalfCalBox(HDC hDC, DWORD x, DWORD y)
{
	if(y >= 30)
		y -= 30;
	else
		y += 6;
	PatBlt(hDC,
			(((y / 6) % 3) * 22 + x * 3 + 1) * dwSmallFontWidth + FrameWidth - 1,
			((y / 18) * 8 + (y % 6) + 2) * dwSmallFontHight,
			dwSmallFontWidth * 5 / 2 + 1,
			dwSmallFontHight, PATINVERT);
}
//
//	Reverse Small Calender Date
//
void DispHalfCalBox(HDC hDC)
{
	if(dwOldCurX != 7)
		ReverseHalfCalBox(hDC, dwOldCurX, dwOldCurY);
	ReverseHalfCalBox(hDC, dwCurX, dwCurY);
	dwOldCurX = dwCurX;
	dwOldCurY = dwCurY;
}

//
//	Xor Day Line Cursor
//
void ReverseHalfBox(HDC hDC, DWORD y)
{
	PatBlt(hDC,
			1,
			(y + 1) * LineHight + 1,
			FrameWidth - 1,
			dwSmallFontHight, PATINVERT);
}
//
//	Reverse Half Cursor
//
void DispHalfLineCursor(HDC hDC)
{
	if(dwOldLine != DayLineCount)
		ReverseHalfBox(hDC, dwOldLine);
	ReverseHalfBox(hDC, dwDayLine);
	dwOldLine = dwDayLine;
	SetupMenu(FALSE);
	if(dwAppDispMode)
	{
		if(DayOrgIDBuf[dwDayLine] == 0)	// End of Line?
			CurrentTime = ConvHalfHour(GetTime(31, 0));
		else
		{
			if(DayOrgAllDayBuf[dwDayLine] == 0)	// All Day Record?
				CurrentTime = MAXDWORD;
			else
				CurrentTime = ConvHalfHour(DayOrgTimeBuf[dwDayLine]);
		}
	}
	else
	{
		if(DayOrgIDBuf[dwDayLine] && DayOrgAllDayBuf[dwDayLine] == 0)// Points All Day Record?
			CurrentTime = MAXDWORD;
		else
			CurrentTime = ConvHalfHour(DayOrgTimeBuf[dwDayLine]);
	}
}
//
//	Get Index Year/Month (Buffer Center)
//
static DWORD GetBufferMonth(DWORD Index)
{
	return GetDate(MonthBuf[Index][15][0], MonthBuf[Index][15][1], 0);
}
//
//	Get Current Year/Month
//
static DWORD GetCursorMonth(DWORD Index)
{
	return GetDate(GetCurrentYear(Index), GetCurrentMonth(Index), 0);
}
//
//	Justify Cusor to Inside
//
static void JustifyCursor(void)
{
DWORD CurArray = dwCurY / 6;
	while(GetCursorMonth(CurArray) > GetBufferMonth(CurArray))
		dwCurY--;
	while(GetCursorMonth(CurArray) < GetBufferMonth(CurArray))
		dwCurY++;
}
//
//	Half Year Screen Key Down Proc
//
void HalfKeyDownProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
DWORD	LastYear, LastMonth, LastDay, CurArray;
DWORD	i, j;
	CurArray = dwCurY / 6;
	LastYear = GetCurrentYear(CurArray);
	LastMonth = GetCurrentMonth(CurArray);
	LastDay = GetCurrentDay(CurArray);
	if(wParam == VK_UP)
	{
		if(GetKeyState(VK_CONTROL) < 0)
		{
			if(dwYear == 1951 && dwMonth <= 2)
				return;
			for(i = 0; i < 3; i++)
			{
				dwMonth--;
				if(dwMonth == 0)
				{
					dwYear--;
					dwMonth = 12;
				}
				if(dwYear == 1951 && dwMonth <= 2)
					break;
			}
			SetupMonth(dwYear, dwMonth);
			JustifyCursor();
			goto PageChangeEnd;
		}
		else if(GetKeyState(VK_SHIFT) < 0)// Shift + Cursor UP
		{
			if(dwCurY % 6 == 0)
			{
UpColum:
				if(dwToDoHalf && (dwCurY / 6) == 1)
				{
					dwCurY = 11;
				}
				else
				{
					if(dwCurY <= 12)
						dwCurY += 23;
					else
						dwCurY -= 13;
				}
				CurArray = dwCurY / 6;
				JustifyCursor();
			}
			else
			{
				dwCurY--;
				if(dwCurY % 6 == 0)	// Check Also This Month
				{
					CurArray = dwCurY / 6;
					if(GetCurrentMonth(CurArray) != MonthBuf[CurArray][15][1])
						goto UpColum;
				}
			}
			goto CursorMoveEnd;
		}
		else	// Cursor UP
		{
			if(dwAppDispMode)
			{
				if(dwDayLine == 0)// Top Line?
				{
					if(HideSkip)
					{
						HideSkip--;
						goto ScrollEnd;
					}
				}
				if(dwDayLine)
					dwDayLine--;
				goto ShiftEnd;
			}
			else
			{
				if(dwDayLine == AllDayHight)// Not Hide Mode?
				{
					if(StartIndex)
					{
						StartIndex--;
						goto ScrollEnd;
					}
				}
				if(dwDayLine)
					dwDayLine--;
				else if(AllStartIndex)
				{
					AllStartIndex--;
					goto ScrollEnd;
				}
				goto ShiftEnd;
			}
		}
	}
	else if(wParam == VK_DOWN)
	{
		if(GetKeyState(VK_CONTROL) < 0)
		{
			if(dwYear == 2077 && dwMonth >= 8)
				return;
			for(i = 0; i < 3; i++)
			{
				dwMonth++;
				if(dwMonth == 13)
				{
					dwYear++;
					dwMonth = 1;
				}
				if(dwYear == 2077 && dwMonth >= 8)
					break;
			}
			SetupMonth(dwYear, dwMonth);
			JustifyCursor();
PageChangeEnd:
			HalfScreen(hDC, TRUE, FALSE, TRUE);// No Scroll & No SetCursor 
			DispHalfCalBox(hDC);
			InvalidateRect(hWnd, NULL, FALSE);
			return;
		}
		else if(GetKeyState(VK_SHIFT) < 0)// Shift + Cursor DOWN
		{
			if(dwCurY % 6 == 5)
			{
DownColum:
				dwCurY = (dwCurY / 6) * 6;
				if(dwToDoHalf && (dwCurY / 6) == 1)
				{
					dwCurY = 6;
				}
				else
				{
					if(dwCurY <= 12)
						dwCurY += 18;
					else
						dwCurY -= 18;
				}
				CurArray = dwCurY / 6;
				JustifyCursor();
			}
			else
			{
				dwCurY++;
				CurArray = dwCurY / 6;
				if(GetCurrentMonth(CurArray) != MonthBuf[CurArray][15][1])
					goto DownColum;
			}
			goto CursorMoveEnd;
		}
		else	// Cursor DOWN
		{
			if(dwAppDispMode)
			{
				if(dwDayLine == DayLineCount - 1)	// Bottom Line?
				{	// Scroll Down
					if(DayLineCount + HideSkip < ActualNum[0] + 1)
					{
						HideSkip++;
						goto ScrollEnd;
					}
				}	// Just Cursor Down
				if(dwDayLine < DayLineCount - 1 && dwDayLine < ActualNum[0] - HideSkip)
					dwDayLine++;
				goto ShiftEnd;
			}
			else
			{
//	For All Day Scroll
				if((DayAllNum[0] > dwTopDayLine)
				&& (dwDayLine == dwTopDayLine - 1)
				&& (AllStartIndex < DayAllNum[0] - dwTopDayLine))
				{
					AllStartIndex++;
					goto ScrollEnd;
				}
//	End of All Day Scroll
				if(dwDayLine == DayLineCount - 1)
				{
					if(DispHour < dwFinalTime)
					{
						StartIndex++;
ScrollEnd:
						HalfScreen(hDC, FALSE, TRUE, FALSE);// Not Draw Calender & Not Set Cursor Pos
						InvalidateRect(hWnd, NULL, FALSE);
						return;
					}
				}
				if(dwDayLine < DayLineCount - 1)
					dwDayLine++;
				goto ShiftEnd;
			}
		}
	}
	else if(wParam == VK_LEFT)	// Cursor LEFT
	{
		if(GetKeyState(VK_SHIFT) < 0)
		{
			if(dwCurX == 0)
			{
				dwCurX = 6;
				switch(dwCurY / 6)
				{
				case 0:
					dwCurY += 30;
					break;
				case 1:
				case 3:
				case 4:
					dwCurY -= 6;
					break;
				case 2:
					if(dwToDoHalf)
						dwCurY += 6; 
					else
						dwCurY += 12;
					break;
				case 5:
					dwCurY -= 24;
				}
			}
			else
				dwCurX--;
			JustifyCursor();
		}
		else
		{
			if(dwCurX == 0)
			{
				dwCurX = 6;
				if((dwCurY % 6) == 0)	// Just Left Top?
					goto SeekPrevMonth;
				else
					dwCurY--;
			}
			else
				dwCurX--;
			if(GetCurrentMonth(dwCurY / 6) != LastMonth)	// Change Month?
			{
SeekPrevMonth:
				dwCurY = (dwCurY / 6) * 6;
				if(dwCurY == 0)	// if Current Month then Back to Array 5
					j = 5;
				else
					j = dwCurY / 6 - 1;
				if(dwToDoHalf && j == 4)
					j = 3;
				for(i = 41; TRUE; i--)	// Search Last Day of Array 5
					if(MonthBuf[j][i][1] == MonthBuf[j][15][1])
						break;
				dwCurY = i / 7 + j * 6;
				dwCurX = i % 7;
			}
		}
		goto CursorMoveEnd;
	}
	else if(wParam == VK_RIGHT)	// Cursor RIGHT
	{
		if(GetKeyState(VK_SHIFT) < 0)
		{
			if(dwCurX == 6)
			{
				dwCurX = 0;
				switch(dwCurY / 6)
				{
				case 3:
					if(dwToDoHalf)
					{
						dwCurY -= 6;	// Skip ToDo Part
						break;
					}
				case 0:
				case 2:
					dwCurY += 6;
					break;
				case 1:
					dwCurY += 24;
					break;
				case 4:
					dwCurY -= 12;
					break;
				case 5:
					dwCurY -= 30;
				}
			}
			else
				dwCurX++;
			JustifyCursor();
		}
		else
		{
			if(dwCurX == 6)
			{
				dwCurX = 0;
				if(dwCurY % 6 == 5)
					goto SeekNextMonth;
				else
					dwCurY++;
			}
			else
				dwCurX++;
			CurArray = dwCurY / 6;
			if(GetCurrentMonth(CurArray) != LastMonth)	// Change Month?
			{
SeekNextMonth:
				dwCurY = (dwCurY / 6) * 6 + 6;
				if(dwToDoHalf && dwCurY == 24)
					dwCurY = 30;	// Skip ToDo Part
				if(dwCurY == 36)
					dwCurY = 0;
				CurArray = dwCurY / 6;
				for(dwCurX = 0; dwCurX < 7; dwCurX++)// Scan First Day
					if(MonthBuf[CurArray][dwCurX][1] == MonthBuf[CurArray][15][1])
						break;// Found Same Month
			}
		}
CursorMoveEnd:
		HalfScreen(hDC, FALSE, FALSE, TRUE);// No setup & No Scroll & No SetCursor 
		DispHalfCalBox(hDC);
		InvalidateRect(hWnd, NULL, FALSE);
	}
	else if(wParam == VK_PRIOR && dwAppDispMode == 0)
	{
		if(StartIndex == 0)
			goto Home;
		if(StartIndex > 8)
			StartIndex -= 8;
		else
			StartIndex = 0;
		goto ScrollEnd;
	}
	else if(wParam == VK_NEXT && dwAppDispMode == 0)
	{
		for(i = 0; i < 8; i++)
			if(StartIndex + DayLineCount - 1 < MAXDAYBUF
			&& DayStartTime[0][StartIndex + DayLineCount - 1] < GetTime(dwFinalTime, 0))
				StartIndex++;
			else
				break;
		goto ScrollEnd;
	}
	else if(wParam == VK_HOME)
	{
Home:
		dwDayLine = 0;
		goto ShiftEnd;
	}
	else if(wParam == VK_END && dwAppDispMode == 0)
	{
		dwDayLine = DayLineCount - 1;
ShiftEnd:
		DispHalfLineCursor(hDC);
		InvalidateRect(hWnd, NULL, FALSE);
		return;
	}
}
//
//	Half Year Mouse Left Button Proc
//
void HalfLeftProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
DWORD	Top = dwWinTop + 2;
DWORD	CurX, CurY, Page;
	if(LOWORD(lParam) >= FrameWidth + dwSmallFontWidth * 67)
		return;
	if(LOWORD(lParam) >= FrameWidth + dwSmallFontWidth)
	{	// Cal Part Click
		CurX = (LOWORD(lParam) - FrameWidth - dwSmallFontWidth) / dwSmallFontWidth;
		if(CurX > 42)
			CurX--;
		if(CurX > 21)
			CurX--;
		CurX /= 3;
		CurY = (HIWORD(lParam) - dwWinTop) / dwSmallFontHight;
		if(CurY > 16)
		{	// Blank Area
			SendMessage(hWnd, WM_COMMAND, IDM_JUMP_DATE, 0);
			return;
		}
		if(CurY < 8)
		{	// Upper Part
			if(CurX < 7)
				Page = 5;
			else if(CurX < 14)
				Page = 0;
			else
				Page = 1;
		}
		else
		{	// Lower Part
			if(CurX < 7)
				Page = 2;
			else if(CurX < 14)
				Page = 3;
			else
				Page = 4;
		}
		if(dwToDoHalf && Page == 4)
			return;
		if(CurY < 2 || CurY == 8 || CurY == 9)
		{	// Click Month Name Part
			return;
		}
		CurX = CurX % 7;
		if(CurY > 8)
			CurY -= 8;
		CurY = CurY - 2 + Page * 6;
		if(MonthBuf[Page][(CurY % 6) * 7 + CurX][1] != MonthBuf[Page][15][1])
			return;
		dwCurX = CurX;
		dwCurY = CurY;
		HalfScreen(hDC, FALSE, FALSE, FALSE);
		DispHalfCalBox(hDC);
		InvalidateRect(hWnd, NULL, FALSE);
		return;	
	}
	else if(LOWORD(lParam) < FrameWidth && HIWORD(lParam) >= Top + LineHight)
	{
		dwDayLine = (HIWORD(lParam) - Top - LineHight) / LineHight;
		if(dwDayLine > DayLineCount - 1)
			dwDayLine = DayLineCount - 1;
		if(dwAppDispMode && dwDayLine > ActualNum[0] - HideSkip)
			dwDayLine = ActualNum[0] - HideSkip;
		DispHalfLineCursor(hDC);
		InvalidateRect(hWnd, NULL, FALSE);
	}
}
//
//	Half Year Double Click Proc
//
void HalfDblProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
DWORD	Top = dwWinTop + 2;
DWORD	Page, CurX, CurY;
	if(LOWORD(lParam) >= FrameWidth + dwSmallFontWidth * 67)
		return;
	if(LOWORD(lParam) >= FrameWidth + dwSmallFontWidth)
	{	// Cal Part Click
		CurX = (LOWORD(lParam) - FrameWidth - dwSmallFontWidth) / dwSmallFontWidth;
		if(CurX > 42)
			CurX--;
		if(CurX > 21)
			CurX--;
		CurX /= 3;
		CurY = (HIWORD(lParam) - dwWinTop) / dwSmallFontHight;
		if(CurY > 16)
		{	// Blank Area
			SendMessage(hWnd, WM_COMMAND, IDM_JUMP_DATE, 0);
			return;
		}
		if(CurY < 8)
		{	// Upper Part
			if(CurX < 7)
				Page = 5;
			else if(CurX < 14)
				Page = 0;
			else
				Page = 1;
		}
		else
		{	// Lower Part
			if(CurX < 7)
				Page = 2;
			else if(CurX < 14)
				Page = 3;
			else
				Page = 4;
		}
		if(CurY < 2 || CurY == 8 || CurY == 9)
		{	// Click Month Name Part
			return;
		}
		if(dwToDoHalf && Page == 4)
		{
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_TODO, 0);
			return;
		}
		CurX = CurX % 7;
		if(CurY > 8)
			CurY -= 8;
		CurY = CurY - 2 + Page * 6;
		if(MonthBuf[Page][(CurY % 6) * 7 + CurX][1] != MonthBuf[Page][15][1])
			return;
		dwCurX = CurX;
		dwCurY = CurY;
		SendMessage(hWnd, WM_COMMAND, IDM_VIEW_MONTH, 0);
		return;
	}
	else if(LOWORD(lParam) < FrameWidth && HIWORD(lParam) >= Top + LineHight)
	{
		dwDayLine = (HIWORD(lParam) - Top - LineHight) / LineHight;
		if(dwDayLine > DayLineCount - 1)
			dwDayLine = DayLineCount - 1;
		if(dwAppDispMode && dwDayLine >  ActualNum[0] - HideSkip)
			dwDayLine = ActualNum[0] - HideSkip;
		SendMessage(hWnd, WM_CHAR, TEXT('\r'), NULL);
	}
}
