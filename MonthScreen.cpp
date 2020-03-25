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
BOOL	CALLBACK OyajiDateEditProc(HWND, UINT, WPARAM, LPARAM);
void	SetupMonth(DWORD, DWORD);
static void SetupMonthSub(DWORD, DWORD, DWORD);
void	JumpDay(DWORD, DWORD, DWORD);
void	JumpDate(DWORD);
void	JumpCurrentDay(DWORD);
void	DispReverseBox(HDC);
void	ReverseBox(HDC, DWORD, DWORD);
void	HalfScreen(HDC, BOOL, BOOL, BOOL);
void	MonthScreen(HDC);
void	DayScreen(HDC, BOOL, BOOL, BOOL);
void	WeekScreen(HDC, BOOL, BOOL);
void	SetupInputScreen(HDC);
void	SetupHolydayBrush(HDC, BOOL);
void	HalfKeyDownProc(HWND, HDC, WPARAM, LPARAM);
void	HalfLeftProc(HWND, HDC, WPARAM, LPARAM);
void	HalfDblProc(HWND, HDC, WPARAM, LPARAM);
void	MonthKeyProc(HWND, HDC, WPARAM, LPARAM);
void	MonthKeyDownProc(HWND, HDC, WPARAM, LPARAM);
void	MonthLeftProc(HWND, HDC, WPARAM, LPARAM);
void	MonthDblProc(HWND, HDC, WPARAM, LPARAM);
void	DayKeyProc(HWND, HDC, WPARAM, LPARAM);
void	DayKeyDownProc(HWND, HDC, WPARAM, LPARAM);
void	DayLeftProc(HWND, HDC, WPARAM, LPARAM);
void	DayRightProc(HWND, HDC, WPARAM, LPARAM);
void	DayDblProc(HWND, HDC, WPARAM, LPARAM);
void	WeekKeyProc(HWND, HDC, WPARAM, LPARAM);
void	WeekKeyDownProc(HWND, HDC, WPARAM, LPARAM);
void	WeekLeftProc(HWND, HDC, WPARAM, LPARAM);
void	WeekDblProc(HWND, HDC, WPARAM, LPARAM);
void	ToDoKeyProc(HWND, HDC, WPARAM, LPARAM);
void	ToDoKeyDownProc(HWND, HDC, WPARAM, LPARAM);
void	ToDoLeftProc(HWND, HDC, WPARAM, LPARAM);
void	ToDoDblProc(HWND, HDC, WPARAM, LPARAM);
void	InputKeyProc(HWND, HDC, WPARAM, LPARAM);
void	MainLButtonProc(HWND, HDC, WPARAM, LPARAM);
void	MainRButtonProc(HWND, HDC, WPARAM, LPARAM);
void	MainDblClickProc(HWND, HDC, WPARAM, LPARAM);
void	GetToday(void);
void	GetUniCode(char *, LPTSTR);
void	SetupBrush(HDC);
BOOL	RestoreClipData(HWND, BOOL);
BOOL	AddRecord(BOOL, BOOL);
DWORD	GetCurrentYear(DWORD);
DWORD	GetCurrentMonth(DWORD);
DWORD	GetCurrentDay(DWORD);
DWORD	GetCurrentDate(void);
DWORD	GetDate(DWORD, DWORD, DWORD);
DWORD	GetTime(DWORD, DWORD);
DWORD	OyajiSearchRecord(FILEWORKS *);
LPCTSTR GetDateString(DWORD);
DWORD	GetDate(DWORD, DWORD, DWORD);
BOOL	OyajiGetRecord(FILEWORKS *);
BOOL	OyajiUndoFile(FILEWORKS *);
BOOL	CALLBACK JumpDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL	ConvDateToDword(LPDWORD, LPTSTR);
void	ToDoScreen(HDC, BOOL);
// 
//
//	Global Vars
//
DWORD		dwDayOffset;
DWORD		dwWeekWidth = 7;	// Week Width
DWORD		BoxHight, BoxWidth;	// Size of Day Box
DWORD		DayHight;	// Height of Monday to Sunday
static	DWORD LastDay[12] = {
				31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static	DWORD	NormalOffset[12] = {
				0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
static	DWORD	SpecialOffset[12] = {
				0, 3, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6};
static	DWORD	MonthGrayBuf[5][7];
//
//	Setup Main Screen to Memory Device
//
void SetupMainScreen(HDC hDC)
{
	if(MainWork.FileHandle == NULL)
	{
		SetupHolydayBrush(hDC, FALSE);	// Set Normal Day Brush
		PatBlt(hDC, 0, 0, dwXsize, dwYsize, PATCOPY);	// Clear Memory Device
			return;
	}
	switch(DispMode)
	{
	case 0:	// Month Mode
		HalfScreen(hDC, TRUE, FALSE, TRUE);
		break;
	case 1:	// Month Mode
		MonthScreen(hDC);
		break;
	case 2:	// Week Mode
		WeekScreen(hDC, 2, TRUE);// 2 = Setup All Buffer & TRUE = Set Cursor Y
		break;
	case 3:	// Day Mode
		DayScreen(hDC, TRUE, FALSE, TRUE);// Redraw Calender & No Scroll Mode & Set Cursor Y
		break;
	case 4:	// Input Mode
		SetupInputScreen(hDC);	// 
		break;
	case 5:	// ToDo Mode
		ToDoScreen(hDC, TRUE);	//
		break;
	}
}

//
//	Main Window Char Proc
//
void MainKeyProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
	if(MainWork.FileHandle == NULL)
		return;
	switch(DispMode)
	{
	case 1:	// Month Mode
		MonthKeyProc(hWnd, hDC, wParam, lParam);
		break;
	case 2:	// Week Mode
		WeekKeyProc(hWnd, hDC, wParam, lParam);
		break;
	case 0:	// Half Year Mode
	case 3:
		DayKeyProc(hWnd, hDC, wParam, lParam);
		break;
	case 4:
		InputKeyProc(hWnd, hDC, wParam, lParam);
		break;
	case 5:
		ToDoKeyProc(hWnd, hDC, wParam, lParam);
		break;
	}
}

//
//	Main Window Key Down Proc
//
void MainKeyDownProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
	if(MainWork.FileHandle == NULL)
		return;
	switch(DispMode)
	{
	case 0:	// Month Mode
		HalfKeyDownProc(hWnd, hDC, wParam, lParam);
		break;
	case 1:	// Month Mode
		MonthKeyDownProc(hWnd, hDC, wParam, lParam);
		break;
	case 2:	// Week Mode
		WeekKeyDownProc(hWnd, hDC, wParam, lParam);
		break;
	case 3:
		DayKeyDownProc(hWnd, hDC, wParam, lParam);
		break;
	case 5:
		ToDoKeyDownProc(hWnd, hDC, wParam, lParam);
		break;
	}
}

