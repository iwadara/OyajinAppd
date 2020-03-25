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
void	DispReverseLine(HDC);
void	DispCalenderBox(HDC);
void	ReverseCalBox(HDC, DWORD, DWORD);
void	DispLineCursor(HDC);
void	ReverseLineBox(HDC, DWORD);
void	DrawFrame(HDC, DWORD, DWORD, DWORD, DWORD);
void	DrawToDo(HDC, DWORD, DWORD, DWORD, DWORD);
void	DayScreen(HDC, BOOL, BOOL, BOOL);
void	DayKeyProc(HWND, HDC, WPARAM, LPARAM);
void	SetupMainScreen(HDC);
void	SetupMenu(BOOL);
void	GetToday(void);
void	GetUniCode(char *, LPTSTR);
void	SetupBrush(HDC);
void	SetupCurrentPos(void);
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
BOOL	DelRecord(BOOL);
BOOL	AddRecord(BOOL, BOOL);
BOOL	ConvTimeToDword(LPDWORD, LPTSTR);
BOOL	ConvDateToDword(LPDWORD, LPTSTR);
void	AdvHalfHour(LPDWORD);
void	SetupOneDayArray(DWORD, DWORD, BOOL, DWORD);
DWORD	ConvHalfHour(DWORD);
BOOL	SetupClipData(BOOL);
BOOL	RestoreClipData(HWND, BOOL);
void	SetupToDoArray(void);
//
//	Global Vars
//
static DWORD	LineHight, LineWidth;	// Day Line Cursor Size
static DWORD	FrameWidth;				// Day Screen Frame Width
static DWORD	dwOldLine;				// Old Line Cursor Pos
static DWORD	LineLeft;				// Line Cursor Left Pos
static DWORD	BarLeft;
static DWORD	BarHight, BarUnit;	// Bar Hight & Bar Unit
//
//	Setup Day Display Screen to Memory Device
//
void DayScreen(HDC hDC, BOOL DrawCal, BOOL Scroll, BOOL SetCursor)
{
POINT	LinePoint[4];	// Point Structure for Line
RECT	txRect;		// Rect for DrawText
HPEN	hPen;		// Pen Handle
HBRUSH	hBrush;		// Brush Handle
TCHAR	TimeStr[8];	//	Time String Buffer
DWORD	DayTotal, Year, Month;	// Work for Day Count and Calender
//LPCTSTR	ShortDayStr[] = {TEXT("Su"), TEXT("Mo"), TEXT("Tu"), TEXT("We"),TEXT("Th"), TEXT("Fr"), TEXT("Sa")};
LPCTSTR	ShortDayStr[] = {TEXT("ì˙"), TEXT("åé"), TEXT("âŒ"), TEXT("êÖ"),TEXT("ñÿ"), TEXT("ã‡"), TEXT("ìy")}; // by uminchu
DWORD	DispMin = 0;	// Start Time Work
DWORD	APMode;		// Am Pm Dispay Mode
DWORD	Today;
DWORD	i, j, k;	// Temp Counter
TCHAR	TempStr[256];// Title Buffer
//
//	FrameWidth = dwXsize * 7 / 10;		// Get LineBox Width
	FrameWidth = dwXsize - dwSmallFontWidth * 29;		// Get LineBox Width
//	Setup Dimensions
	SetupBrush(hDC);
	if(DrawCal)
		PatBlt(hDC, 0, 0, dwXsize, dwYsize, PATCOPY);	// Clear Memory Device
	else if(Scroll)
	{
		PatBlt(hDC,	0, 0, FrameWidth, dwSmallFontHight + 2, PATCOPY);
		PatBlt(hDC,		// Clear Only Schedule Part (But Bar)
			dwSmallFontWidth * 3 + FrameWidth / 32 + 2, 
			dwSmallFontHight + 2,
			FrameWidth - dwSmallFontWidth * 3 - FrameWidth / 32 - 1,
			dwYsize - 1, PATCOPY);
	}
	else		// Clear Only Schedule Part
		PatBlt(hDC,	1, 1, FrameWidth - 1, dwYsize - 1, PATCOPY);
//	Calc Cursor Size
	GetToday();
	DayLineCount = (dwYsize - dwSmallFontHight - 6) / dwBigFontHight;// Get Lines
	if(DayLineCount > dwFinalTime * (dwDispStep + 1))	// 00:00 to 31:00 ?
		DayLineCount = dwFinalTime * (dwDispStep + 1) + 1;	// Force Set 00:00 to 31:00
	if(DayLineCount >= MAXDAYLINE)
		DayLineCount = MAXDAYLINE;
	LineHight = dwBigFontHight;// Get LineBox Height
	LineWidth = FrameWidth * 3 / 4;		// Get LineBox Width
//	Draw Today's Date in English
//	Draw Day Frame
	DrawFrame(hDC, 0, 0, FrameWidth, 4 + dwSmallFontHight + LineHight * DayLineCount);
	LinePoint[0].x = 0;				// Frame Left Top Axis
	LinePoint[1].x = FrameWidth;	// Frame Left Top Axis
	LinePoint[0].y = LinePoint[1].y = dwSmallFontHight + 1;
	Polyline(hDC, LinePoint, 2);	// Draw Todays Line
	SelectObject(hDC, hSmallFont);		// Set Font (Small)
	// [iwad] txRect.top = 1;
	txRect.top = 2;
	txRect.bottom = dwSmallFontHight ;
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
//	Draw Week Number
	DayTotal = GetDayTotal();
	Year = GetCurrentYear(0);
//	wsprintf(TempStr, TEXT("Week %d"),(DayTotal + (((Year + 3) / 4 + Year + 5) + 6) % 7 - 1) / 7 + 1);
	wsprintf(TempStr, TEXT("%dèT"),(DayTotal + (((Year + 3) / 4 + Year + 5) + 6) % 7 - 1) / 7 + 1);	// by uminchu
	txRect.left = FrameWidth / 20;
	txRect.right = txRect.left + dwSmallFontWidth * 9;
	DrawText(hDC, TempStr, -1, &txRect, DT_LEFT | DT_VCENTER);
//	Draw Past/Left Days
	wsprintf(TempStr, TEXT("%d/%d"),
		DayTotal, ((Year % 4)? 365:366) - DayTotal);
	txRect.right = FrameWidth - 2;
	txRect.left = txRect.right - dwSmallFontWidth * 9;
	DrawText(hDC, TempStr, -1, &txRect, DT_RIGHT | DT_VCENTER);
	if(dwPrivate)
		PatBlt(hDC, 1, 1, FrameWidth - 1, dwSmallFontHight + 1, PATINVERT);
	if(DrawCal)
	{
//	Draw Calender TEXT
		for(j = 0; j < 2; j++)
		{
			if(j == 1 && dwToDoDay)		// ToDo on Day Screen?
				break;					// Draw Only 1 Calender
			Year = MonthBuf[j][15][0];	// Get Current Year (Center)
			Month = MonthBuf[j][15][1];	//	Get Current Month (Center)
			TempStr[2] = TEXT('\0');	// Set Null Char for Calender Date
			for(i = 0;i < 42; i++)
			{
				if(MonthBuf[j][i][1] != Month)
					continue;
				txRect.left =
					((i % 7) * 4 + 1) * dwSmallFontWidth + FrameWidth;
				txRect.top = (j * 8 + i / 7 + 2) * dwSmallFontHight;
				txRect.bottom = txRect.top + dwSmallFontHight;
				txRect.right = txRect.left + dwSmallFontWidth * 3;
				if(MonthBuf[j][i][2] < 10)
					TempStr[0] = TEXT(' ');
				else
					TempStr[0] = (TCHAR )MonthBuf[j][i][2] / 10 + TEXT('0');
				TempStr[1] = (TCHAR )MonthBuf[j][i][2] % 10 + TEXT('0');
				
			// [iwad] ìyì˙ÉJÉâÅ[ëŒâû
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

				dwDlgDate = GetDate(MonthBuf[j][i][0], MonthBuf[j][i][1], MonthBuf[j][i][2]);
				dwDlgStartTime = 0;
				dwAllDay = 0;
				dwGenID = 0;
				if(dwUnderBar && dwPrivate == 0 && OyajiSearchRecord(&MainWork))
				{	// Search Main File & Key is dwDlg Works
					LinePoint[0].x = 	// Frame Left Top Axis
						((i % 7) * 4 + 1) * dwSmallFontWidth + FrameWidth;
					LinePoint[1].x = 
						LinePoint[0].x + dwSmallFontWidth * 3 
						- dwSmallFontWidth / 2;	// Frame Left Top Axis
					LinePoint[0].y = LinePoint[1].y = 
						(j * 8 + i / 7 + 3) * dwSmallFontHight - 1;
					Polyline(hDC, LinePoint, 2);	// Draw Todays Line
				}
				if(MonthBuf[j][i][0] == dwTodayYear
				&& MonthBuf[j][i][1] == dwTodayMonth
				&& MonthBuf[j][i][2] == dwTodayDay)
				DrawFrame(hDC,
					((i % 7) * 4 + 1) * dwSmallFontWidth + FrameWidth - 1,
					(j * 8 + i / 7 + 2) * dwSmallFontHight,
					dwSmallFontWidth * 3 - 2,
					dwSmallFontHight - 1);
			}
//	Draw Day Indicator
			txRect.top = (j * 8 + 1) * dwSmallFontHight;
			txRect.bottom = txRect.top + dwSmallFontHight;

		// [iwad] ìyì˙ÉJÉâÅ[ëŒâû
			for(i = 0;i < 7; i++)
			{
				txRect.left =
					(i % 7) * dwSmallFontWidth * 4 + FrameWidth + dwSmallFontWidth;
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
					(i % 7) * dwSmallFontWidth * 4 + FrameWidth + dwSmallFontWidth;
				txRect.right = txRect.left + dwSmallFontWidth * 3;
				DrawText(hDC, ShortDayStr[i], -1, &txRect, DT_VCENTER);
			}
		#endif
//	Draw Month and Year
			txRect.top = j * dwSmallFontHight * 8;
			txRect.bottom = txRect.top + dwSmallFontHight;
			txRect.left = FrameWidth + dwSmallFontWidth;
			txRect.right = txRect.left + 26 * dwSmallFontWidth;
			if(dwDateFormat)
				wsprintf(TempStr, TEXT("%s %d"), MonthName[Month - 1], Year);
			else
				wsprintf(TempStr, TEXT("%d / %d"), Year, Month);
			DrawText(hDC, TempStr, -1, &txRect, DT_CENTER | DT_VCENTER);
//	Draw Frame
			DrawFrame(hDC,
				FrameWidth + dwSmallFontWidth / 2,	// Frame Left Top Axis
				j * dwSmallFontHight * 8,
				28 * dwSmallFontWidth - dwSmallFontWidth / 2,
				dwSmallFontHight * 8);
		}
		if(dwToDoDay)	// ToDo on Day Screen?
			DrawToDo(hDC,
				FrameWidth + dwSmallFontWidth / 2,	// Frame Left Top Axis
				j * dwSmallFontHight * 8,
				28 * dwSmallFontWidth - dwSmallFontWidth / 2,
				dwSmallFontHight * 8 + 1);
		dwOldCurX = 7;
		DispCalenderBox(hDC);
	}
	if(Scroll)	// Scroll Mode?
		goto DispAgain;	// Skip Array Setup
	Today = GetCurrentDate();	// Set Search Day
//
	AllDayHight = 0;// All Day Scedule Line Number
	SetupOneDayArray(Today, 0, TRUE, 2);	// Setup One Day Array Index = 0 DayMode = 2 (Day)
//	Disp Diagram Bar
	if(dwGraphMode)
	{
		BarHight = DayLineCount * LineHight;
		BarUnit = BarHight / (dwFinalTime - dwStartTime + 1) / 2;
		BarHight = BarUnit * (dwFinalTime - dwStartTime) * 2 + 1;
		if(BarUnit)	// Legal Scale Unit?
			DrawFrame(hDC,	dwSmallFontWidth * 3 + 1,
						dwSmallFontHight * 3 / 2,
						FrameWidth / 32, 
						BarHight);
		for(i = dwStartTime; i < dwFinalTime; i++)
		{
		TCHAR	NumBuf[16];
			if(BarUnit * 4 < dwSmallFontHight)
			{
				if(i % 3)
					continue;
			}
			else if(BarUnit * 2 < dwSmallFontHight)
			{
				if(i % 2)
					continue;
			}
			txRect.left = 2;
			txRect.right = dwSmallFontWidth * 3 - 2;
			txRect.top = (i - dwStartTime) * BarUnit * 2 + dwSmallFontHight + 2,
			txRect.bottom = txRect.top + dwSmallFontHight;
			wsprintf(NumBuf, TEXT("%d"), (dwHourMode) ? i : ((i == 12 || i == 24)? 12: i % 12));
			DrawText(hDC, NumBuf, -1, &txRect, DT_RIGHT | DT_VCENTER);
		}
		hPen = (HPEN )GetStockObject(NULL_PEN);	// Get Black Pen
		SelectObject(hDC, hPen);			// Set Brush
		for(i = dwStartTime * 2; i < dwFinalTime * 2; i++)
		{
		DWORD	BrushCode;
			switch(BarBuf[i])
			{
			case 0:	// Blank?
				if(dwBackColor == 0)
					continue;
				BrushCode = WHITE_BRUSH;
				break;
			case 1:
				BrushCode = LTGRAY_BRUSH;
				break;
			case 2:
				BrushCode = GRAY_BRUSH;
				break;
			case 3:
				BrushCode = DKGRAY_BRUSH;
				break;
			default:
				BrushCode = BLACK_BRUSH;
			}
			LinePoint[0].x = LinePoint[3].x = dwSmallFontWidth * 3 + 2;	// Bar Left Top Axis
			LinePoint[1].x = LinePoint[0].x + FrameWidth / 32 - 1;	// Frame Left Top Axis
			LinePoint[0].y = LinePoint[1].y = (i - dwStartTime * 2) * BarUnit + dwSmallFontHight * 3 / 2 + 1;
			LinePoint[2].x = LinePoint[1].x;
			LinePoint[2].y = LinePoint[3].y = LinePoint[0].y + BarUnit;
			hBrush = (HBRUSH )GetStockObject(BrushCode);	// Get Black Pen
			SelectObject(hDC, hBrush);			// Set Brush
			Polygon(hDC, LinePoint, 4);	// Draw Todays Line
		}
		SetupBrush(hDC);
	}
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
	SelectObject(hDC, hBigFont);		// Set Font (Small)
	StartTime = ConvHalfHour(DayStartTime[0][DayAllNum[0] + StartIndex]);
	LineLeft = FrameWidth / 7 + 8 * dwBigFontWidth;	// Left Pos of Line Cursor
	if(DayAllNum[0] > dwTopDayLine)
		AllDayHight = dwTopDayLine;	// Maximum AllDay Disp Line is 3
	else
		AllDayHight = DayAllNum[0];// All Day Scedule Line Number
	if(DayAllNum[0] && dwAppDispMode == 0)	// All Day Scedule Exists & All Display Mode?
	{
		for(i = 0; i < AllDayHight; i++)
		{
//	Draw Title of the Record
			txRect.left = LineLeft;
			txRect.right = FrameWidth - 1;
			txRect.top = i * LineHight + dwSmallFontHight + 2,
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
				txRect.top = dwSmallFontHight + 2 + i * LineHight;
				txRect.bottom = txRect.top + LineHight;
				txRect.left = LineLeft - 8 * dwBigFontWidth;
				txRect.right = LineLeft - 2 * dwBigFontWidth;
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
//	Draw Title of the Record
			txRect.left = LineLeft;
			txRect.right = FrameWidth - 1;
			txRect.top = i * LineHight + dwSmallFontHight + 2;
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
// Write Time
		DWORD	OneIndex = StartIndex + i + DayAllNum[0] - AllDayHight;
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
			txRect.top = dwSmallFontHight + 2 + i * LineHight;
			txRect.bottom = txRect.top + LineHight;
			txRect.left = LineLeft - 8 * dwBigFontWidth;
			txRect.right = LineLeft - 2 * dwBigFontWidth;
			DrawText(hDC, TimeStr, -1, &txRect, DT_RIGHT | DT_VCENTER);
			if(dwHourMode == 0)	// 12H?
			{
				txRect.left = txRect.right + 1;
				txRect.right += 2 * dwBigFontWidth;
				DrawText(hDC, &APMark[APMode], 1, &txRect, DT_LEFT | DT_VCENTER);
			}
//	Draw Title of the Record
			txRect.left = LineLeft;
			txRect.right = FrameWidth - 1;
			txRect.top = i * LineHight + dwSmallFontHight + 2;
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
//	Draw Blacket
			if(DayBlacket[OneIndex])
			{
				for(j = 0; j < 4; j++)
				{
					BarLeft = LineLeft - 8 * dwBigFontWidth - j * FrameWidth / 64;// Blacket Right Pos
					switch((DayBlacket[0][OneIndex] >> (j * 3)) & 7)
					{
					case 0:	// Blank
						continue;	// No Line
					case 1:	// -
						LinePoint[0].x = BarLeft;// Blacket Left
						LinePoint[1].x = LinePoint[0].x - FrameWidth / 128;	// Frame Left Top Axis
						LinePoint[0].y = LinePoint[1].y =
							i * LineHight + LineHight / 2 + dwSmallFontHight + 2;
						Polyline(hDC, LinePoint, 2);	// Draw Todays Line
						break;
					case 2:	// |
						LinePoint[0].x = BarLeft - FrameWidth / 128;// Blacket Left
						LinePoint[1].x = LinePoint[0].x;	// Frame Left Top Axis
						LinePoint[0].y = i * LineHight + dwSmallFontHight + 2;
						LinePoint[1].y = LinePoint[0].y + LineHight;
						Polyline(hDC, LinePoint, 2);	// Draw Todays Line
						break;
					case 3:	// Start Blacket
						LinePoint[0].x = BarLeft;// Blacket Left
						LinePoint[1].x = LinePoint[0].x - FrameWidth / 128;	// Frame Left Top Axis
						LinePoint[0].y = LinePoint[1].y =
							i * LineHight + LineHight / 2 + dwSmallFontHight + 2;
						LinePoint[2].x = LinePoint[1].x;
						LinePoint[2].y = (i + 1) * LineHight + dwSmallFontHight + 2;
						Polyline(hDC, LinePoint, 3);	// Draw Todays Line
						break;
					case 4:	// Start Blacket
						LinePoint[0].x = BarLeft;// Blacket Left
						LinePoint[1].x = LinePoint[0].x - FrameWidth / 128;	// Frame Left Top Axis
						LinePoint[0].y = LinePoint[1].y =
							i * LineHight + LineHight / 2 + dwSmallFontHight + 2;
						LinePoint[2].x = LinePoint[1].x;
						LinePoint[2].y = i * LineHight + dwSmallFontHight + 1;
						Polyline(hDC, LinePoint, 3);	// Draw Todays Line
						break;
					}
				}
			}
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
				dwSmallFontWidth * 3 + FrameWidth / 32 + 2,
				dwSmallFontHight + 2,
				FrameWidth - dwSmallFontWidth * 3 - FrameWidth / 32 - 2,
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
			{	// Hide Blank Mode Scan Pos
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
	DispLineCursor(hDC);
}
//
//	Reverse Small Calender Date
//
void DispCalenderBox(HDC hDC)
{
	if(dwOldCurX != 7)
		ReverseCalBox(hDC, dwOldCurX, dwOldCurY);
	ReverseCalBox(hDC, dwCurX, dwCurY);
	dwOldCurX = dwCurX;
	dwOldCurY = dwCurY;
}

void ReverseCalBox(HDC hDC, DWORD x, DWORD y)
{
	PatBlt(hDC,
			x * dwSmallFontWidth * 4 + FrameWidth + dwSmallFontWidth - 1,
			(y + 2) * dwSmallFontHight,
			dwSmallFontWidth * 3 - 1,
			dwSmallFontHight, PATINVERT);
}
//
//	Reverse Day Cursor
//
void DispLineCursor(HDC hDC)
{
	if(dwOldLine != DayLineCount)
		ReverseLineBox(hDC, dwOldLine);
	ReverseLineBox(hDC, dwDayLine);
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
//	Xor Day Line Cursor
//
void ReverseLineBox(HDC hDC, DWORD y)
{
	PatBlt(hDC,
			LineLeft,
			y * LineHight + dwSmallFontHight + 2,
			FrameWidth - LineLeft,
			dwBigFontHight, PATINVERT);
}
//
//	Advance Time (1H or 30 Min)
//
void	AdvHalfHour(LPDWORD timeptr)
{
	if(dwDispStep)
		*timeptr = *timeptr + (((*timeptr & 63) == 30)? 34: 30);
	else
		*timeptr += 64;
}
//
//	Convert Time to Half Hour Format
//
DWORD ConvHalfHour(DWORD time)
{
	if(dwDispStep)
		return (time / 64 * 2) + ((time & 63)/ 30);	// 30 Min Mode
	else
		return (time / 64 * 2);	// 30 Min Mode
}
//
//	Setup OneDay Array
//
void SetupOneDayArray(DWORD Date, DWORD Index, BOOL Place, DWORD Week)
{
DWORD	LastHour;	// Last Set Hour
DWORD	AllMode;	// All Day Flag Loop Work
DWORD	i, j, k;
TCHAR	TempStr[256];
DWORD	DispTime;	// Cureent Buffer Set Time
DWORD	FillLen, FillPos;	// Bar Fill Length & Pos
DWORD	EndTime;	// End Time Save
	DayAllNum[Index] = 0;
	DayOneNum[Index] = 0;
	ActualNum[Index] = 0;
//	Setup All Day & One Shot Scedule to Buffer
	i = 0;
	for(AllMode = 0; AllMode < 2; AllMode++)	// Repeat All Day & One Shot
	{
		DispTime = 0;	// Begin From 0:00
		LastHour = MAXDWORD;
		dwDlgDate = Date;
		dwDlgStartTime = 0;
		dwGenID = 0;
		dwAllDay = AllMode;
		if(OyajiSearchRecord(&MainWork) > 1)	// Scedule Exists?
		{
			while(TRUE)// Forever
			{
				dwAllDay = AllMode;	// All Day Schedule
				if(OyajiGetRecord(&MainWork) == FALSE)
					break;	// ReadError or File End
				if(AllMode != dwAllDay || dwDlgDate != Date)// All Day Scedule Record Over?
					break;
				if(dwGenID == HideGenID)
					continue;	// Hide Current Add/Delete Record
				if(Week == 1 && dwDlgWeekView == 0)
					continue;
				if(Week == 2 && dwDlgDayView == 0)
					continue;
				if(dwDlgPrivate != dwPrivate)
					continue;
				if(i >= MAXDAYBUF)	// Check Max Display
					break;
				if(dwAllDay)	// One Shot Mode?
				{
					while(ConvHalfHour(DispTime) < ConvHalfHour(dwDlgStartTime))
					{	// Clear Display Buffer
						if(i >= MAXDAYBUF)	// Check Max Display
							break;
						DayGenID[Index][i] = 0;	// Clear Current Line Seek Ptr
						DayAllDay[Index][i] = 1;	// Save Original AllDay Flag
						DayStartTime[Index][i] = DispTime;// Save (Hidden) Start Time
						AdvHalfHour(&DispTime);	// Advance 1 Hour or 30 Min
						i++;
						DayOneNum[Index]++;
					}
					if( i < MAXDAYBUF	// Limit Check
						&& LastHour != ConvHalfHour(dwDlgStartTime)
						&& (dwDispStep? ((dwDlgStartTime & 63) != 30 && (dwDlgStartTime & 63))
										:(dwDlgStartTime & 63)))// One Shot & Not Begin :00
					{	// Clear Display Buffer
						DayGenID[Index][i] = 0;	// Clear Current Line Seek Ptr
						DayAllDay[Index][i] = 1;	// Save Original AllDay Flag
						DayStartTime[Index][i] = DispTime;// Save (Hidden) Start Time
						i++;
						DayOneNum[Index]++;	// If All Display Mode then Count up
					}
					if(LastHour != ConvHalfHour(dwDlgStartTime))
						AdvHalfHour(&DispTime);
				}
				LastHour = ConvHalfHour(dwDlgStartTime);
				DayGenID[Index][i] = dwGenID;	// Clear Current Line Seek Ptr
				DayAllDay[Index][i] = dwAllDay;	// Save Original AllDay Flag
				DayStartTime[Index][i] = dwDlgStartTime;// Save (Hidden) Start Time
				DayEndTime[Index][i] = dwDlgEndTime;// Save (Hidden) Start Time
				DayAlarm[i] = dwDlgEnable;
				DayToDo[i] = dwDlgToDo;
//	Setup Title of the Record
				GetUniCode(lpDlgTitle, TempStr);
				if(Place)
				{
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
				}
				if(_tcslen(TempStr) >= 64)	// Too Long?
					TempStr[63] = TEXT('\0');
				_tcscpy(DayTitle[Index][i], TempStr);
				i++;
				ActualNum[Index]++;
				if(dwAllDay)
					DayOneNum[Index]++;
				else
				{
					DayAllNum[Index]++;
//	For All Day Scroll
//					if(DayAllNum[Index] == dwTopDayLine && dwAppDispMode == 0)// Limit is 3 when All Display Mode
//						break;
				}
			}
		}
		if(AllMode)	// One Shot Mode?
		{
			while(DispTime <= GetTime(dwFinalTime + 1, 0))
			{	// Clear Display Buffer
				if(i >= MAXDAYBUF)	// Limit Check
					break;
				DayGenID[Index][i] = 0;	// Clear Current Line Seek Ptr
				DayAllDay[Index][i] = 1;	// Save Original AllDay Flag
				DayStartTime[Index][i] = DispTime;// Save Start Time
				AdvHalfHour(&DispTime);	// Advance 1 Hour
				i++;
				DayOneNum[Index]++;
			}
		}
	}
//	Build Blacket Array
//	Clear Bar Buf Array
	for(i = 0; i < 64; i++)	// Max 31 Hour * 2
		BarBuf[i] = 0;	// Clear Bar Buffer
	for(i = 0; i < MAXDAYBUF; i++)
		DayBlacket[Index][i] = 0;	// Claer Blacket Buffer
	for(i = DayAllNum[Index]; i < (DayAllNum[Index] + DayOneNum[Index]); i++)// Scan Disp Buffer
	{
		if(DayGenID[Index][i] == 0)
			continue;
		for(j = 0; j < 5; j++)
			if((DayBlacket[Index][i] & BlacketBitPat[j]) == 0)
				break;
		if(j == 4)	// No Space?
			continue;// Go Next Buffer
		if(DayStartTime[Index][i] == DayEndTime[Index][i])	// Pin Point Schedule?
		{
			DayBlacket[Index][i] |= 1 << (j * 3);// Set -
			continue;	// Go Next Buffer
		}
		EndTime = DayEndTime[Index][i];		// Get End Time Save
		DayBlacket[Index][i] |= 3 << (j * 3);	// Set Start Blacket
		k = i;// Copy Index
		while(k < MAXDAYBUF - 1 && DayStartTime[Index][++k] < EndTime)
			DayBlacket[Index][k] |=  2 << (j * 3);	// Search End Time Pos
		if(dwDispStep?((EndTime & 63) == 30 || (EndTime & 63) == 0): ((EndTime & 63) == 0))// If End Time is Just :00?
			DayBlacket[Index][k] |=  4 << (j * 3);	// Set End Blacket
//	Bulid Diagram
		FillLen =(ConvTimeToMin(DayEndTime[Index][i]) - ConvTimeToMin(DayStartTime[Index][i]) + 15) / 30;
		FillPos = (ConvTimeToMin(DayStartTime[Index][i]) + 15) / 30;
		if(FillPos < 96 && (FillLen + FillPos) <= 96 && FillLen)
			for(j = FillPos; j < FillPos + FillLen; j++)
				BarBuf[j]++;
	}
}
//
//	Day Screen Key Proc
//
void DayKeyProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
#ifdef	_WIN32_WCE
DWORD	Format = CF_UNICODETEXT;
#else
DWORD	Format = CF_TEXT;
#endif
DWORD	Duration;
BOOL	DeleteMode;
DWORD	DlgResult;
	switch(LOWORD(wParam))
	{
	case VK_BACK:
		break;
	case TEXT('\r'):	// Enter
		SendMessage(hWnd, WM_COMMAND, IDM_VIEW_INPUT, NULL);
		break;
	case TEXT('\033'):	// Change to Day Screen(ESC)
		SendMessage(hWnd, WM_COMMAND, IDM_VIEW_MONTH, NULL);
		break;
	case TEXT('\003'):	// Ctrl+C (Copy)?
		if(FileLock)		// Now File Working?
		{
			MessageBeep(MB_ICONASTERISK);	// Alarm
			break;		// No Operation
		}
		OpenClipboard(hWnd);
		EmptyClipboard();
		ClipBuf = 0;
		if(DayOrgIDBuf[dwDayLine] && SetupClipData(FALSE))	// Alloc Succeeds ?
			SetClipboardData(Format, ClipBuf);
NoClipSet:
		SetupMenu(FALSE);
		CloseClipboard();
		break;
	case TEXT('\030'):	// Ctrl+X (Cut)?
		if(FileLock)		// Now File Working?
		{
			MessageBeep(MB_ICONASTERISK);	// Alarm
			break;		// No Operation
		}
//		if(dwConfirm && MessageBox(hWnd, TEXT("Delete Highlighted Item?"), TEXT("Delete"),MB_OKCANCEL) != IDOK)
		if(dwConfirm && MessageBox(hWnd, TEXT(" ≤◊≤ƒÇ≥ÇÍÇΩ±≤√—ÇçÌèúÇµÇ‹Ç∑Ç©?"), TEXT("Delete"),MB_OKCANCEL) != IDOK)	// by uminchu
			break;
		OpenClipboard(hWnd);
		EmptyClipboard();
		ClipBuf = 0;
		if(DayOrgIDBuf[dwDayLine] == 0)
			goto NoClipSet;	// No Copy Action
		if(SetupClipData(FALSE))	// Alloc Succeeds ?
			SetClipboardData(Format, ClipBuf);
		dwDlgDate = GetCurrentDate();	// Set Date
		dwDlgStartTime = DayOrgTimeBuf[dwDayLine];// Set Time
		dwGenID = DayOrgIDBuf[dwDayLine];	// Set Record ID
		dwAllDay = DayOrgAllDayBuf[dwDayLine];// Set All Day Flag
		OyajiSeekGetRecord(&MainWork);// Get Record Data
		DeleteMode = TRUE;
		if(dwMultiConfirm && (lpDlgRepeat[0] || dwDlgConsDay != 1))
		{
//			DlgResult = MessageBox(hWnd, TEXT("Delete All Item?"), TEXT("Repeat Delete"),MB_YESNOCANCEL);
			DlgResult = MessageBox(hWnd, TEXT("ëSÇƒÇÃ±≤√—ÇçÌèúÇµÇ‹Ç∑Ç©?"), TEXT("Repeat Delete"),MB_YESNOCANCEL);	// by uminchu
			if(DlgResult == IDCANCEL)
				break;
			else if(DlgResult == IDNO)
				DeleteMode = FALSE;
		}
		if(DeleteMode)
			dwDlgDate = dwOrgDate;	// Set Top Date
		DelRecord(DeleteMode);	// BOOL but ignore
//	Set Pending GenID Here (To Erase)
		DayOrgIDBuf[dwDayLine] = 0;// Clear Line Seek Ptr
		CloseClipboard();
		SetupMenu(FALSE);
		SetupMainScreen(hMemDC);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case TEXT('\026'):	// Ctrl+V (Paste)?
		if(FileLock)	// Now File Working?
		{
			MessageBeep(MB_ICONASTERISK);	// Alarm
			break;		// No Operation
		}
		if(RestoreClipData(hWnd, FALSE) == FALSE)	// Alloc Succeeds ?
			break;
		dwDlgDate = GetCurrentDate();	// Set Date
		dwOrgDate =dwDlgDate;	// Begin from Target Day
		Duration = SubTime(dwDlgEndTime, dwDlgStartTime);// Get Length (Time Format)
		if(dwAppDispMode == 0)			// All Disp Mode
			dwDlgStartTime = DayOrgTimeBuf[dwDayLine];// Set Start Time
		dwDlgEndTime = AddTime(dwDlgStartTime, Duration);// Set End Time
		if(dwDlgEndTime > GetTime(31, 0))	//	31:00 ?
			dwDlgEndTime = GetTime(31, 0);	// Force Set 31:00
		dwGenID = 0;	// Clear Gen ID (New Mode)
		dwDlgMode = 0;	// New Add Mode
		dwDlgPrivate = dwPrivate;	// Force Set Private Mode
		AddRecord(TRUE, TRUE);// All Change Mode,but Return Val ignore
//	Set Pending GenID Here (To Erase)
		SetupMainScreen(hMemDC);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case TEXT('\032'):	// Ctrl+Z?
		if(FileLock)	// Now File Working?
		{
			MessageBeep(MB_ICONASTERISK);	// Alarm
				break;		// No Operation
		}
		OyajiUndoFile(&MainWork);
		SetupMainScreen(hMemDC);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
default:
		if(FileLock == 0 && wParam >= TEXT(' '))
		{
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_INPUT, NULL);
			SendMessage(GetFocus(), WM_CHAR, wParam, lParam);
		}
	}
}

void DrawFrame(HDC hDC, DWORD x, DWORD y, DWORD w, DWORD h)
{
POINT	LP[5];	// Arry for DrawLine
	LP[0].x = LP[4].x = LP[3].x = x;	// Frame Left Top Axis
	LP[0].y = LP[4].y = LP[1].y = y;
	LP[1].x = LP[2].x = x + w;
	LP[2].y = LP[3].y = y + h;
	Polyline(hDC, LP, 5);	// Draw
}
//
//	Day Screen Key Down Proc
//
void DayKeyDownProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
DWORD	LastYear, LastMonth, LastDay, i;
	LastYear = GetCurrentYear(0);
	LastMonth = GetCurrentMonth(0);
	LastDay = GetCurrentDay(0);
	if(wParam == VK_UP)
	{
		if(GetKeyState(VK_CONTROL) < 0)
		{
			if(dwYear == 1951 && dwMonth == 1)
				return;
			dwMonth--;
			if(dwMonth == 0)
			{
				dwYear--;
				dwMonth = 12;
			}
			SetupMonth(dwYear, dwMonth);
			while(GetCurrentMonth(0) > dwMonth)
				dwCurY--;
			while(GetCurrentMonth(0) < dwMonth)
				dwCurY++;
			goto CursorMoveEnd;
		}
		else if(GetKeyState(VK_SHIFT) < 0)// Shift + Cursor UP
		{
CursorUp:
			if(dwCurY == 0)
			{
				if(dwYear == 1951 && dwMonth == 1)
					return;
				dwCurY = 4;
				dwMonth--;
				if(dwMonth == 0)
				{
					dwYear--;
					dwMonth = 12;
				}
				SetupMonth(dwYear, dwMonth);
				if(GetCurrentMonth(0) == LastMonth
					&& GetCurrentDay(0) == LastDay)
					dwCurY--;
			}
			else
				dwCurY--;
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
			if(dwYear == 2077 && dwMonth == 12)
				return;
			dwMonth++;
			if(dwMonth == 13)
			{
				dwYear++;
				dwMonth = 1;
			}
			SetupMonth(dwYear, dwMonth);
			while(GetCurrentMonth(0) < dwMonth)
				dwCurY++;
			while(GetCurrentMonth(0) > dwMonth)
				dwCurY--;
			goto CursorMoveEnd;
		}
		else if(GetKeyState(VK_SHIFT) < 0)// Shift + Cursor DOWN
		{
CursorDown:
			if(dwCurY == 5)
			{
				if(dwYear == 2077 && dwMonth == 12)
					return;
				dwCurY = 0;
				dwMonth++;
				if(dwMonth == 13)
				{
					dwYear++;
					dwMonth = 1;
				}
				SetupMonth(dwYear, dwMonth);
				if(GetCurrentMonth(0) == LastMonth
					&& GetCurrentDay(0) == LastDay)
					dwCurY++;
			}
			else
				dwCurY++;
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
						DayScreen(hDC, FALSE, TRUE, FALSE);// Not Draw Calender & Not Set Cursor Pos
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
		if(dwCurX == 0)
		{
			dwCurX = 6;
			if(dwCurY == 0)
			{
				LastMonth = GetCurrentMonth(0);
				LastDay = GetCurrentDay(0);
				goto CursorUp;
			}
			dwCurY--;
		}
		else
			dwCurX--;
		goto CursorMoveEnd;
	}
	else if(wParam == VK_RIGHT)	// Cursor RIGHT
	{
		if(dwCurX == 6)
		{
			dwCurX = 0;
			if(dwCurY == 4)
				goto CursorDown;
			dwCurY++;
		}
		else
			dwCurX++;
CursorMoveEnd:
		JumpCurrentDay(0);
		DayScreen(hDC, (LastYear != dwYear || LastMonth != dwMonth), FALSE, TRUE);
		DispCalenderBox(hDC);
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
		DispLineCursor(hDC);
		InvalidateRect(hWnd, NULL, FALSE);
		return;
	}
}
//
//	Mouse Left Button Proc
//
void DayLeftProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
DWORD	Top = dwSmallFontHight + dwWinTop + 2;
DWORD	CurX, CurY, Page;
DWORD	LastYear = GetCurrentYear(0);
DWORD	LastMonth = GetCurrentMonth(0);
DWORD	BarPtr, Time;
	if(LOWORD(lParam) >= FrameWidth + dwSmallFontWidth * 28)
		return;
	else if(LOWORD(lParam) >= FrameWidth + dwSmallFontWidth)
	{
		CurX = (LOWORD(lParam) - FrameWidth - dwSmallFontWidth) / dwSmallFontWidth / 4;
		if(CurX > 6)
			CurX = 6;
		if(HIWORD(lParam) < dwSmallFontHight * 8 + dwWinTop)
		{
			Page = 0;
			if(HIWORD(lParam) <= dwSmallFontHight * 2 + dwWinTop)
			{
				if(dwYear == 1951 && dwMonth == 1)
					return;
				dwMonth--;
				if(dwMonth == 0)
				{
					dwYear--;
					dwMonth = 12;
				}
				SetupMonth(dwYear, dwMonth);
				while(MonthBuf[0][dwCurX][1] != dwMonth)
					dwCurX++;
				goto MoveMonth;
			}
			CurY = (HIWORD(lParam) - dwSmallFontHight * 2 - dwWinTop) / dwSmallFontHight;
		}
		else
		{
			if(dwToDoDay)
				return;
			if(HIWORD(lParam) <= dwSmallFontHight * 10 + dwWinTop)
			{
				if(dwYear == 2077 && dwMonth == 12)
					return;
				dwMonth++;
				if(dwMonth == 13)
				{
					dwYear++;
					dwMonth = 1;
				}
				SetupMonth(dwYear, dwMonth);
				Page = 0;
				while(MonthBuf[0][dwCurX][1] != dwMonth)
					dwCurX++;
				goto MoveMonth;
			}
			if(HIWORD(lParam) >= dwSmallFontHight * 16 + dwWinTop)
				return;
			Page = 1;
			CurY = (HIWORD(lParam) - dwSmallFontHight * 10 - dwWinTop) / dwSmallFontHight;
		}
		dwCurX = CurX;
		dwCurY = CurY;
MoveMonth:
		JumpCurrentDay(Page);
		DayScreen(hDC, (LastYear != dwYear || LastMonth != dwMonth), FALSE, TRUE);
		DispCalenderBox(hDC);
		InvalidateRect(hWnd, NULL, FALSE);
		return;	
	}
	else if(HIWORD(lParam) <= Top)
	{
		SendMessage(hWnd, WM_COMMAND, IDM_JUMP_DATE, 0);
		return;
	}
	if(dwGraphMode && LOWORD(lParam) < BarLeft)
	{
		if(dwAppDispMode)
			return;	// Hide Mode Bar Touch Case 
		if(HIWORD(lParam) <= dwSmallFontHight / 2 + Top)// To Low?
			return;
		BarPtr = HIWORD(lParam) - dwSmallFontHight / 2 - Top;// Get Offset
		if(BarPtr >= BarHight)	// Over?
			return;
		Time = dwStartTime + BarPtr / (BarUnit * 2);// Get Time
		if(Time >= dwFinalTime)// Time is Over?
			Time = dwFinalTime;
		Time = GetTime(Time, 0);// Convert to DWORD Time
		StartTime = dwStartTime * 2;	// Set Day Screen Start Time
		CurrentTime = StartTime;		// Current Select Time
		DayScreen(hDC, FALSE, FALSE, TRUE);// Not Draw Calender & Not Set Cursor Pos
		if(DayOrgTimeBuf[dwDayLine] < Time)
		{
			while(DayOrgTimeBuf[dwDayLine] < Time)
				SendMessage(hWnd, WM_KEYDOWN, VK_DOWN, 0);
		}
		else
			InvalidateRect(hWnd, NULL, FALSE);// Start Time Case
		return;
	}
	dwDayLine = (HIWORD(lParam) - Top) / LineHight;
	if(dwDayLine > DayLineCount - 1)
		dwDayLine = DayLineCount - 1;
	if(dwAppDispMode && dwDayLine >  ActualNum[0] - HideSkip)
		dwDayLine = ActualNum[0] - HideSkip;
	DispLineCursor(hDC);
	InvalidateRect(hWnd, NULL, FALSE);
}

//
//	[iwad] Mouse Right Button Proc
//
void DayRightProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
DWORD	Top = dwSmallFontHight + dwWinTop + 2;
DWORD	Page;
	if(HIWORD(lParam) <= Top)
		return;
	if(LOWORD(lParam) >= FrameWidth + dwSmallFontWidth * 28)
		return;
	if(dwAppDispMode == 0 && dwGraphMode && LOWORD(lParam) <= BarLeft)
		return;
	else if(LOWORD(lParam) >= FrameWidth + dwSmallFontWidth)
	{
		if(HIWORD(lParam) < dwSmallFontHight * 8 + dwWinTop)
			Page = 0;
		else
			Page = 1;
		if(dwToDoDay && Page == 1)
		{
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_TODO, 0);
			return;
		}
		JumpCurrentDay(Page);	// Enable 30,31 on Left Top
		SendMessage(hWnd, WM_COMMAND, IDM_VIEW_MONTH, 0);
		return;
	}
	dwDayLine = (HIWORD(lParam) - Top) / LineHight;
	if(dwDayLine > DayLineCount - 1)
		dwDayLine = DayLineCount - 1;
	if(dwAppDispMode && dwDayLine >  ActualNum[0] - HideSkip)
		dwDayLine = ActualNum[0] - HideSkip;
	SendMessage(hWnd, WM_CHAR, TEXT('\r'), NULL);
}
//
//	Double Click Proc
//
void DayDblProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
DWORD	Top = dwSmallFontHight + dwWinTop + 2;
DWORD	Page;
	if(HIWORD(lParam) <= Top)
		return;
	if(LOWORD(lParam) >= FrameWidth + dwSmallFontWidth * 28)
		return;
	if(dwAppDispMode == 0 && dwGraphMode && LOWORD(lParam) <= BarLeft)
		return;
	else if(LOWORD(lParam) >= FrameWidth + dwSmallFontWidth)
	{
		if(HIWORD(lParam) < dwSmallFontHight * 8 + dwWinTop)
			Page = 0;
		else
			Page = 1;
		if(dwToDoDay && Page == 1)
		{
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_TODO, 0);
			return;
		}
		JumpCurrentDay(Page);	// Enable 30,31 on Left Top
		SendMessage(hWnd, WM_COMMAND, IDM_VIEW_MONTH, 0);
		return;
	}
	dwDayLine = (HIWORD(lParam) - Top) / LineHight;
	if(dwDayLine > DayLineCount - 1)
		dwDayLine = DayLineCount - 1;
	if(dwAppDispMode && dwDayLine >  ActualNum[0] - HideSkip)
		dwDayLine = ActualNum[0] - HideSkip;
	SendMessage(hWnd, WM_CHAR, TEXT('\r'), NULL);
}
//
//	Cut '&' char
//
void UnderCut(LPTSTR Buf)
{
TCHAR	Temp[4096];
TCHAR	*src, *dst;
	_tcscpy(Temp, Buf);
	for(src = Temp, dst = Buf; *src; src++)
		if(*src != TEXT('&'))
			*dst++ = *src;
	*dst = TEXT('\0');
}
//
//	Convert char to TCHAR (For Unicode)
//
void GetUniCode(char *src, LPTSTR dst)
{
#ifdef	_WIN32_WCE
#ifdef	_KCTRL
	sjis2unicode((BYTE *)src, dst, 4096 );
#else
DWORD	TextLen;
	TextLen = MultiByteToWideChar(0,0,src,-1,NULL,NULL);
	MultiByteToWideChar(0,0,src,-1,dst,TextLen);
#endif
#else	/* 95,NT */
	_tcscpy(dst, src);
#endif
}
//
//	Convert char to TCHAR (For Unicode)
//
DWORD GetSJIS(LPTSTR src, char *dst, DWORD MaxLen)
{
DWORD	TextLen;
#ifdef	_WIN32_WCE
#ifdef	_KCTRL
	unicode2sjis( src, (BYTE *)dst, MaxLen );
	TextLen = strlen( dst );
	return(( MaxLen - 1 )  > TextLen ? TextLen : MaxLen - 1 );
#else
	TextLen = WideCharToMultiByte(0,0,src,-1,NULL,NULL,0,0);
	if(TextLen > MaxLen)
		TextLen = MaxLen;
	WideCharToMultiByte(0,0,src,-1,dst,TextLen,0,0);
	return TextLen - 1;	// Decrement for NULL
#endif
#else	/* 95,NT */
	TextLen = strlen(src);
	if(TextLen > MaxLen)
		*(src + MaxLen) = '\0';
	strcpy(dst, src);
	return strlen(src);
#endif
}
#ifndef	_WIN32_WCE
//
//	Test 1st Byte of SJIS
//
BOOL isKanji(unsigned char ch)
{
	if(ch >= 0x81 && ch <=0x9f)
		return TRUE;
	return (ch >=0xe0 && ch <= 0xfc);
}
#endif
//
//	Setup Clip Board Text Data
//
BOOL SetupClipData(BOOL Csv)
{
FIELD	*fptr;
TCHAR	String[4096];
TCHAR	NumStr[32];
DWORD	TextLen;
DWORD	Val, i, j, YenCount, StrLen;
TCHAR	AddString[256];
	if(Csv == FALSE)
	{
		SetupCurrentPos();
		if(OyajiSeekGetRecord(&MainWork) == FALSE)// Get Record Data
			return FALSE;	// Read Error then Return Blank
		dwDlgDate = dwOrgDate;	// Set Top Date
	}
	ClipBuf = (LPTSTR )LocalAlloc(LMEM_FIXED, sizeof(TCHAR));// Minimum Alloc 
	if(ClipBuf == NULL)
		return FALSE;
	_tcscpy(ClipBuf, TEXT(""));	// Set EOS Anyway
	for(fptr = MainWork.FieldList; fptr->Attr; fptr++)
	{	// Repeat for All Field
InvalidFlow:
		if(fptr->Width == 0)	// Blank Field?
			continue;		// Skip
		switch(fptr->Attr)
		{
		case FLQWORD:
		case FLSINGLE:
		case FLDOUBLE:
		case FLCURRENCY:
			break;
		case FLDATE:
			_tcscpy(String, GetDateString(*(unsigned short *)fptr->Ptr));
			goto StrSet;
		case FLTIME:
			if(dwAllDay == 0)
			{
				_tcscpy(String, TEXT("None "));
				_tcscat(String, GetTimeString(*(unsigned short *)fptr->Ptr));
			}
			else
				_tcscpy(String, GetTimeString(*(unsigned short *)fptr->Ptr));
			goto StrSet;
		case FLRADIO:
		case FLCHECK:
			Val = *(unsigned char *)fptr->Ptr;
			fptr++;	// Get Next Field
			if(fptr->Attr != FLSTATIC)	// Right Order?
				goto InvalidFlow;	// Invalid Order
			if(Csv)
				wsprintf(String, TEXT("%d"), Val);
			else
			{
				GetUniCode((char *)fptr->Ptr, String);
				UnderCut(String);
				wsprintf(NumStr, TEXT(":%d"), Val);
				_tcscat(String, NumStr);
			}
			goto StrSet;
			break;
		case FLBYTE:
			Val = *(unsigned char *)fptr->Ptr;
			goto SetNum;
		case FLSHORT:
			Val = *(unsigned short *)fptr->Ptr;
			goto SetNum;
		case FLDWORD:
			Val = *(DWORD *)fptr->Ptr;
SetNum:
			wsprintf(String, TEXT("%d"), Val);
			goto StrSet;
		case FLSTRING:
			GetUniCode((char *)fptr->Ptr, String);
			goto StrSet;
		case FLMULTIEDIT:
			TextLen = *(unsigned short *)fptr->Ptr;
			if(TextLen)
				GetUniCode((char *)fptr->Ptr + 2, String);
			else
				_tcscpy(String, TEXT(""));
StrSet:
			YenCount = 0;
			StrLen = 0;
			for(i = 0; String[i]; StrLen++, i++)
				if(String[i] == TEXT('\\') || String[i] == TEXT('"'))
					YenCount++;
			ClipBuf = (LPTSTR )LocalReAlloc(ClipBuf, (_tcslen(ClipBuf) + _tcslen(String) + 3 + Csv + YenCount) * sizeof(TCHAR), LMEM_MOVEABLE);
			if(ClipBuf == NULL)
				return FALSE;
			if(Csv)
				_tcscat(ClipBuf, TEXT("\""));
			for(i = 0; String[i]; i++)// Loop Until EOS
			{	// Convert CR+LF to '\n' 
#ifndef	_WIN32_WCE
				if(isKanji(String[i]))
				{
					i++;
					continue;
				}
#endif
				if(String[i] == TEXT('\\'))	// Char = '\'
				{
					for(j = 0; j < StrLen - i + 1; j++)
						String[StrLen - j + 1] = String[StrLen - j];
					i++;
					StrLen++;
					continue;
				}
				else if(String[i] == TEXT('"'))	// Char = '"'
				{
					for(j = 0; j < StrLen - i + 1; j++)
						String[StrLen - j + 1] = String[StrLen - j];
					String[i] = TEXT('\\');
					i++;
					StrLen++;
					continue;
				}
				else if(String[i] == TEXT('\015'))	// Char = CR?
					String[i] = TEXT('\\');
				else if(String[i] == TEXT('\012'))	// Char = LF?
					String[i] = TEXT('n');
			}
			_tcscat(ClipBuf, String);
			if(Csv)
				_tcscat(ClipBuf, TEXT("\","));	// Set EOL
			else
				_tcscat(ClipBuf, TEXT("\015\012"));	// Set EOL
			break;
		case FLREPEAT:
			if(*(unsigned char *)fptr->Ptr == 0)// No Repeat ?
			{
				_tcscpy(String, TEXT("None"));
				goto StrSet;
			}
//	Copy Date String because Result is Static
			_tcscpy(AddString, GetDateString(lpDlgRepeat[4] * 256 + lpDlgRepeat[3]));
			wsprintf(String, TEXT("%s %s %s %d"),// Make Repeat String Head
					RepeatName[lpDlgRepeat[1]], AddString,
					GetDateString(lpDlgRepeat[6] * 256 + lpDlgRepeat[5]),
					lpDlgRepeat[2]);
			if(lpDlgRepeat[1])
			{
				if(lpDlgRepeat[8])// By Pos?
					_tcscat(String, TEXT(" Pos"));// Set By Pos
				else
					_tcscat(String, TEXT(" Date"));// Set By Date
				wsprintf(AddString, TEXT(" %d/%d"),
						((lpDlgRepeat[10] * 256 + lpDlgRepeat[9]) / 32) & 15,
						lpDlgRepeat[9] & 31);
				_tcscat(String, AddString);
				Val = lpDlgRepeat[7];
				j = 1;
				for(i = 0; i < 7; i++, j *= 2)
					if(Val & j)
					{
						wsprintf(AddString, TEXT(" %s"), ShortDayName[i]);
						_tcscat(String, AddString);
					}
				Val = lpDlgRepeat[11];
				j = 1;// Make Week Pos String
				for(i = 0; i < 5; i++, j *= 2)
					if(Val & j)
					{
						wsprintf(AddString, TEXT(" %s"), WeekPosName[i]);
						_tcscat(String, AddString);
					}
				Val = lpDlgRepeat[13] * 256 + lpDlgRepeat[12];
				j = 1;
				for(i = 0; i < 12; i++, j *= 2)
					if(Val & j)
					{
						wsprintf(AddString, TEXT(" %s"), ShortMonthStr[i]);
						_tcscat(String, AddString);
					}
			}
			goto StrSet;
		case FLSTATIC:
			if(Csv)
				break;
			GetUniCode((char *)fptr->Ptr, String);
			UnderCut(String);
			ClipBuf = (LPTSTR )LocalReAlloc(ClipBuf, (_tcslen(ClipBuf) + _tcslen(String) + 2) * sizeof(TCHAR), LMEM_MOVEABLE);
			if(ClipBuf == NULL)
				return FALSE;
			_tcscat(ClipBuf, String);
			_tcscat(ClipBuf, TEXT(":"));
			break;
		}
	}
	if(Csv)	// Add Last CR/LF
	{
		ClipBuf = (LPTSTR )LocalReAlloc(ClipBuf, (_tcslen(ClipBuf) + 2) * sizeof(TCHAR), LMEM_MOVEABLE);
		if(ClipBuf == NULL)
			return FALSE;
		if(_tcslen(ClipBuf))
			_tcscpy(ClipBuf + _tcslen(ClipBuf) - 1, TEXT("\015\012"));	// Cut Last ,
	}
	return TRUE;
}
//
//	Restore Work from Clip Board
//
BOOL RestoreClipData(HWND hWnd, BOOL Csv)
{
FIELD	*fptr;
DWORD	TextLen, i, TempDate;
DWORD	TempDay, TempWeek, TempMonth, SetBit;
HANDLE	hClip;
LPTSTR	ClipData, StrPtr, ClipPtr = 0;
LPTSTR	Token, TokenPtr;
TCHAR	LineBuf[4096];
TCHAR	String[256];
DWORD	DateLen;
LPTSTR	StorePtr;
#ifdef	_WIN32_WCE
DWORD	Format = CF_UNICODETEXT;
#else
DWORD	Format = CF_TEXT;
#endif
//
//	Set Defaults for CSV blank (,,)
//
	dwDlgStartTime = dwStartTime;
	dwDlgEndTime = dwStartTime + dwDefLength;
	dwDlgConsDay = 1;
	dwDlgDate = GetCurrentDate();	// Set Date Default
	for(i = 0; i < 14; i++)
		lpDlgRepeat[i] = 0;	// Clear Repeat Status
	dwAllDay = 1;	// One Shot is Default
	strcpy(lpDlgTitle, "");	// Clear Title
	strcpy(&lpDlgNote[2], "");
	strcpy(lpDlgLocation, "");	// Clear Location
	lpDlgNote[0] = 0;	// Clear Note
	lpDlgNote[1] = 0;
	dwDlgLeadTime = dwPDefLeadTime;	// Lead Time 5 Min
	dwDlgWeekView = dwPDefWeek;	// Week View On
	dwDlgDayView = dwPDefDay;		// Day View On
	dwDlgGrayView = dwPDefGray;		// Day View On
	dwDlgMonthView = dwPDefMonth;	// Month View On
	dwDlgEnable = dwPDefEnable;	// Alarm Disenable
	if(Csv)
	{	// Get Data From Memory
		ClipData = CsvBuf;
	}
	else
	{	// Get Data From Clip Board
		OpenClipboard(hWnd);
		hClip = GetClipboardData(Format);
		if(hClip == NULL)	// Format Exists?
			goto EmptyExit;	// Fail
		ClipPtr = (LPTSTR )LocalAlloc(LMEM_FIXED, LocalSize(hClip));	// Get Clip Data Size
		if(ClipPtr == NULL)	// Allocate Succeed?
		{
EmptyExit:
			CloseClipboard();
			return FALSE;	// No Clip Board Data
		}
		StrPtr = (LPTSTR )LocalLock(hClip);	//	Lock Clip Data
		_tcscpy(ClipPtr, StrPtr);
		LocalUnlock(hClip);	// Release Memory
		CloseClipboard();	// Close Clip
		ClipData = ClipPtr;
	}
	while(TRUE)	// Forever
	{	// Get 1 Line to LineBuf
	TCHAR	ch;
		StorePtr = LineBuf;	// Set Buffer Top Adrs
		while(ch = *ClipData++)	// Repeat until EOL
		{
#ifndef	_WIN32_WCE
			if(isKanji(ch))
			{
				*StorePtr++ = ch;	// Store Normal
				*StorePtr++ = *ClipData++;	// Store Normal
				continue;
			}
#endif
			if(ch == TEXT('\015'))	// CR?
				break;
			if(ch == TEXT('\012'))	// LF?
				continue;
			else if(ch == TEXT('\\') && *ClipData == TEXT('n'))	// Check '\n'
			{
				*StorePtr++ = TEXT('\015');	// Store CR
				*StorePtr++ = TEXT('\012');	// Store LF
				ClipData++;
			}
			else if(ch == TEXT('\\') && *ClipData == TEXT('\\'))	// Check '\\'
			{
				*StorePtr++ = TEXT('\\');	// Store LF
				ClipData++;
			}
			else if(ch == TEXT('\\') && *ClipData == TEXT('"'))	// Check '\\'
			{
				*StorePtr++ = TEXT('"');	// Store LF
				ClipData++;
			}
			else
				*StorePtr++ = ch;	// Store Normal
		}
		*StorePtr = TEXT('\0');	// Store EOS
		if(LineBuf[0] == TEXT('\0'))	// NULL Line?
			break;	// Exit while
		StrPtr = _tcschr(LineBuf, TEXT(':'));	// Check ':' exists
		if(StrPtr == NULL)
			break;	// Invalid Format
		*StrPtr++ = TEXT('\0');	// Clear ':' & Point Next of ':'
		for(fptr = MainWork.FieldList; fptr->Attr; fptr++)
		{	// Repeat for All Field
			if(fptr->Attr != FLSTATIC)	// Blank Field?
				continue;		// Skip
			if(fptr->Width == 0)	// Blank Field?
				continue;		// Skip
			GetUniCode((char *)fptr->Ptr, String);	// Convert TCHAR
			UnderCut(String);	// Erase '&' Char
			if(_tcscmp(String, LineBuf) == 0)	// Search Match Field
				break;	// Exit Field Name Search Loop
		}
		if(fptr->Attr == 0)	// Not Found?
			continue;	// Get Next Clip Board Data
		if(fptr != MainWork.FieldList)	// Not Top Field?
		{
			fptr--;	// Check Prev Field
			if(fptr->Attr == FLCHECK || fptr->Attr == FLRADIO)
				goto AttrFound;	// Target Field is Prev Field (Only Check & Radio)
			fptr++;	// Restore Pointer
		}
		fptr++;	// Move to Next Field
		if(fptr->Attr == 0)	// End of Table?
			break;	// End of Restore Clip Board Data
AttrFound:
		switch(fptr->Attr)	// Found then Convert TEXT to Buffer
		{
		case FLCHECK:
		case FLRADIO:
		case FLBYTE:
			*(unsigned char *)fptr->Ptr = (unsigned char )_ttoi(StrPtr);
			break;
		case FLSHORT:
			*(unsigned short *)fptr->Ptr = (unsigned short)_ttoi(StrPtr);
			break;
		case FLDWORD:
			*(DWORD *)fptr->Ptr = _ttoi(StrPtr);
			break;
		case FLDATE:
			if(ConvDateToDword((LPDWORD )fptr->Ptr, StrPtr) == FALSE)
				goto ClipError;
			break;
		case FLTIME:
			if(_tcsstr(StrPtr, TEXT("None ")) == StrPtr)
			{
				dwAllDay = 0;	// All Day Mode
				StrPtr += 5;	// Skip "None "
			}
			if(ConvTimeToDword((LPDWORD )fptr->Ptr, StrPtr) == FALSE)
				goto ClipError;
			break;
		case FLSTRING:
			GetSJIS(StrPtr, (char *)fptr->Ptr, fptr->MaxLen);
			break;
		case FLMULTIEDIT:
			TextLen = GetSJIS(StrPtr, (char *)fptr->Ptr + 2, fptr->MaxLen);
			*(unsigned short *)fptr->Ptr = (unsigned short)TextLen;	// Set Len
			break;			
		case FLREPEAT:
			if(Csv == FALSE)	// Non CSV Mode (Not Import)
				break;		// No Repeat Infos
			Token = _tcstok(StrPtr, TEXT(" "));
			if(Token == NULL)
				break;
			for(i = 0; i < 5; i++)
				if(_tcscmp(Token, RepeatName[i]) == 0)
					break;
			if(i == 0 || i == 5)	// None or Unknown
				break;
			lpDlgRepeat[1] = (unsigned char)i;
//	Get Start Date
			Token = _tcstok(NULL, TEXT(" "));
			if(Token == NULL)
				break;
			DateLen = _tcslen(Token);
			if(ConvDateToDword(&TempDate, Token) == FALSE)
				break;
			lpDlgRepeat[3] = LOBYTE(TempDate);
			lpDlgRepeat[4] = HIBYTE(TempDate);
//	Get End Date
			Token = _tcstok(Token + DateLen + 1, TEXT(" "));
			if(Token == NULL)
				break;
			DateLen = _tcslen(Token);
			if(ConvDateToDword(&TempDate, Token) == FALSE)
				break;
			lpDlgRepeat[5] = LOBYTE(TempDate);
			lpDlgRepeat[6] = HIBYTE(TempDate);
//	Get Frequency
			Token = _tcstok(Token + DateLen + 1, TEXT(" "));
			if(Token == NULL)
				break;
			lpDlgRepeat[2] = (unsigned char)_ttoi(Token);
			if(lpDlgRepeat[2] == 0)
				break;	// Syntax Error
			Token = _tcstok(NULL, TEXT(" "));
			if(Token == NULL)
				break;	// Syntax Error
			if(_tcscmp(Token, TEXT("Pos")) == 0)
				lpDlgRepeat[8] = TRUE;
			else if(_tcscmp(Token, TEXT("Date")))
				break;	// Syntax Error
			Token = _tcstok(NULL, TEXT(" "));
			if(Token == NULL)
				break;	// Syntax Error
			DateLen = _tcslen(Token);
			if(ConvDateToDword(&TempDate, Token) == FALSE)
				break;
			TempDate &= 0x1ff;
			lpDlgRepeat[9] = LOBYTE(TempDate);
			lpDlgRepeat[10] = HIBYTE(TempDate);
			TokenPtr = Token + DateLen + 1;// Set for 1st Search
			lpDlgRepeat[0] = 13;	// Enable
			TempDay = 0;
			TempWeek = 0;
			TempMonth = 0;
			while(TRUE)
			{
				Token = _tcstok(TokenPtr, TEXT(" "));
				TokenPtr = NULL;	// For Second Token
				if(Token == NULL)
					break;	// End Get Week Day
				for(i = 0, SetBit = 1; i < 7; i++, SetBit *= 2)
					if(_tcscmp(ShortDayName[i], Token) == 0)
						break;
				if(i != 7)
				{
					TempDay |= SetBit;
					continue;	// Get Next Token
				}
				for(i = 0, SetBit = 1; i < 5; i++, SetBit *= 2)
					if(_tcscmp(WeekPosName[i], Token) == 0)
						break;
				if(i != 5)
				{
					TempWeek |= SetBit;
					continue;	// Get Next Token
				}
				for(i = 0, SetBit = 1; i < 12; i++, SetBit *= 2)
					if(_tcscmp(ShortMonthStr[i], Token) == 0)
						break;
				if(i != 12)
				{
					TempMonth |= SetBit;
					continue;	// Get Next Token
				}
			}
			lpDlgRepeat[7] = (unsigned char)TempDay;
			lpDlgRepeat[11] = (unsigned char)TempWeek;
			lpDlgRepeat[12] = LOBYTE(TempMonth);
			lpDlgRepeat[13] = HIBYTE(TempMonth);
		}
	}
	if(strlen(lpDlgTitle) == 0)
		goto ClipError;
	if(ClipPtr)
		LocalFree(ClipPtr);
	return TRUE;	// Clip Data is Right
//
ClipError:
	if(ClipPtr)
		LocalFree(ClipPtr);
	return FALSE;
}
//
//	Convert Time to Minutes
//
DWORD ConvTimeToMin(DWORD Time)
{
	return (Time / 64) * 60 + (Time & 63);
}
//
//	Convert Minutes to Time
//
DWORD ConvMinToTime(DWORD Min)
{
	return (Min / 60) * 64 + (Min % 60);
}
//
//	Add Time Sub
//
DWORD AddTime(DWORD Time, DWORD Add)
{
	return ConvMinToTime(ConvTimeToMin(Time) + ConvTimeToMin(Add));
}
//
//	Sub Time Sub
//
DWORD SubTime(DWORD Time, DWORD Add)
{
	return ConvMinToTime(ConvTimeToMin(Time) - ConvTimeToMin(Add));
}
//
//	Get Date DWORD from Year,Month,Day
//
DWORD GetDate(DWORD Year, DWORD Month, DWORD Day)
{
	return (Year - 1950) * 512 + Month * 32 + Day;
}
//
//	Get Time DWORD from Hour,Min
//
DWORD GetTime(DWORD Hour, DWORD Min)
{
	return Hour * 64 + Min;
}
