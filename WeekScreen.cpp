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
//	Macros
//
#define	countof(x)	(sizeof(x)/(sizeof((x)[0])))

//
//	ProtTypes
//
void	SetupMonth(DWORD, DWORD);
void	JumpDay(DWORD, DWORD, DWORD);
void	JumpCurrentDay(DWORD);
void	DispWeekBox(HDC);
void	ReverseWeekBox(HDC, DWORD);
void	DayKeyProc(HWND, HDC, WPARAM, LPARAM);
void	SetupMainScreen(HDC);
void	SetupMenu(BOOL);
void	GetToday(void);
void	SetupWeek(void);
void	GetUniCode(char *, LPTSTR);
void	AdvHalfHour(LPDWORD);	// Advance 1 Hour or 30 Min
void	SetupOneDayArray(DWORD, DWORD, BOOL, DWORD);
void	SetupBrush(HDC);
LPCTSTR	GetTimeString(DWORD);
DWORD	ConvTimeToMin(DWORD);
DWORD	ConvMinToTime(DWORD);
DWORD	GetTime(DWORD, DWORD);
DWORD	GetCurrentYear(DWORD);
DWORD	GetCurrentMonth(DWORD);
DWORD	GetCurrentDay(DWORD);
DWORD	GetCurrentWeekDay(void);
DWORD	GetCurrentDate(void);
DWORD	GetDayTotal(void);
DWORD	GetDate(DWORD, DWORD, DWORD);
DWORD	OyajiSearchRecord(FILEWORKS *);
BOOL	OyajiGetRecord(FILEWORKS *);
BOOL	OyajiSeekGetRecord(FILEWORKS *);
BOOL	OyajiUndoFile(FILEWORKS *);
BOOL	DelRecord(BOOL);
BOOL	AddRecord(BOOL, BOOL);
DWORD	ConvHalfHour(DWORD);
DWORD	AddTime(DWORD, DWORD);
DWORD	SubTime(DWORD, DWORD);
void	DispWeekBox(HDC);
BOOL	SetupClipData(BOOL);
BOOL	RestoreClipData(HWND, BOOL);
//
//	Global Vars
//
//
static DWORD	dwOldWeekCurX, dwOldWeekCurY;
static DWORD	WeekBoxHight, WeekBoxWidth;	// Size of Day Box
static DWORD	WeekStartIndex[7];			// One Shot Schedule Start Index
static DWORD	WeekAllStartIndex[7];		// All Day Schedule Start Index
static DWORD	WeekHideSkip[7];
static DWORD	WeekLineCount;
//
//	Setup Week Display Screen to Memory Device
//
//	Setup: 0:Just Cussor Move 1:for Scroll Redraw 2:Setup New Week Arrays
void WeekScreen(HDC hDC, BOOL Setup, BOOL SetCursor)
{
POINT	LinePoint[5];	// Arry for DrawLine
RECT	txRect;			// Rect for DrawText
TCHAR	LeftChar, RightChar;
DWORD	LineLeft = 0;		// Draw Left Pos
DWORD	APMode;
DWORD	i, j, k, l;	// Temp Counter
DWORD	DispYear, DispMonth;	// Display Year & Month
DWORD	DispMin = 0;
DWORD	TimeLeft, BarLeft, BarStep;
TCHAR	TempStr[256];// Title Buffer
DWORD	TempIndex;	// Temp Array Index
//	Set Font,Pen,Brush
	SetupBrush(hDC);
	SelectObject(hDC, hMidFont);		// Set Font
//	Calc Box Size
	WeekBoxHight = dwMidFontHight + 1;	// Get DayBox Height
	WeekLineCount = (dwYsize - 2) / WeekBoxHight;	// Get WeekLineCount in the Box
	if(WeekLineCount >= dwFinalTime * (dwDispStep + 1) + dwTopWeekLine + 1)	// 00:00 to 31:00 ?
		WeekLineCount = dwFinalTime * (dwDispStep + 1) + dwTopWeekLine + 1;	// Force Set 00:00 to 31:00
	if(WeekLineCount >= MAXDAYLINE)
		WeekLineCount = MAXDAYLINE;
	WeekBoxWidth = dwXsize / 8;	// Get DayBox Width
	TimeLeft = (dwWeekFloat)?(dwCurX * WeekBoxWidth + 1): 0;
	BarStep = WeekBoxWidth / 20;
	GetToday();		// Get Today's Data
	if(Setup)
	{
		PatBlt(hDC, 0, 0, dwXsize, dwYsize, PATCOPY);// Clear Memory Device
		dwOldWeekCurX = 8;	// Cursor is New
	}
	if(Setup > 1)
	{
		SetupWeek();	// Setup Week Buffer
		for(i = 0; i < 7; i++)
		{
			for(j = DayAllNum[i]; j < (DayAllNum[i] + DayOneNum[i]); j++)// Scan Disp Buffer
				if(ConvHalfHour(DayStartTime[i][j]) >= StartTime)
					break;	// Get Display Start Time Found
			WeekStartIndex[i] = j - DayAllNum[i];
			WeekHideSkip[i] = 0;	// Clear if Hide Mode
			WeekAllStartIndex[i] = 0;
		}
	}
//	All WeekDay Sync
	if(Setup == 1 && dwAppDispMode == 0 && dwWeekSync)
	{
		StartTime = ConvHalfHour(DayStartTime[dwCurX][DayAllNum[dwCurX] + WeekStartIndex[dwCurX]]);
		for(i = 0; i < 7; i++)
		{
			if(i == dwCurX)
				continue;
			for(j = DayAllNum[i]; j < (DayAllNum[i] + DayOneNum[i]); j++)// Scan Disp Buffer
				if(ConvHalfHour(DayStartTime[i][j]) >= StartTime)
					break;	// Get Display Start Time Found
			WeekStartIndex[i] = j - DayAllNum[i];
		}
	}
//	Disp Year,Month & Day
	DispYear = GetCurrentYear(0);	// Get Center Year
	DispMonth = GetCurrentMonth(0);	// Get Center Month
	if(dwWeekFloat == 0)	// Hide Year & Month if Float Mode
	{
		PatBlt(hDC, 0, 0, WeekBoxWidth, dwMidFontHight + 1, PATCOPY);
		// [iwad] txRect.top = 0;
		txRect.top = 5;
		txRect.bottom = dwMidFontHight;
		txRect.left = 0;
		txRect.right = txRect.left + WeekBoxWidth - 1;
		if(dwDateFormat)
			wsprintf(TempStr, TEXT("%s '%2.2d"),	// Make Month & Year String
				ShortMonthStr[DispMonth - 1], DispYear % 100);
		else
			wsprintf(TempStr, TEXT("%d/%d"),	// Make Month & Year String
				DispYear, DispMonth);
			DrawText(hDC, TempStr, -1, &txRect, DT_CENTER | DT_VCENTER);// Draw Month & Year(2 digit)
		if(dwPrivate)
			PatBlt(hDC, 0, 0, WeekBoxWidth, dwMidFontHight + 1, PATINVERT);
	}
//	Draw Horizontal lines
	if(Setup)
	{
		LinePoint[0].x = 0;	// Left Edge
		LinePoint[1].x = WeekBoxWidth * 8 - 1;	// Set Right Edge
		// [iwad] LinePoint[0].y = LinePoint[1].y = WeekBoxHight;
		LinePoint[0].y = LinePoint[1].y = WeekBoxHight + 5;
		Polyline(hDC, LinePoint, 2);	// Draw Top Horizontal Line
		// [iwad] LinePoint[0].y = LinePoint[1].y = WeekBoxHight * WeekLineCount + 1;
		LinePoint[0].y = LinePoint[1].y = WeekBoxHight * WeekLineCount + 6;
		LinePoint[1].x++;	// Inc Right Edge
		Polyline(hDC, LinePoint, 2);	// Draw Bottom Horizontal Line
//	Draw Day Name & Virtical Lines
		LinePoint[0].y = 0;	// 1st Line
		// [iwad] LinePoint[1].y = WeekBoxHight * WeekLineCount + 1;
		LinePoint[1].y = WeekBoxHight * WeekLineCount + 6;	// 1st Line
		for(i = 0; i < 8; i++)	// Repeat for Sun to Sat
		{	// Check is it Today?
			if(WeekBuf[i][0] == dwTodayYear && WeekBuf[i][1] == dwTodayMonth && WeekBuf[i][2] == dwTodayDay)
			{	// Set Blacket Char
				LeftChar = TEXT('<');
				RightChar = TEXT('>');
			}
			else// Set Blank Char
				LeftChar = RightChar = TEXT(' ');
			LinePoint[0].x = (i + 1) * WeekBoxWidth;
			LinePoint[1].x = LinePoint[0].x;
			if(dwWeekFloat == 0 || dwCurX != i)	// Erase Center Line if Float
				Polyline(hDC, LinePoint, 2);	// Draw Virtical Line
			if(i == 7)	// Right Edge Line?
				break;	// Exit for
			if(dwWeekFloat && i <= dwCurX)
			{
				if(i == dwCurX)
				{
					txRect.left = i * WeekBoxWidth + 1;	// Left Pos of Line Cursor
					txRect.right = txRect.left + WeekBoxWidth * 2 - 1;// Double Space
				}
				else
				{
					txRect.left = i * WeekBoxWidth + 1;	// Left Pos of Line Cursor
					txRect.right = txRect.left + WeekBoxWidth - 1;// Double Space
				}
			}
			else
			{
				txRect.left = (i + 1) * WeekBoxWidth + 1;	// Left Pos of Line Cursor
				txRect.right = txRect.left + WeekBoxWidth - 1;// Single Space
			}
//	Draw Day of the Week & Date
			// [iwad] txRect.top = 0;
			txRect.top = 5;
			// [iwad] txRect.bottom = dwMidFontHight;
			txRect.bottom = dwMidFontHight + 5;
			if(dwWeekFloat && i == dwCurX)
			{
				if(dwDateFormat)// US Format?
					wsprintf(TempStr, TEXT("%c%s %s %d,%2.2d%c"),	// Make Month & Year String
					LeftChar, ShortDayName[i], ShortMonthStr[WeekBuf[i][1] - 1], WeekBuf[i][2], WeekBuf[i][0] % 100, RightChar);
				else	// Japanese Format
					wsprintf(TempStr, TEXT("%c%d/%d/%d %s%c"),	// Make Month & Year String
					LeftChar, WeekBuf[i][0], WeekBuf[i][1], WeekBuf[i][2], ShortDayName[i], RightChar);
			}
			else
			{
				if(dwDateFormat)// US Format?
					wsprintf(TempStr, TEXT("%c%s %d%c"), LeftChar, ShortDayName[i], WeekBuf[i][2], RightChar);
				else
					wsprintf(TempStr, TEXT("%c%d/%d %s%c"), LeftChar, WeekBuf[i][1], WeekBuf[i][2], ShortDayName[i], RightChar);
			}

		// [iwad] 土日カラー対応
			if (i == 0)	// Sunday
			{
				SetTextColor(hDC, RGB(255, 0, 0));
				DrawText(hDC, TempStr, -1, &txRect, DT_CENTER | DT_VCENTER);
				if(dwTextColor)
					SetTextColor(hDC, dwTextColor);
				else
					SetTextColor(hDC, RGB(0, 0, 0));
			}
			else if (i == 6)	// Saturday
			{
				SetTextColor(hDC, RGB(0, 0, 255));
				DrawText(hDC, TempStr, -1, &txRect, DT_CENTER | DT_VCENTER);
				if(dwTextColor)
					SetTextColor(hDC, dwTextColor);
				else
					SetTextColor(hDC, RGB(0, 0, 0));
			}
			else
			{
				DrawText(hDC, TempStr, -1, &txRect, DT_CENTER | DT_VCENTER);
			}	

		#if 0
			DrawText(hDC, TempStr, -1, &txRect, DT_CENTER | DT_VCENTER);
		#endif

			if(dwPrivate && dwWeekFloat && i == dwCurX)
				// [iwad] プライベート反転表示 PatBlt(hDC, txRect.left, 0, WeekBoxWidth * 2, dwMidFontHight + 1, PATINVERT);
				PatBlt(hDC, txRect.left, 4, WeekBoxWidth * 2, dwMidFontHight + 2, PATINVERT);
		}
	}
//	Draw Appointments
	if(Setup)
	{
		for(i = 0; i < 7; i++)	// Repeat for Sun to Sat
		{
DispAgain:
//	Clear Appointment Buffer
			for(j = 0; j < MAXDAYLINE; j++)
			{
				WeekGenIDBuf[i][j] = 0;	// Clear Buffer
				WeekTimeBuf[i][j] = GetTime(dwStartTime, 0);// Set Default Time
				WeekAllDayBuf[i][j] = 0;// Clear All Day Flag
			}
			if(dwWeekFloat)
			{
				if(i >= dwCurX)
				{
					LineLeft = (i + 1) * WeekBoxWidth + 1;	// Left Pos of Line Cursor
				}
				else
				{
					LineLeft = i * WeekBoxWidth + 1;	// Left Pos of Line Cursor
				}
			}
			else
				LineLeft = (i + 1) * WeekBoxWidth + 1;	// Left Pos of Line Cursor
//	Draw Schedule Titles
//			StartTime = ConvHalfHour(DayStartTime[i][DayAllNum[i] + WeekStartIndex[i]]);
			if(dwAppDispMode)	// Hide Blank Mode?
			{
				j = 0;
				l = WeekHideSkip[i];
				for(k = 0; k < DayAllNum[i] + DayOneNum[i]; k++)// Repeat for Day WeekLineCount (Horizontal Line)
				{
					if(DayGenID[i][k] == 0)
						continue;
					if(l)	// Skip Leading Record
					{
						l--;
						continue;
					}
//	Draw Title of the Record
					txRect.left = LineLeft;
					txRect.right = txRect.left + WeekBoxWidth - 1;
					// [iwad] txRect.top = (j + 1) * WeekBoxHight + 1;
					txRect.top = (j + 1) * WeekBoxHight + 6;
					txRect.bottom = txRect.top + dwMidFontHight;
					DrawText(hDC, DayTitle[i][k], -1, &txRect, DT_LEFT | DT_VCENTER | DT_NOPREFIX);
					WeekGenIDBuf[i][j] = DayGenID[i][k];	// Clear Current Line Seek Ptr
					WeekAllDayBuf[i][j] = DayAllDay[i][k];	// Save Original AllDay Flag
					WeekTimeBuf[i][j] = DayStartTime[i][k];
					j++;	// Progress Line
					if(j == WeekLineCount - 1)
						break;
				}
			}
			else
			{	// No Hide Blank Mode (All Display)
//	Draw All Day Schedule
				for(j = 0; j < dwTopWeekLine; j++)
				{
				DWORD	AllIndex = j + WeekAllStartIndex[i];
					txRect.left = LineLeft;
					txRect.right = txRect.left + WeekBoxWidth - 1;
					// [iwad] txRect.top = (j + 1) * WeekBoxHight + 1,
					txRect.top = (j + 1) * WeekBoxHight + 6,
					txRect.bottom = txRect.top + dwMidFontHight;
					if(DayGenID[i][AllIndex] && AllIndex < DayAllNum[i])
					{
						DWORD	BkColorBuf = 0xCCCC66;	// [iwad] 全日表示はカラー背景(色:ナイルブルー)

						_tcscpy(TempStr, TEXT(""));
						if(
						(dwTopWeekLine < DayAllNum[i])
						&&
						(((WeekAllStartIndex[i] + dwTopWeekLine < DayAllNum[i])
						&& (j == dwTopWeekLine - 1))
						||
						((WeekAllStartIndex[i] != 0) && (j == 0)))
						)
							_tcscat(TempStr, TEXT("..."));	// Continue Mark
						_tcscat(TempStr, DayTitle[i][AllIndex]);
						// [iwad] 全日表示はカラー背景
						BkColorBuf = SetBkColor(hDC, BkColorBuf);
						DrawText(hDC, TempStr, -1, &txRect, DT_LEFT | DT_VCENTER | DT_NOPREFIX);
						SetBkColor(hDC, BkColorBuf);
						//DrawText(hDC, TempStr, -1, &txRect, DT_LEFT | DT_VCENTER | DT_NOPREFIX);
						WeekGenIDBuf[i][j] = DayGenID[i][AllIndex];	// Clear Current Line Seek Ptr
						WeekAllDayBuf[i][j] = DayAllDay[i][AllIndex];	// Save Original AllDay Flag
						WeekTimeBuf[i][j] = DayStartTime[i][AllIndex];
					}
				}
				for(j = 0; j < WeekLineCount - dwTopWeekLine - 1; j++)	// Repeat for Day WeekLineCount (Horizontal Line)
				{
// Write Time
					TempIndex = WeekStartIndex[i] + DayAllNum[i] + j;
					if(TempIndex >= MAXDAYBUF)	// Limit Check
						goto ReWrite;	// Clear Screen & ReWrite
					DispHour = DayStartTime[i][TempIndex];
					DispMin = DispHour & 63;
					DispHour /= 64;
					if(DispHour == dwFinalTime + 1)
						break;
//	Draw Title of the Reecord
					txRect.left = LineLeft;
					txRect.right = txRect.left + WeekBoxWidth - 1;
					// [iwad] txRect.top = (j + dwTopWeekLine + 1) * WeekBoxHight + 1;
					txRect.top = (j + dwTopWeekLine + 1) * WeekBoxHight + 6;
					txRect.bottom = txRect.top + dwMidFontHight;
					if(DayGenID[i][TempIndex])
						DrawText(hDC, DayTitle[i][TempIndex], -1, &txRect, DT_LEFT | DT_VCENTER | DT_NOPREFIX);
					WeekGenIDBuf[i][j + dwTopWeekLine] = DayGenID[i][TempIndex];	// Clear Current Line Seek Ptr
					WeekAllDayBuf[i][j + dwTopWeekLine] = DayAllDay[i][TempIndex];	// Save Original AllDay Flag
					WeekTimeBuf[i][j + dwTopWeekLine] = DayStartTime[i][TempIndex];
//	Progress Hour and Minutes
				}
				if(DispHour == dwFinalTime + 1 || (dwDispStep && (DispHour == dwFinalTime && DispMin)))
				{
ReWrite:
					WeekStartIndex[i]--;
					PatBlt(hDC,		// Clear Only Schedule Part
						LineLeft,
						// [iwad] WeekBoxHight + 1,
						WeekBoxHight + 6,
						WeekBoxWidth - 1,
						WeekBoxHight * (WeekLineCount - 1), PATCOPY);
						dwOldWeekCurX = 8;
					goto DispAgain;
				}
			}
		}
	}
//	Draw Time Line
	APMode = 0;
	if(DayStartTime[dwCurX][DayAllNum[dwCurX] + WeekStartIndex[dwCurX]] >= GetTime(13, 0) && dwAppDispMode == 0)
		APMode = 2;			// PM Char
//	Clear Time Line
	// [iwad] PatBlt(hDC, TimeLeft, WeekBoxHight + 1, WeekBoxWidth - 1, WeekBoxHight * (WeekLineCount - 1), PATCOPY);
	PatBlt(hDC, TimeLeft, WeekBoxHight + 6, WeekBoxWidth - 1, WeekBoxHight * (WeekLineCount - 1), PATCOPY);// Clear Memory Device
	if(dwAppDispMode)	// Hide Mode?
	{
		i = 0;
		k = WeekHideSkip[dwCurX];
		for(j = 0; j < DayAllNum[dwCurX] + DayOneNum[dwCurX]; j++)// Repeat for Day WeekLineCount (Horizontal Line)
		{
			if(DayGenID[dwCurX][j] == 0)
				continue;
			if(k)	// Skip Leading Record
			{
				k--;
				continue;
			}
			DispHour = DayStartTime[dwCurX][j];
			DispMin = DispHour & 63;
			DispHour /= 64;
			if(DayAllDay[dwCurX][j])
			{

				if(dwHourMode)	// 24H Mode?
				{
					wsprintf(TempStr, TEXT("%2d:%2.2d"), DispHour, DispMin);
				}
				else
				{
					if(DispHour > 24)
						wsprintf(TempStr, TEXT("%2d:%2.2d"), DispHour - 24, DispMin);
					else if(DispHour > 12)
						wsprintf(TempStr, TEXT("%2d:%2.2d"), DispHour - 12, DispMin);
					else
						wsprintf(TempStr, TEXT("%2d:%2.2d"), DispHour, DispMin);
				}
				// [iwad] txRect.top = (i + 1) * WeekBoxHight + 1;
				txRect.top = (i + 1) * WeekBoxHight + 6;
				txRect.bottom = txRect.top + dwMidFontHight;
				if(dwWeekFloat)
					txRect.left = dwCurX * WeekBoxWidth;
				else
					txRect.left = 0;
				txRect.right = txRect.left + WeekBoxWidth - dwMidFontWidth * 2 - 1;
				DrawText(hDC, TempStr, -1, &txRect, DT_RIGHT | DT_VCENTER);
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
					txRect.right += dwMidFontWidth;
					DrawText(hDC, &APMark[APMode], 1, &txRect, DT_LEFT | DT_VCENTER);
					if(DayAllDay[dwCurX][j] && (APMode == 0 || APMode == 2 || APMode == 4))
						APMode++;
				}
			}
			i++;
			if(i == WeekLineCount - 1)
				break;
		}
	}
	else
	{
		for(i = 0; i < WeekLineCount - dwTopWeekLine - 1; i++)	// Repeat for Day WeekLineCount (Horizontal Line)
		{
			TempIndex = WeekStartIndex[dwCurX] + DayAllNum[dwCurX] + i;
			if(TempIndex >= MAXDAYBUF)	// Limit Check
				goto ReWrite;	// Clear Screen & ReWrite
			DispHour = DayStartTime[dwCurX][TempIndex];
			DispMin = DispHour & 63;
			DispHour /= 64;
			if(DispHour == dwFinalTime + 1)
				break;
			if(DayAllDay[dwCurX][TempIndex] == 0)
				continue;
			if(dwHourMode)	// 24H Mode?
			{
				wsprintf(TempStr, TEXT("%2d:%2.2d"), DispHour, DispMin);
			}
			else
			{
				if(DispHour > 24)
					wsprintf(TempStr, TEXT("%2d:%2.2d"), DispHour - 24, DispMin);
				else if(DispHour > 12)
					wsprintf(TempStr, TEXT("%2d:%2.2d"), DispHour - 12, DispMin);
				else
					wsprintf(TempStr, TEXT("%2d:%2.2d"), DispHour, DispMin);
			}
			// [iwad] txRect.top = (i + dwTopWeekLine + 1) * WeekBoxHight + 1;
			txRect.top = (i + dwTopWeekLine + 1) * WeekBoxHight + 6;
			txRect.bottom = txRect.top + dwMidFontHight;
			if(dwWeekFloat)
				txRect.left = dwCurX * WeekBoxWidth;
			else
				txRect.left = 0;
			txRect.right = txRect.left + WeekBoxWidth - dwMidFontWidth * 2 - 1;
			DrawText(hDC, TempStr, -1, &txRect, DT_RIGHT | DT_VCENTER);
//	Draw Blacket
			if(DayBlacket[dwCurX][TempIndex])
			{
				for(j = 0; j < 4; j++)
				{
					BarLeft = TimeLeft + WeekBoxWidth / 5 - j * BarStep;// Blacket Right Pos
					switch((DayBlacket[dwCurX][TempIndex] >> (j * 3)) & 7)
					{
					case 0:	// Blank
						continue;	// No Line
					case 1:	// -
						LinePoint[0].x = BarLeft;// Blacket Left
						LinePoint[1].x = LinePoint[0].x - BarStep / 2;	// Frame Left Top Axis
						LinePoint[0].y = LinePoint[1].y =
							(i + dwTopWeekLine + 1) * WeekBoxHight + WeekBoxHight / 2;
						Polyline(hDC, LinePoint, 2);	// Draw Todays Line
						break;
					case 2:	// |
						LinePoint[0].x = BarLeft - BarStep / 2;// Blacket Left
						LinePoint[1].x = LinePoint[0].x;	// Frame Left Top Axis
						LinePoint[0].y = (i + dwTopWeekLine + 1) * WeekBoxHight;
						LinePoint[1].y = LinePoint[0].y + WeekBoxHight;
						Polyline(hDC, LinePoint, 2);	// Draw Todays Line
						break;
					case 3:	// Start Blacket
						LinePoint[0].x = BarLeft;// Blacket Left
						LinePoint[1].x = LinePoint[0].x - BarStep / 2;// Frame Left Top Axis
						LinePoint[0].y = LinePoint[1].y =
							(i + dwTopWeekLine + 1) * WeekBoxHight + WeekBoxHight / 2;
						LinePoint[2].x = LinePoint[1].x;
						LinePoint[2].y = (i + dwTopWeekLine + 2) * WeekBoxHight;
						Polyline(hDC, LinePoint, 3);	// Draw Todays Line
						break;
					case 4:	// Start Blacket
						LinePoint[0].x = BarLeft;// Blacket Left
						LinePoint[1].x = LinePoint[0].x - BarStep / 2;	// Frame Left Top Axis
						LinePoint[0].y = LinePoint[1].y =
							(i + dwTopWeekLine + 1) * WeekBoxHight + WeekBoxHight / 2;
						LinePoint[2].x = LinePoint[1].x;
						LinePoint[2].y = (i + dwTopWeekLine + 1) * WeekBoxHight - 1;
						Polyline(hDC, LinePoint, 3);	// Draw Todays Line
						break;
					}
				}
			}
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
				txRect.right += dwMidFontWidth * 2;
				DrawText(hDC, &APMark[APMode], 1, &txRect, DT_LEFT | DT_VCENTER);
				if(DayAllDay[dwCurX][TempIndex]
					&& (APMode == 0 || APMode == 2 || APMode == 4))
					APMode++;
			}
		}
	}
	if(SetCursor)
	{
		if(CurrentTime == MAXDWORD)	// Current is All Day ?
			WeekCurY = 0;	// Set Cursor Top
		else
		{
			if(dwAppDispMode)
			{	// Hide Blank Mode Scan Pos
				for(i = 0; i < WeekLineCount - 2; i++)
					if(WeekGenIDBuf[dwCurX][i] == 0)
						break;
					else if(WeekAllDayBuf[dwCurX][i] == 0)
						continue;
					else if(ConvHalfHour(WeekTimeBuf[dwCurX][i]) >= CurrentTime)
						break;	// Get Display Start Time Found
			}
			else
			{	// Show All Mode Scan Pos
				for(i = dwTopWeekLine; i < WeekLineCount - 2; i++)
					if(ConvHalfHour(WeekTimeBuf[dwCurX][i]) >= CurrentTime)
						break;	// Get Display Start Time Found
			}
			WeekCurY = i;
		}
	}
	if(dwAppDispMode && WeekCurY > ActualNum[dwCurX])	// If Hide Mode & Cursor Over?
		WeekCurY = ActualNum[dwCurX];// Set Cursor Bottom
	DispWeekBox(hDC);
}
//
//	Setup Week
//
void SetupWeek(void)
{
DWORD	i, j;
DWORD	Today;
	for(i = 0; i < 7; i++)
		for(j = 0; j < 3; j++)
			WeekBuf[i][j] = MonthBuf[0][i + dwCurY * 7][j];
//	Setup Week Array
	for(i = 0; i < 7; i++)
	{
		for(j = 0; j < MAXDAYBUF; j++)
			DayGenID[i][j] = 0;	// Clear Buffer
		Today = GetDate(WeekBuf[i][0], WeekBuf[i][1], WeekBuf[i][2]);
		SetupOneDayArray(Today, i, FALSE, 1);	// No Place String Set
	}
}
//
//	Invert Week Cursor
//
void ReverseWeekBox(HDC hDC,DWORD x, DWORD y)
{
	PatBlt(hDC,
			(x + 1) * WeekBoxWidth + 1,
			// [iwad] (y + 1) * WeekBoxHight + 1,
			(y + 1) * WeekBoxHight + 6,
			WeekBoxWidth - 1,
			WeekBoxHight - 1, PATINVERT);
}
//
//	Disp Week Cursor
//
void DispWeekBox(HDC hDC)
{
	if(dwOldWeekCurX != 8)
		ReverseWeekBox(hDC, dwOldWeekCurX, dwOldWeekCurY);
	ReverseWeekBox(hDC, dwCurX, WeekCurY);
	dwOldWeekCurX = dwCurX;
	dwOldWeekCurY = WeekCurY;
	SetupMenu(FALSE);
	if(dwAppDispMode)
	{
		if(WeekGenIDBuf[dwCurX][WeekCurY] == 0)	// End of Line?
			CurrentTime = ConvHalfHour(GetTime(31, 0));
		else
		{
			if(WeekAllDayBuf[dwCurX][WeekCurY] == 0)	// All Day Record?
				CurrentTime = MAXDWORD;
			else
				CurrentTime = ConvHalfHour(WeekTimeBuf[dwCurX][WeekCurY]);
		}
	}
	else
	{
		if(WeekGenIDBuf[dwCurX][WeekCurY] && WeekAllDayBuf[dwCurX][WeekCurY] == 0)// Points All Day Record?
			CurrentTime = MAXDWORD;
		else
			CurrentTime = ConvHalfHour(WeekTimeBuf[dwCurX][WeekCurY]);
	}
}
//
//	Week Key Proc
//
void WeekKeyProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
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
	case TEXT('\r'):	// Change to Day Screen
		if(FileLock == 0)
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_INPUT, 0);
		break;
	case TEXT('\033'):	// ESC Key
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
		if(WeekGenIDBuf[dwCurX][WeekCurY] && SetupClipData(FALSE))	// Alloc Succeeds ?
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
		if(dwConfirm && MessageBox(hWnd, TEXT("ﾊｲﾗｲﾄされたｱｲﾃﾑを削除しますか?"), TEXT("Delete"),MB_OKCANCEL) != IDOK)	// by uminchu
			break;
		OpenClipboard(hWnd);
		EmptyClipboard();
		ClipBuf = 0;
		if(WeekGenIDBuf[dwCurX][WeekCurY] == 0)
			goto NoClipSet;	// No Copy Action
		if(SetupClipData(FALSE))	// Alloc Succeeds ?
			SetClipboardData(Format, ClipBuf);
		dwDlgDate = GetCurrentDate();	// Set Date
		dwDlgStartTime = WeekTimeBuf[dwCurX][WeekCurY];// Set Time
		dwGenID = WeekGenIDBuf[dwCurX][WeekCurY];	// Set Record ID
		dwAllDay = WeekAllDayBuf[dwCurX][WeekCurY];// Set All Day Flag
		OyajiSeekGetRecord(&MainWork);// Get Record Data
		DeleteMode = TRUE;
		if(dwMultiConfirm && (lpDlgRepeat[0] || dwDlgConsDay != 1))
		{
//			DlgResult = MessageBox(hWnd, TEXT("Delete All Item?"), TEXT("Repeat Delete"),MB_YESNOCANCEL);
			DlgResult = MessageBox(hWnd, TEXT("全てのｱｲﾃﾑを削除しますか?"), TEXT("Repeat Delete"),MB_YESNOCANCEL);	// by uminchu
			if(DlgResult == IDCANCEL)
				break;
			else if(DlgResult == IDNO)
				DeleteMode = FALSE;
		}
		if(DeleteMode)
			dwDlgDate = dwOrgDate;	// Set Top Date
		DelRecord(DeleteMode);	// BOOL but ignore