//
//	Mouse Left Button Proc
//
void MainLButtonProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
	if(MainWork.FileHandle == NULL)
		return;
	switch(DispMode)
	{
	case 0:	// Month Mode
		HalfLeftProc(hWnd, hDC, wParam, lParam);
		break;
	case 1:	// Month Mode
		MonthLeftProc(hWnd, hDC, wParam, lParam);
		break;
	case 2:	// Week Mode
		WeekLeftProc(hWnd, hDC, wParam, lParam);
		break;
	case 3:
		DayLeftProc(hWnd, hDC, wParam, lParam);
		break;
	case 5:
		ToDoLeftProc(hWnd, hDC, wParam, lParam);
		break;
	}
}

//
//	Mouse Right Click Proc [iwad]
//
void MainRButtonProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
	if(MainWork.FileHandle == NULL)
		return;
	switch(DispMode)
	{
	case 0:	// Month Mode
		//HalfDblProc(hWnd, hDC, wParam, lParam);
		break;
	case 1:	// Month Mode
		//MonthDblProc(hWnd, hDC, wParam, lParam);
		break;
	case 2:	// Week Mode
		//WeekDblProc(hWnd, hDC, wParam, lParam);
		break;
	case 3:
		DayRightProc(hWnd, hDC, wParam, lParam);
		break;
	case 5:
		//ToDoDblProc(hWnd, hDC, wParam, lParam);
		break;
	}
}

//
//	Mouse Double Click Proc
//
void MainDblClickProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
	if(MainWork.FileHandle == NULL)
		return;
	switch(DispMode)
	{
	case 0:	// Month Mode
		HalfDblProc(hWnd, hDC, wParam, lParam);
		break;
	case 1:	// Month Mode
		MonthDblProc(hWnd, hDC, wParam, lParam);
		break;
	case 2:	// Week Mode
		WeekDblProc(hWnd, hDC, wParam, lParam);
		break;
	case 3:
		DayDblProc(hWnd, hDC, wParam, lParam);
		break;
	case 5:
		ToDoDblProc(hWnd, hDC, wParam, lParam);
		break;
	}
}

//
//	Setup Month Display Screen to Memory Device
//
void MonthScreen(HDC hDC)
{
POINT	LinePoint[5];	// Arry for DrawLine
RECT	txRect;			// Rect for DrawText
TCHAR	TempStr[64], *Mark;	// Day Number String Buf
DWORD	Today;			// Today's Date Copy
DWORD	i, j, k, Bit = 1, Lines;	// Temp Counter
DWORD	DispYear, DispMonth;
BOOL	GrayMode;
BOOL	GrayDisp;
TCHAR	LineText[16][64 + 4];// Schedule Text Buffer
DWORD	LTGRAYBUF;				// [iwad] 土日カラー対応 バッファ
DWORD	dwGrayBUF;				// [iwad] 土日カラー対応 バッファ
#define	LTGRAY_SAT	0xFFCCCC	// [iwad] 土日カラー対応 土曜色
#define	LTGRAY_SUN	0xBBBBFF	// [iwad] 土日カラー対応 日曜色

	SetupBrush(hDC);
	PatBlt(hDC, 0, 0, dwXsize, dwYsize, PATCOPY); // Clear Memory Device
	SelectObject(hDC, hSmallFont);		// Set Font
	StartTime = dwStartTime * 2;		// Set Day Screen Start Time
	CurrentTime = StartTime;			// Current Select Time
	WeekCurY = dwTopWeekLine;			// Set Week Screen Cursor Y
	GetToday();
	BoxHight = (dwYsize - dwSmallFontHight * 2) / 5;	// Get DayBox Height
	BoxWidth = (dwXsize / dwWeekWidth);	// Get DayBox Width
	DayHight = dwYsize - BoxHight * 5;	// Get Height of Day	
	Lines = BoxHight / dwMidFontHight;	// Get Lines in the Box
	if(Lines > 16)
		Lines = 16;	// Max Disp Line = 16
	LinePoint[0].y = dwSmallFontHight;	// 1st Line
	LinePoint[1].y = dwYsize - 1;	// 1st Line
	DispYear = MonthBuf[0][15][0];	// Get Center Year
	DispMonth = MonthBuf[0][15][1];	// Get Center Month
	// [iwad] txRect.top = 0;
	txRect.top = 2;
	// [iwad] txRect.bottom = dwSmallFontHight;
	txRect.bottom = dwSmallFontHight + 2;
	txRect.left = 0;
	txRect.right = BoxWidth * 7 + 1;
	wsprintf(TempStr, TEXT("%s %d (%d / %d)"), 
		MonthName[DispMonth - 1], DispYear, DispYear, DispMonth);
	DrawText(hDC, TempStr, -1, &txRect, DT_CENTER | DT_VCENTER);
	if(dwPrivate)
		PatBlt(hDC, 0, 0, BoxWidth * 7 + 1, dwSmallFontHight, PATINVERT);
	txRect.top = dwSmallFontHight;
	txRect.bottom = DayHight;
	for(i = 0; i < dwWeekWidth + 1; i++)	// Repeat for Row (Vertical line)
	{
		LinePoint[0].x = i * BoxWidth;
		LinePoint[1].x = LinePoint[0].x;
		Polyline(hDC, LinePoint, 2);	// Draw
		if(i == dwWeekWidth)	// Last Vertical Line?
			continue;
		txRect.left = i * BoxWidth + 1;
		txRect.right = (i + 1) * BoxWidth;

	// [iwad] 土日カラー対応
		if (i == 0)	// Sunday
		{
			SetTextColor(hDC, RGB(255, 0, 0));
			DrawText(hDC, DayName[i], -1, &txRect, DT_CENTER | DT_VCENTER);
			if(dwTextColor)
				SetTextColor(hDC, dwTextColor);
			else
				SetTextColor(hDC, RGB(0, 0, 0));
		}
		else if (i == 6)	// Saturday
		{
			SetTextColor(hDC, RGB(0, 0, 255));
			DrawText(hDC, DayName[i], -1, &txRect, DT_CENTER | DT_VCENTER);
			if(dwTextColor)
				SetTextColor(hDC, dwTextColor);
			else
				SetTextColor(hDC, RGB(0, 0, 0));
		}
		else
		{
			DrawText(hDC, DayName[i], -1, &txRect, DT_CENTER | DT_VCENTER);
		}	

	#if 0
		DrawText(hDC, DayName[i], -1, &txRect, DT_CENTER | DT_VCENTER);
	#endif

	}
	LinePoint[0].x = 0;
	LinePoint[1].x = BoxWidth * dwWeekWidth;
	for(i = 0; i < 6; i++)	// Repeat for 5 colums (Horizontal Line)
	{
		if(i == 5)
			LinePoint[1].x++;	// For Right Bottom 1 Dot
		LinePoint[0].y = i * BoxHight + DayHight - 1;	// 1st Line
		LinePoint[1].y = LinePoint[0].y;	// 1st Line
		Polyline(hDC, LinePoint, 2);	// Draw
	}
	LinePoint[0].y = dwSmallFontHight;	// 1st Line
	LinePoint[1].y = LinePoint[0].y;	// 1st Line
	Polyline(hDC, LinePoint, 2);	// Draw Top Line
	for(i = 0; i < 35; i++)	// Repeat
	{
		if(i % 7 == 0)
			Bit = 1;	// SunDay
		else
			Bit *= 2;
		GrayDisp = (((dwPrivate)?dwPHolyDay: dwHolyDay) & Bit);
//	Clear Gray Mode
		GrayMode = FALSE;
//	Disp Monthly Schedule
		Today = GetDate(MonthBuf[0][i][0], MonthBuf[0][i][1], MonthBuf[0][i][2]);
// Test the Day have Record
		dwDlgDate = Today;
		dwDlgStartTime = 0;
		dwAllDay = 0;	// All Day Schedule First
		dwGenID = 0;	// All Generations
		k = 0;
		if(OyajiSearchRecord(&MainWork))// Record in the Day?
		{
			for(j = 0; j < Lines; j++)
			{
ReadAgain:
				if(OyajiGetRecord(&MainWork) == FALSE)
					break;
				if(dwDlgDate != Today)
					break;
				if(dwGenID == HideGenID)
					goto ReadAgain;
				if(dwDlgPrivate != dwPrivate)
					goto ReadAgain;
				if(dwDlgGrayView)
					GrayMode = TRUE;	// Mark Gray
				if(dwDlgMonthView == 0)
					goto ReadAgain;
				GetUniCode(lpDlgTitle, TitleBuf);
				Mark = TEXT(" ");
				if(dwAllDay == 0)
					Mark = TEXT("x");
				else if(dwDlgStartTime >= GetTime(12, 0))
					Mark = TEXT("+");
				if(j == Lines - 1)
				{
					while(TRUE)
					{
						if(OyajiGetRecord(&MainWork) == FALSE)
							break;
						if(dwDlgDate != Today)
							break;
						if(dwGenID == HideGenID)
							continue;
						if(dwDlgPrivate != dwPrivate)
							continue;
						Mark = TEXT("...");
						break;
					}
				}
				_tcscpy(LineText[k], Mark);
				_tcscat(LineText[k++], TitleBuf);
			}
		}
		if(GrayMode)	// if Gray Record
		{
			//GrayDisp = (GrayDisp == 0);	// XOR Gray Info
			GrayDisp = GrayMode; // [iwad]
		}
//	Draw Gray BackGround
		// [iwad] 土日カラー対応
		if(dwGrayColor)
		{
			dwGrayBUF = dwGrayColor;
			hGrayBrush = CreateSolidBrush(dwGrayColor);
		}
		else
		{
			LTGRAYBUF = LTGRAYCOLOR;
			hGrayBrush = CreateSolidBrush(LTGRAYCOLOR);
		}

		SetupHolydayBrush(hDC, GrayDisp);	// Set HolyDay Color
		MonthGrayBuf[i / 7][i % 7] = GrayDisp;	// Save HolyDay Flag
		if(GrayDisp)	// Gray Mode?
		{
			txRect.top = ((i / 7)) * BoxHight + DayHight;
			txRect.left = (i % 7) * BoxWidth + 1;
			PatBlt(hDC, txRect.left, txRect.top,	// Fill
				BoxWidth - 1, BoxHight - 1, PATCOPY);
		}
		else	// [iwad] 土日カラー対応 else文の追加(休日設定されていない土日はカラーに)
		{
			if (Bit == 1)	// [iwad] Sunday Color
			{
				if(dwGrayColor)
				{
					dwGrayColor = LTGRAY_SUN;
					hGrayBrush = CreateSolidBrush(dwGrayColor);
				}
				else
				{
					LTGRAYCOLOR = LTGRAY_SUN;
					hGrayBrush = CreateSolidBrush(LTGRAYCOLOR);
				}
				SetupHolydayBrush(hDC, 1);
				txRect.top = ((i / 7)) * BoxHight + DayHight;
				txRect.left = (i % 7) * BoxWidth + 1;
				PatBlt(hDC, txRect.left, txRect.top,	// Fill
					BoxWidth - 1, BoxHight - 1, PATCOPY);
			}
			else if (Bit == 64)	// [iwad] Saturday Color
			{
				if(dwGrayColor)
				{
					dwGrayColor = LTGRAY_SAT;
					hGrayBrush = CreateSolidBrush(dwGrayColor);
				}
				else
				{
					LTGRAYCOLOR = LTGRAY_SAT;
					hGrayBrush = CreateSolidBrush(LTGRAYCOLOR);
				}
				SetupHolydayBrush(hDC, 1);
				txRect.top = ((i / 7)) * BoxHight + DayHight;
				txRect.left = (i % 7) * BoxWidth + 1;
				PatBlt(hDC, txRect.left, txRect.top,	// Fill
					BoxWidth - 1, BoxHight - 1, PATCOPY);
			}
		}

		// [iwad] 土日カラー対応
		if(dwGrayColor)
		{
			dwGrayColor = dwGrayBUF;
			hGrayBrush = CreateSolidBrush(dwGrayColor);
		}
		else
		{
			LTGRAYCOLOR = LTGRAYBUF;
			hGrayBrush = CreateSolidBrush(LTGRAYCOLOR);
		}
		hGrayBrush = CreateSolidBrush(0xFFFFFF);	// [iwad] 休日表示されている選択色を白にする

//	Draw App Text
		SelectObject(hDC, hMidFont);		// Set Font
		txRect.left = (i % 7) * BoxWidth + 1;	// Frame left
		txRect.right = txRect.left + BoxWidth - 1;// Frame right
		txRect.top = (i / 7) * BoxHight + DayHight + 1;	// Frame top
		txRect.bottom = txRect.top + dwMidFontHight;// 1 Line Hight
		for(j = 0; j < k; j++)
		{
			DrawText(hDC, LineText[j], -1, &txRect, DT_LEFT | DT_VCENTER | DT_NOPREFIX);
			txRect.top += dwMidFontHight;
			txRect.bottom += dwMidFontHight;
		}
//	Draw Day Number
		SetupHolydayBrush(hDC, FALSE);	// Set Normal Brush
		txRect.top = ((i / 7) + 1) * BoxHight + DayHight - dwSmallFontHight;
		txRect.bottom = txRect.top + dwSmallFontHight - 1;
		txRect.left = ((i % 7) + 1) * BoxWidth - dwSmallFontWidth * 3 + 1;
		txRect.right = ((i % 7) + 1) * BoxWidth;
//	Make Number String
		TempStr[2] = TEXT('\0');	// Set Terminator
		if(MonthBuf[0][i][2] < 10)
			TempStr[0] = TEXT(' ');
		else
			TempStr[0] = (TCHAR )MonthBuf[0][i][2] / 10 + TEXT('0');
		TempStr[1] = (TCHAR )MonthBuf[0][i][2] % 10 + TEXT('0');
//	Draw Number
		SelectObject(hDC, hSmallFont);		// Set Font
		PatBlt(hDC, txRect.left + 1, txRect.top,
				dwSmallFontWidth * 3 - 2, dwSmallFontHight - 1, PATCOPY);
		DrawText(hDC, TempStr, -1, &txRect, DT_RIGHT | DT_VCENTER);
		if(MonthBuf[0][i][1] == MonthBuf[0][15][1])	// Draw Frame
			PatBlt(hDC, txRect.left + 1, txRect.top,
					dwSmallFontWidth * 3 - 2, dwSmallFontHight - 1, PATINVERT);
		LinePoint[0].x = LinePoint[1].x = txRect.left;
		LinePoint[2].x = txRect.right;
		LinePoint[1].y = LinePoint[2].y = txRect.top - 1;
		LinePoint[0].y = txRect.bottom;
		Polyline(hDC, LinePoint, 3);	// Draw Frame
//	Draw Double Frame
		if(MonthBuf[0][i][0] == dwTodayYear
		&& MonthBuf[0][i][1] == dwTodayMonth
		&& MonthBuf[0][i][2] == dwTodayDay)// Today?
		{
			LinePoint[0].x = txRect.left,
			LinePoint[0].y = LinePoint[1].y = txRect.bottom - 1,
			LinePoint[1].x = LinePoint[2].x = (i % 7) * BoxWidth + 1;
			LinePoint[2].y = LinePoint[3].y = (i / 7) * BoxHight + DayHight;
			LinePoint[3].x = LinePoint[4].x = LinePoint[1].x + BoxWidth - 2;
			LinePoint[4].y = txRect.top;
			Polyline(hDC, LinePoint, 5);	// Draw Double Frame
		}
	}
	dwOldCurX = 7;
	DispReverseBox(hDC);
}