//	Set Pending GenID Here (To Erase)
		WeekGenIDBuf[dwCurX][WeekCurY] = 0;// Clear Line Seek Ptr
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
		if(RestoreClipData(hWnd, FALSE) == 0)	// Alloc Succeeds ?
			break;
		dwDlgDate = GetCurrentDate();	// Set Date
		dwOrgDate =dwDlgDate;	// Begin from Target Day
		Duration = SubTime(dwDlgEndTime, dwDlgStartTime);// Get Length (Time Format)
		dwDlgStartTime = WeekTimeBuf[dwCurX][WeekCurY];// Set Start Time
		dwDlgEndTime = AddTime(dwDlgStartTime, Duration);// Set End Time
		if(dwDlgEndTime > GetTime(31, 0))	//	31:00 ?
			dwDlgEndTime = GetTime(31, 0);	// Force Set 31:00
		dwGenID = 0;	// Clear Gen ID (New Mode)
		dwDlgMode = 0;	// New Add Mode
		dwDlgPrivate = dwPrivate;	// Force Set Private Mode
		AddRecord(TRUE, TRUE);// All Change Mode & Ret Val ignore
//	Set Pending GenID Here (To Erase)
		SetupMainScreen(hMemDC);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case TEXT('\032'):	// Ctrl+Z?
		OyajiUndoFile(&MainWork);
		SetupMainScreen(hMemDC);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	default:
		if(FileLock == 0 && wParam >= TEXT(' '))
		{
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_INPUT, 0);
			SendMessage(GetFocus(), WM_CHAR, wParam, lParam);
		}
	}
}
//
//	Week Key Down Proc
//
void WeekKeyDownProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
DWORD	LastYear, LastMonth, LastDay, i;
	LastYear = GetCurrentYear(0);
	LastMonth = GetCurrentMonth(0);
	LastDay = GetCurrentDay(0);
	if(wParam == VK_UP)
	{
		if(GetKeyState(VK_SHIFT) < 0)	// Shift Up
			goto PrevMonth;
		if(dwAppDispMode)
		{
			if(WeekCurY == 0)// Top Line?
			{
				if(WeekHideSkip[dwCurX])
				{
					WeekHideSkip[dwCurX]--;
					goto ScrollDownEnd;
				}
			}
			if(WeekCurY)
				WeekCurY--;
			goto CursorDownEnd;
		}
		else
		{
			if(WeekCurY == dwTopWeekLine)// Not Hide Mode?
			{
				if(WeekStartIndex[dwCurX])
				{
					WeekStartIndex[dwCurX]--;
					goto ScrollDownEnd;
				}
			}
			if(WeekCurY)
				WeekCurY--;
			else if(WeekAllStartIndex[dwCurX])
			{
				WeekAllStartIndex[dwCurX]--;
					goto ScrollDownEnd;
			}
			goto CursorDownEnd;
		}
	}
	else if(wParam == VK_DOWN)
	{
		if(GetKeyState(VK_SHIFT) < 0)
			goto NextMonth;
		if(dwAppDispMode)
		{
			if(WeekCurY == WeekLineCount - 2)	// Bottom Line?
			{	// Scroll Down
				if(WeekLineCount + WeekHideSkip[dwCurX] < ActualNum[dwCurX] + 2)
				{
					WeekHideSkip[dwCurX]++;
					goto ScrollDownEnd;
				}
			}	// Just Cursor Down
			if(WeekCurY < WeekLineCount - 2 && WeekCurY < ActualNum[dwCurX] - WeekHideSkip[dwCurX])
				WeekCurY++;
			goto CursorDownEnd;
		}
		else
		{
//	For All Day Scroll
			if((DayAllNum[dwCurX] > dwTopWeekLine)
			&& (WeekCurY == dwTopWeekLine - 1)
			&& (WeekAllStartIndex[dwCurX] < DayAllNum[dwCurX] - dwTopWeekLine))
			{
				WeekAllStartIndex[dwCurX]++;
				goto ScrollDownEnd;
			}
//	End of All Day Scroll
			if(WeekCurY == WeekLineCount - 2)
			{
				if(DispHour < dwFinalTime)
				{
					WeekStartIndex[dwCurX]++;
ScrollDownEnd:
					WeekScreen(hDC, 1, FALSE);// Scroll Only
					InvalidateRect(hWnd, NULL, FALSE);
					return;
				}
			}
			if(WeekCurY < WeekLineCount - 2)
				WeekCurY++;
CursorDownEnd:
			DispWeekBox(hDC);
			InvalidateRect(hWnd, NULL, FALSE);
			return;
		}
	}
	else if(wParam == VK_LEFT)	// Cursor LEFT
	{
		if(dwCurX == 0)
		{
			dwCurX = 6;
PrevMonth:
			if(dwCurY == 0)
			{
				if(dwYear == 1951 && dwMonth == 1)
					goto ExitWeek;
				LastMonth = GetCurrentMonth(0);
				LastDay = GetCurrentDay(0);
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
			goto PageChangeEnd;
		}
		else
			dwCurX--;
		goto CursorRightEnd;
	}
	else if(wParam == VK_RIGHT)	// Cursor RIGHT
	{
		if(dwCurX == 6)
		{
			dwCurX = 0;
NextMonth:
			if(dwCurY == 4)
			{
				if(dwYear == 2077 && dwMonth == 12)
					goto ExitWeek;
				LastMonth = GetCurrentMonth(0);
				LastDay = GetCurrentDay(0);
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
PageChangeEnd:
			JumpCurrentDay(0);
ExitWeek:
			WeekScreen(hDC, 2, FALSE);// Setup All Data
			InvalidateRect(hWnd, NULL, FALSE);
			return;
		}
		else
			dwCurX++;
CursorRightEnd:
		WeekScreen(hDC, dwWeekFloat, FALSE);// Disp Time Only & Not Set Cursor
		InvalidateRect(hWnd, NULL, FALSE);
		return;
	}
	else if(wParam == VK_PRIOR)
	{
		if(dwAppDispMode)	// If Hide Mode
			goto PrevMonth;
		if(WeekStartIndex[dwCurX] == 0)
			goto Home;
		if(WeekStartIndex[dwCurX] > 8)
			WeekStartIndex[dwCurX] -= 8;
		else
			WeekStartIndex[dwCurX] = 0;
		goto ScrollDownEnd;
	}
	else if(wParam == VK_NEXT)
	{
		if(dwAppDispMode)	// If Hide Mode
			goto NextMonth;
		for(i = 0; i < 8; i++)
		{
		DWORD	OneIndex = WeekStartIndex[dwCurX] + WeekLineCount - dwTopWeekLine - 2;
//			if(WeekStartIndex[dwCurX] + WeekLineCount - 2 < MAXDAYBUF
//			&& DayStartTime[dwCurX][WeekStartIndex[dwCurX] + WeekLineCount - 2] < GetTime(dwFinalTime, 0))
			if(OneIndex < MAXDAYBUF
			&& DayStartTime[dwCurX][OneIndex] < GetTime(dwFinalTime, 0))
				WeekStartIndex[dwCurX]++;
			else
				break;
		}
		goto ScrollDownEnd;
	}
	else if(wParam == VK_HOME)
	{
Home:
		WeekCurY = 0;
		goto CursorDownEnd;
	}
	else if(wParam == VK_END && dwAppDispMode == 0)
	{
		WeekCurY = WeekLineCount - 2;
		goto CursorDownEnd;
	}
}
//
//	Week Left Button Proc
//
void WeekLeftProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
DWORD	Top = WeekBoxHight + dwWinTop + 1;
DWORD	LastX;
	LastX = dwCurX;	// Save Original
	if(LOWORD(lParam) >= 8 * WeekBoxWidth)	// Right Over
		return;
	if(HIWORD(lParam) <= Top)
	{
		SendMessage(hWnd, WM_COMMAND, IDM_JUMP_DATE, 0);
		return;
	}
	dwCurX = LOWORD(lParam) / WeekBoxWidth;
	if(dwWeekFloat == 0)
	{
		if(dwCurX == 0)
			return;
		dwCurX--;
	}
	else
	{
		if(dwCurX > LastX)
			dwCurX--;
	}
	WeekCurY = (HIWORD(lParam) - Top) / WeekBoxHight;
	if(WeekCurY > WeekLineCount - 2)
		WeekCurY = WeekLineCount - 2;
	if(dwAppDispMode && WeekCurY > ActualNum[dwCurX] - WeekHideSkip[dwCurX])
		WeekCurY = ActualNum[dwCurX] - WeekHideSkip[dwCurX];
	if(dwCurX != LastX)
		WeekScreen(hDC, 1, FALSE);// Scroll Only & Not Set Cursor
	else
		DispWeekBox(hDC);
	InvalidateRect(hWnd, NULL, FALSE);
}
//
//	Week Double Click Proc
//
void WeekDblProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
DWORD	Top = WeekBoxHight + dwWinTop + 1;
DWORD	LastX;
	LastX = dwCurX;	// Save Original
	if(HIWORD(lParam) <= Top)
		return;
	if(LOWORD(lParam) >= 8 * WeekBoxWidth)	// Right Over
		return;
	dwCurX = LOWORD(lParam) / WeekBoxWidth;
	if(dwWeekFloat == 0)
	{
		if(dwCurX == 0)
			return;
		dwCurX--;
	}
	else
	{
		if(dwCurX > LastX)
			dwCurX--;
	}
	WeekCurY = (HIWORD(lParam) - Top) / WeekBoxHight;
	if(WeekCurY > WeekLineCount - 2)
		WeekCurY = WeekLineCount - 2;
	if(dwAppDispMode && WeekCurY > ActualNum[dwCurX] - WeekHideSkip[dwCurX])
		WeekCurY = ActualNum[dwCurX] - WeekHideSkip[dwCurX];
	if(dwCurX != LastX)
		WeekScreen(hDC, 1, FALSE);// Scroll Only & Not Set Cursor
	else
		DispWeekBox(hDC);
	InvalidateRect(hWnd, NULL, FALSE);
	SendMessage(hWnd, WM_CHAR, TEXT('\r'), NULL);
}