void DispReverseBox(HDC hDC)
{
	if(dwOldCurX != 7)
		ReverseBox(hDC, dwOldCurX, dwOldCurY);
	ReverseBox(hDC, dwCurX, dwCurY);
	dwOldCurX = dwCurX;
	dwOldCurY = dwCurY;
}

void ReverseBox(HDC hDC,DWORD x, DWORD y)
{
	SetupHolydayBrush(hDC, MonthGrayBuf[y][x]);	// Set Brush Color
	PatBlt(hDC,
			x * BoxWidth + 1,
			y * BoxHight + DayHight,
			BoxWidth - 1,
			BoxHight - 1, PATINVERT);
}
//
//	Month Key Proc
//
void MonthKeyProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case TEXT('\r'):	// Change to Day Screen
		JumpCurrentDay(0);	// No Adjust Last day(30 or 31) on Left Top
		SendMessage(hWnd, WM_COMMAND, IDM_VIEW_DAY, 0);
		break;
	case TEXT('\033'):	// ESC Key
		switch(dwExitMode)
		{
		case 1:
			SendMessage(hWnd, WM_CLOSE, 0, NULL);
			break;
		case 2:
			SendMessage(hWnd, MYWM_NOTIFYICON, ICONSTART, WM_LBUTTONDOWN);
			break;
		}
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
			SendMessage(hWnd, WM_COMMAND, IDM_VIEW_INPUT, NULL);
			SendMessage(GetFocus(), WM_CHAR, wParam, lParam);
		}
	}
}
//
//	Month Key Down Proc
//
void MonthKeyDownProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
DWORD	LastMonth, LastDay;
	LastMonth = GetCurrentMonth(0);
	LastDay = GetCurrentDay(0);
	if(wParam == VK_UP)
	{
		if(GetKeyState(VK_SHIFT) < 0)
			goto PrevMonth;
		if(GetKeyState(VK_CONTROL) < 0)
			goto PrevMonth;
CursorUp:
		if(dwCurY == 0)
		{
			if(dwYear == 1951 && dwMonth == 1)
				goto ExitMonth;
			LastMonth = GetCurrentMonth(0);
			LastDay = GetCurrentDay(0);
			dwCurY = 4;
PrevMonth:
			if(dwYear == 1951 && dwMonth == 1)
				goto ExitMonth;
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
			MonthScreen(hDC);
		}
		else
		{
			dwCurY--;
			DispReverseBox(hDC);
		}
		InvalidateRect(hWnd, NULL, FALSE);
		return;
	}
	else if(wParam == VK_DOWN)
	{
		if(GetKeyState(VK_SHIFT) < 0)
			goto NextMonth;
		if(GetKeyState(VK_CONTROL) < 0)
			goto NextMonth;
CursorDown:
		if(dwCurY == 4)
		{
			if(dwYear == 2077 && dwMonth == 12)
				goto ExitMonth;
			LastMonth = GetCurrentMonth(0);
			LastDay = GetCurrentDay(0);
			dwCurY = 0;
NextMonth:
			if(dwYear == 2077 && dwMonth == 12)
				goto ExitMonth;
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
			MonthScreen(hDC);
		}
		else
		{
			dwCurY++;
			DispReverseBox(hDC);
		}
		InvalidateRect(hWnd, NULL, FALSE);
		return;
	}
	else if(wParam == VK_LEFT)
	{
		if(dwCurX == 0)
		{
			dwCurX = 6;
			if(dwCurY == 0)
				goto CursorUp;
			dwCurY--;
		}
		else
			dwCurX--;
		DispReverseBox(hDC);
		InvalidateRect(hWnd, NULL, FALSE);
		return;
	}
	else if(wParam == VK_RIGHT)
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
ExitMonth:
		DispReverseBox(hDC);
		InvalidateRect(hWnd, NULL, FALSE);
		return;
	}
	else if(wParam == VK_PRIOR)
		goto PrevMonth;
	else if(wParam == VK_NEXT)
		goto NextMonth;
}
//
//	Get Current Date Data
//
DWORD GetCurrentYear(DWORD Page)
{
	return MonthBuf[Page][(dwCurY % 6)* 7 + dwCurX][0];
}
DWORD GetCurrentMonth(DWORD Page)
{
	return MonthBuf[Page][(dwCurY % 6) * 7 + dwCurX][1];
}
DWORD GetCurrentDay(DWORD Page)
{
	return MonthBuf[Page][(dwCurY % 6)* 7 + dwCurX][2];
}
DWORD GetCurrentWeekDay(void)
{
	return ((dwCurY % 6) * 7 + dwCurX) % 7;
}
//
//	Get Current Select Date
//
DWORD GetCurrentDate(void)
{
DWORD	CurrentArray;
	CurrentArray = dwCurY / 6;
	return GetDate(	GetCurrentYear(CurrentArray),
					GetCurrentMonth(CurrentArray),
					GetCurrentDay(CurrentArray));
}
//
//	Get Week Day From Date
//
DWORD ConvDateToWeekDay(DWORD date)
{
DWORD	Day, Year, Month;
	Year = (date / 512) + 1950;
	Month = (date / 32) & 15;
	Day = (Year + 3) / 4 + Year + 5;
	if(Year % 4)
		Day += NormalOffset[Month - 1];// Normal Year
	else
		Day += SpecialOffset[Month - 1];// Olympic Year
	return (Day + (date & 31) - 1) % 7;
}
//
//	Get Week Number From Date
//
DWORD ConvDateToWeekNum(DWORD date)
{
	return ((date & 31) - 1) / 7;
}
//
//	Get Final Day of the Month
//
DWORD GetFinalDay(DWORD date)
{
DWORD	Year, Month;
	Year = (date / 512) + 1950;
	Month = (date / 32) & 15;
	if((Year % 4) == 0 && Month == 2)
		return 29;
	else
		return LastDay[Month - 1];
}
//
//	Get Sequence Number of the Day (0-365)
//
DWORD GetDayTotal(void)
{
DWORD	i, tempm, sum = 0;
	tempm = GetCurrentMonth(0);
	for(i = 1;i < tempm; i++)
		sum += LastDay[i - 1];	// Sum up Last Month
	if((GetCurrentYear(0) % 4) == 0 && tempm > 2)	// For Olympic Year (2/29)
		sum++;
	return sum + GetCurrentDay(0);
}
//
//	Get Today's Date
//
void GetToday(void)
{
SYSTEMTIME	Now;
	GetLocalTime(&Now);
	dwTodayYear = Now.wYear;
	dwTodayMonth = Now.wMonth;
	dwTodayDay = Now.wDay;
	dwTodayHour = Now.wHour;
	dwTodayMinute = Now.wMinute;
}
//
//	Jump Today
//
void JumpToday(void)
{
	GetToday();
	JumpDay(dwTodayYear, dwTodayMonth, dwTodayDay);
}	
//
//	Jump to Specific Day
//
void JumpDay(DWORD Year, DWORD Month, DWORD Day)
{
	if(dwYear != Year || dwMonth != Month)	// Current Month?
		SetupMonth(Year, Month);	// Setup Month Array
	dwCurX = (Day + dwDayOffset - 1) % 7;	// Get Current BlackBox X
	dwCurY = (Day + dwDayOffset - 1) / 7;	// Get Current BlackBox Y
	if((DispMode != 3) && (dwDayOffset + Day) > 35)	// For 6 line Month
	{
		if(Month == 12)
		{
			Month = 1;
			Year++;
			if(Year > 2077)
			{
				Year = 2077;
				Month = 12;
			}
		}
		else
			Month++;
		SetupMonth(Year, Month);	// Setup Month Array
		dwCurY = 0;
	}
}
//
//	Jump DWORD Date
//
void JumpDate(DWORD Date)
{
	JumpDay(Date / 512 + 1950, (Date / 32) & 15, Date & 31);
}
//
//	Jump Current Day
//
void JumpCurrentDay(DWORD Page)
{
	JumpDay(GetCurrentYear(Page), GetCurrentMonth(Page), GetCurrentDay(Page));
}
//
//	Setup Month Array
//
void SetupMonth(DWORD Year, DWORD Month)
{
DWORD	i;
DWORD	YearSave, MonthSave;
	YearSave = Year;
	MonthSave = Month;
//	Setup Prev Month
	MonthSave--;
	if(MonthSave == 0)
	{
		MonthSave = 12;
		YearSave--;
		if(YearSave < 1951)	// Check Low Limit
		{
			YearSave = 1951;
			MonthSave = 12;
		}
	}
	SetupMonthSub(YearSave, MonthSave, 5);// Set Index 5
	YearSave = Year;	// Restore Year
	MonthSave = Month;	// Restore Month
//	Setup Next Month
	for(i = 1; i < 5; i++)
	{
		MonthSave++;
		if(MonthSave == 13)
		{
			MonthSave = 1;
			YearSave++;
			if(YearSave > 2077)
			{
				YearSave = 2077;
				MonthSave = 12;
			}
		}
		SetupMonthSub(YearSave, MonthSave, i);
	}
//	Setup Current Month
	SetupMonthSub(Year, Month, 0);
}
//
//	Setup Month Array Sub
//
void SetupMonthSub(DWORD Year, DWORD Month, DWORD Index)
{
DWORD	i, FinalDay, LastFinalDay, tempY, tempM, tempD;
	dwOldCurX = 7;
	dwDayOffset = (Year + 3) / 4 + Year + 5;
	if(Year % 4)
		dwDayOffset += NormalOffset[Month - 1];		// Normal Year
	else
		dwDayOffset += SpecialOffset[Month - 1];	// Olympic Year
	FinalDay = LastDay[Month - 1];	// Get Final Day
	if(Month == 1)
		LastFinalDay = 31;			// Last Year Decmber 31
	else
		LastFinalDay = LastDay[Month - 2];
	if((Year % 4) == 0 && Month == 2)	// February of Olympic Year?
		FinalDay++;					// Reset 28 to 29
	if((Year % 4) == 0 && Month == 3)	// March of Olympic Year?
		LastFinalDay++;				// Reset Last Month 28 to 29

	dwDayOffset %= 7;
//	Set Arry of This Month
	for(i = 0; i < FinalDay; i++)	// Repeat for All days of Month
	{
		MonthBuf[Index][i + dwDayOffset][0] = Year;	// Set Array Data (Year)
		MonthBuf[Index][i + dwDayOffset][1] = Month;	// Set Array Data (Month)
		MonthBuf[Index][i + dwDayOffset][2] = i + 1;	// Set Array Data (Day)
	}
//	Prepare for Next Month Data
	if(Year == 2077 && Month ==12)
	{
		tempY = 2077; tempM = 12;
	}
	else
	{
		if(Month == 12)	// December?
			{tempY = Year + 1; tempM = 1;}		// New Year Next Month
		else
			{tempY = Year; tempM = Month + 1;}	// Normal Next Month
	}
	tempD = 1;	// Day is 1st
//	Set Arry of Next Month
	for(;i < (42 - dwDayOffset); i++)		// Repeat for Next Month few Days
	{
		MonthBuf[Index][i + dwDayOffset][0] = tempY;	// Set Array Data (Year)
		MonthBuf[Index][i + dwDayOffset][1] = tempM;	// Set Array Data (Month)
		MonthBuf[Index][i + dwDayOffset][2] = tempD++;	// Set Array Data (Day)
	}
//	Prepare for Last Month Data
	if(dwDayOffset)	// 1st is't SunDay?
	{
		if(Year == 1951 && Month ==1)
		{
			tempY = 1951; tempM = 1;
	}
	else
	{
		if(Month == 1)	// January?
			{tempY = Year - 1; tempM = 12;}		// Last Year Last Month
		else
			{tempY = Year; tempM = Month - 1;}	// Normal Next Month
	}
	tempD = LastFinalDay;	// Set Final Day
//	Set Arry of Last Month
		for(i = dwDayOffset - 1; i ; i--)	// Decrement Set
		{
			MonthBuf[Index][i][0] = tempY;		// Set Array Data (Year)
			MonthBuf[Index][i][1] = tempM;		// Set Array Data (Month)
			MonthBuf[Index][i][2] = tempD--;	// Set Array Data (Day)
		}
		MonthBuf[Index][0][0] = tempY;		// Set Array Data (Year)
		MonthBuf[Index][0][1] = tempM;		// Set Array Data (Month)
		MonthBuf[Index][0][2] = tempD--;	// Set Array Data (Day)
	}
	dwYear = Year;
	dwMonth = Month;
}
//
//	Mouse Left Button Proc
//
void MonthLeftProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
DWORD	Top = DayHight + dwWinTop;
	if(HIWORD(lParam) <= Top || LOWORD(lParam) < 1)
	{
		SendMessage(hWnd, WM_COMMAND, IDM_JUMP_DATE, 0);
		return;
	}
	dwCurX = (LOWORD(lParam) - 1) / BoxWidth;
	if(dwCurX > 6)
		dwCurX = 6;
	dwCurY = (HIWORD(lParam) - Top) / BoxHight;
	DispReverseBox(hDC);
	InvalidateRect(hWnd, NULL, FALSE);
}

//
//	Mouse Double Click Proc
//
void MonthDblProc(HWND hWnd, HDC hDC, WPARAM wParam, LPARAM lParam)
{
DWORD	Top = DayHight + dwWinTop;
	if(HIWORD(lParam) <= Top || LOWORD(lParam) < 1)
		return;
	dwCurX = (LOWORD(lParam) - 1) / BoxWidth;
	if(dwCurX > 6)
		dwCurX = 6;
	dwCurY = (HIWORD(lParam) - Top) / BoxHight;
	SendMessage(hWnd, WM_CHAR, TEXT('\r'), NULL);
}

//
//	Get Next Date
//
void GetNextDate(LPDWORD lpd)
{
DWORD	date = *lpd;
DWORD	year, month, day;
	year = date / 512;
	if(year)
		year += 1950;
	month = (date / 32) & 15;
	day = date & 31;
	if(year && (year % 4) == 0 && month == 2)	// For Olympic Feb.
	{
		day++;
		if(day == 30)
			{day = 1; month = 3;}	// Set March 1st
	}
	else
	{
		day++;
		if(day > LastDay[month - 1])
			{day = 1; month++;}		// Set Next Month
		if(month == 13)
		{
			if(year)
			{
				year++;
				if(year > 2077)	// Check High Limit
					return;	// No Operation
			}
			month = 1;
		}	// Set Next Year Jan 1st
	}
	if(year)
		*lpd = GetDate(year, month, day);
	else
		*lpd = month * 32 + day;
	return;
}
//
//	Get Prev Date
//
void GetPrevDate(LPDWORD lpd)
{
DWORD	date = *lpd;
DWORD	year, month, day;
	year = date / 512;
	if(year)
		year += 1950;
	month = (date / 32) & 15;
	day = date & 31;
	if(year && (year % 4) == 0 && month == 3 && day == 1)	// For Olympic Feb.
		{day = 29; month = 2;}	// Set March 1st
	else
	{
		if(day != 1)
			day--;
		else
		{
			if(month == 1)
			{
				if(year)	// For Date Only
				{
					year--;
					if(year == 1950)	// Check Low Limit
						return;
				}
				month = 12;
				day = 31;
			}
			else
			{
				month--;
				day = LastDay[month - 1];
			}		// Set Next Month
		}
	}
	if(year)
		*lpd = GetDate(year, month, day);
	else
		*lpd = month * 32 + day;
		return;
}
//
//	Jump Dialog Proc
//
BOOL CALLBACK JumpDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
TCHAR	DateBuf[64];
DWORD	DateVal, i;
DWORD	CurYear, CurMonth, FinalDay;
	switch(message)
	{
//	Init Jump DialogBox
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg, IDC_DATE, EM_SETLIMITTEXT, 10, 0);
		SetDlgItemText(hDlg, IDC_DATE,
			GetDateString(GetDate(GetCurrentYear(0), GetCurrentMonth(0), GetCurrentDay(0))));
		if(lpEdit == NULL)	// Not Set Yet?
			lpEdit = (WNDPROC )GetWindowLong(GetDlgItem(hDlg, IDC_DATE), GWL_WNDPROC);// Store Edit Handler Adrs
		SetWindowLong(GetDlgItem(hDlg, IDC_DATE), GWL_WNDPROC, (LONG )OyajiDateEditProc);	// Set Hook Handler Adrs
		return TRUE;
//	Buttons
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDC_NEXTWEEK:
			GetDlgItemText(hDlg, IDC_DATE, DateBuf, sizeof(DateBuf) / sizeof(TCHAR) - 1);			
			if(ConvDateToDword(&DateVal, DateBuf))	// Write Format?
			{
				for(i = 0; i < 7; i++)
					GetNextDate(&DateVal);	// Get Next Date
				SetDlgItemText(hDlg, IDC_DATE, GetDateString(DateVal));			
			}
			break;
		case IDC_PREVWEEK:
			GetDlgItemText(hDlg, IDC_DATE, DateBuf, sizeof(DateBuf) / sizeof(TCHAR) - 1);			
			if(ConvDateToDword(&DateVal, DateBuf))	// Write Fromat?
			{
				for(i = 0; i < 7; i++)
					GetPrevDate(&DateVal);	// Get Prev Date
				SetDlgItemText(hDlg, IDC_DATE, GetDateString(DateVal));			
			}
			break;
		case IDC_NEXTMONTH:
			GetDlgItemText(hDlg, IDC_DATE, DateBuf, sizeof(DateBuf) / sizeof(TCHAR) - 1);			
			if(ConvDateToDword(&DateVal, DateBuf))	// Write Fromat?
			{
				CurMonth = (DateVal / 32) & 15;	// Get Current Month
				CurYear = DateVal / 512;
				if(CurMonth == 12)
				{
					CurMonth = 0;
					CurYear++;
					if(CurYear > 127)
						break;
				}
				CurMonth++;
				FinalDay = GetFinalDay(CurYear * 512 + CurMonth * 32);
				if(FinalDay < (DateVal & 31))
					DateVal = CurYear * 512 + CurMonth * 32 + FinalDay;
				else
					DateVal = CurYear * 512 + CurMonth * 32 + (DateVal & 31);
				SetDlgItemText(hDlg, IDC_DATE, GetDateString(DateVal));
			}
			break;
		case IDC_PREVMONTH:
			GetDlgItemText(hDlg, IDC_DATE, DateBuf, sizeof(DateBuf) / sizeof(TCHAR) - 1);			
			if(ConvDateToDword(&DateVal, DateBuf))	// Write Fromat?
			{
				CurMonth = (DateVal / 32) & 15;	// Get Current Month
				CurYear = DateVal / 512;
				if(CurMonth == 1)
				{
					CurMonth = 13;
					CurYear--;
					if(CurYear == 0)	// Check Low Limit
						break;
				}
				CurMonth--;
				FinalDay = GetFinalDay(CurYear * 512 + CurMonth * 32);
				if(FinalDay < (DateVal & 31))
					DateVal = CurYear * 512 + CurMonth * 32 + FinalDay;
				else
					DateVal = CurYear * 512 + CurMonth * 32 + (DateVal & 31);
				SetDlgItemText(hDlg, IDC_DATE, GetDateString(DateVal));
			}
			break;
		case IDC_PREVYEAR:
			GetDlgItemText(hDlg, IDC_DATE, DateBuf, sizeof(DateBuf) / sizeof(TCHAR) - 1);			
			if(ConvDateToDword(&DateVal, DateBuf))	// Write Fromat?
			{
				CurMonth = (DateVal / 32) & 15;	// Get Current Month
				CurYear = DateVal / 512;
				CurYear--;
				if(CurYear == 0)
					break;
				FinalDay = GetFinalDay(CurYear * 512 + CurMonth * 32);
				if(FinalDay < (DateVal & 31))
					DateVal = CurYear * 512 + CurMonth * 32 + FinalDay;
				else
					DateVal = CurYear * 512 + CurMonth * 32 + (DateVal & 31);
				SetDlgItemText(hDlg, IDC_DATE, GetDateString(DateVal));
			}
			break;
		case IDC_NEXTYEAR:
			GetDlgItemText(hDlg, IDC_DATE, DateBuf, sizeof(DateBuf) / sizeof(TCHAR) - 1);			
			if(ConvDateToDword(&DateVal, DateBuf))	// Write Fromat?
			{
				CurMonth = (DateVal / 32) & 15;	// Get Current Month
				CurYear = DateVal / 512;
				CurYear++;
				if(CurYear > 127)
					break;
				FinalDay = GetFinalDay(CurYear * 512 + CurMonth * 32);
				if(FinalDay < (DateVal & 31))
					DateVal = CurYear * 512 + CurMonth * 32 + FinalDay;
				else
					DateVal = CurYear * 512 + CurMonth * 32 + (DateVal & 31);
				SetDlgItemText(hDlg, IDC_DATE, GetDateString(DateVal));
			}
			break;
		case IDC_TODAY:
			JumpToday();
			EndDialog(hDlg, TRUE);
			return TRUE;
		case IDOK:
			GetDlgItemText(hDlg, IDC_DATE, DateBuf, sizeof(DateBuf) / sizeof(TCHAR) - 1);			
			if(ConvDateToDword(&DateVal, DateBuf) == FALSE || DateVal / 512 == 0)
			{
//				MessageBox(hDlg, TEXT("Invalid Date"), TEXT("Input Error"), MB_OK);
				MessageBox(hDlg, TEXT("無効ﾃﾞｰﾀ"), TEXT("Input Error"), MB_OK);	// by uminchu
				break;
			}
			JumpDate(DateVal);
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
