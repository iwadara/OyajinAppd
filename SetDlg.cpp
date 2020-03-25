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
LPCTSTR	GetTimeString(DWORD);
LPCTSTR	GetSimpleTimeString(DWORD);
BOOL	ConvTimeToDword(LPDWORD, LPTSTR);
DWORD	GetTime(DWORD, DWORD);
BOOL	FileOpenDialog(HWND, BOOL, OPENFILENAME *, LPTSTR, LPTSTR, LPTSTR, LPTSTR, LPTSTR);
static BOOL	CALLBACK OptionStyleProc(HWND, UINT, WPARAM, LPARAM);
static BOOL	CALLBACK OptionFontProc(HWND, UINT, WPARAM, LPARAM);
static BOOL	CALLBACK OptionActionProc(HWND, UINT, WPARAM, LPARAM);
static BOOL	CALLBACK OptionDefProc(HWND, UINT, WPARAM, LPARAM);
static BOOL	CALLBACK OptionPrivateProc(HWND, UINT, WPARAM, LPARAM);
static BOOL	CALLBACK OptionAlarmProc(HWND, UINT, WPARAM, LPARAM);
static BOOL	CALLBACK OptionToDoProc(HWND, UINT, WPARAM, LPARAM);
static BOOL	CALLBACK OptionDirProc(HWND, UINT, WPARAM, LPARAM);
static BOOL	CALLBACK OptionColorProc(HWND, UINT, WPARAM, LPARAM);
static BOOL CALLBACK OptionAboutProc(HWND, UINT, WPARAM, LPARAM);
static void ClearComboList(HWND, DWORD);
void	SetupBrush(HDC);
//
//	Global Vars
//
static	DWORD	TimeButton[] = {IDC_12H, IDC_24H};
static	DWORD	DispButton[] = {IDC_ALLHOUR, IDC_APPONLY};
static	DWORD	AlarmButton[] = {IDC_BEEP, IDC_WAVE};
static	DWORD	EscButton[] = {IDC_ESCNONE, IDC_ESCEXIT, IDC_ESCHIDE};
static	DWORD	StepButton[] = {IDC_1H, IDC_30M};
static	DWORD	ColorValue[][3] = {
				IDC_BACKRED, IDC_BACKGREEN, IDC_BACKBLUE,
				IDC_GRAYRED, IDC_GRAYGREEN, IDC_GRAYBLUE,
				IDC_TEXTRED, IDC_TEXTGREEN, IDC_TEXTBLUE,
				IDC_LINERED, IDC_LINEGREEN, IDC_LINEBLUE};
static	DWORD	ColorVar[][2] = {
				IDC_BACKDEF, (DWORD )&dwBackColor,
				IDC_GRAYDEF, (DWORD )&dwGrayColor,
				IDC_TEXTDEF, (DWORD )&dwTextColor,
				IDC_LINEDEF, (DWORD )&dwLineColor};
				static	DWORD	ColorShift[3] = {1, 256, 65536};
//
//	Setting Options
//
BOOL SettingSheet(void)
{
PROPSHEETPAGE	Page[10];	// [iwad] [9]→[10] Aboutを追加
PROPSHEETHEADER	PageHeader;

//	Clear Memory
	memset(&Page, 0, sizeof(Page));
	memset(&PageHeader, 0, sizeof(PageHeader));
//	Style Sheet
	Page[0].dwSize = sizeof(PROPSHEETPAGE);
	Page[0].hInstance = hInst;
	Page[0].pszTemplate = MAKEINTRESOURCE(IDD_PROP_STYLE);
	Page[0].pfnDlgProc = OptionStyleProc;
//	Font Sheet
	Page[1].dwSize = sizeof(PROPSHEETPAGE);
	Page[1].hInstance = hInst;
	Page[1].pszTemplate = MAKEINTRESOURCE(IDD_PROP_FONT);
	Page[1].pfnDlgProc = OptionFontProc;
//	Alarm Sheet
	Page[2].dwSize = sizeof(PROPSHEETPAGE);
	Page[2].hInstance = hInst;
	Page[2].pszTemplate = MAKEINTRESOURCE(IDD_PROP_ALARM);
	Page[2].pfnDlgProc = OptionAlarmProc;
//	Defaults Sheet
	Page[3].dwSize = sizeof(PROPSHEETPAGE);
	Page[3].hInstance = hInst;
	Page[3].pszTemplate = MAKEINTRESOURCE(IDD_PROP_DEFAULTS);
	Page[3].pfnDlgProc = OptionDefProc;
//	Private Sheet
	Page[4].dwSize = sizeof(PROPSHEETPAGE);
	Page[4].hInstance = hInst;
	Page[4].pszTemplate = MAKEINTRESOURCE(IDD_PROP_PRIVATE);
	Page[4].pfnDlgProc = OptionPrivateProc;
//	ToDo Sheet
	Page[5].dwSize = sizeof(PROPSHEETPAGE);
	Page[5].hInstance = hInst;
	Page[5].pszTemplate = MAKEINTRESOURCE(IDD_PROP_TODO);
	Page[5].pfnDlgProc = OptionToDoProc;
//	Directry Sheet
	Page[6].dwSize = sizeof(PROPSHEETPAGE);
	Page[6].hInstance = hInst;
	Page[6].pszTemplate = MAKEINTRESOURCE(IDD_PROP_DIR);
	Page[6].pfnDlgProc = OptionDirProc;
//	Color Sheet
	Page[7].dwSize = sizeof(PROPSHEETPAGE);
	Page[7].hInstance = hInst;
	Page[7].pszTemplate = MAKEINTRESOURCE(IDD_PROP_COLOR);
	Page[7].pfnDlgProc = OptionColorProc;
//	Action Sheet
	Page[8].dwSize = sizeof(PROPSHEETPAGE);
	Page[8].hInstance = hInst;
	Page[8].pszTemplate = MAKEINTRESOURCE(IDD_PROP_ACTION);
	Page[8].pfnDlgProc = OptionActionProc;
//	[iwad] About Sheet
	Page[9].dwSize = sizeof(PROPSHEETPAGE);
	Page[9].hInstance = hInst;
	Page[9].pszTemplate = MAKEINTRESOURCE(IDD_ABOUTDLG);
	Page[9].pfnDlgProc = OptionAboutProc;
//	Page Headder
	PageHeader.dwSize = sizeof(PROPSHEETHEADER);
	PageHeader.hInstance = hInst;
//	PageHeader.pszCaption = TEXT("Settings");
	PageHeader.pszCaption = TEXT("ｵﾌﾟｼｮﾝ設定"); // by uminchu
	PageHeader.hwndParent = hMainWnd;
	PageHeader.nPages = countof(Page);
	PageHeader.ppsp = Page;
	// PageHeader.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
	PageHeader.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW | PSH_MAXIMIZE;	// [iwad] オプション画面の拡張
//
	return PropertySheet(&PageHeader);
}
//
//	Style Sheet Proc
//
static BOOL CALLBACK OptionStyleProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
DWORD	i, HourSave;
DWORD	TimeVal;
TCHAR	TimeBuf[16];
	switch(message)
	{
//	Init Style DialogBox
	case WM_INITDIALOG:
//	Setup Radio Buttons
		for(i = 0; i < 2; i++)
			SendDlgItemMessage(hDlg, TimeButton[i], BM_SETCHECK, (i == dwHourMode), 0);
		for(i = 0; i < 2; i++)
			SendDlgItemMessage(hDlg, DispButton[i], BM_SETCHECK, (i == dwAppDispMode), 0);
		for(i = 0; i < 2; i++)
			SendDlgItemMessage(hDlg, StepButton[i], BM_SETCHECK, (i == dwDispStep), 0);
		SendDlgItemMessage(hDlg, IDC_GRAPH, BM_SETCHECK, dwGraphMode, 0);
		SendDlgItemMessage(hDlg, IDC_UNDER, BM_SETCHECK, dwUnderBar, 0);
		SetDlgItemText(hDlg, IDC_FIRSTHOUR, GetTimeString(GetTime(dwStartTime, 0)));
		SetDlgItemText(hDlg, IDC_LASTHOUR, GetTimeString(GetTime(dwFinalTime, 0)));
		return TRUE;
//	Radio Buttons
	case WM_COMMAND:
		if(GET_WM_COMMAND_CMD(wParam, lParam) != BN_CLICKED)
			break;
		switch(GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDC_12H:
			HourSave = dwHourMode;
			dwHourMode = 0;
			goto SetupHour;
		case IDC_24H:
			HourSave = dwHourMode;
			dwHourMode = 1;
SetupHour:
			SetDlgItemText(hDlg, IDC_FIRSTHOUR, GetTimeString(GetTime(dwStartTime, 0)));
			SetDlgItemText(hDlg, IDC_LASTHOUR, GetTimeString(GetTime(dwFinalTime, 0)));
			dwHourMode = HourSave;
			return TRUE;
		}
		break;
//	OK Button
	case WM_NOTIFY:
		{
		LPNMHDR pnmh = (LPNMHDR )lParam;
			switch(pnmh->code)
			{
			case PSN_APPLY:
				for(i = 0; i < 2; i++)
					if(SendDlgItemMessage(hDlg, TimeButton[i], BM_GETCHECK, 0, 0))
						{dwHourMode = i; break;}
				for(i = 0; i < 2; i++)
					if(SendDlgItemMessage(hDlg, DispButton[i], BM_GETCHECK, 0, 0))
						{dwAppDispMode = i; break;};
				for(i = 0; i < 2; i++)
					if(SendDlgItemMessage(hDlg, StepButton[i], BM_GETCHECK, 0, 0))
						{dwDispStep = i; break;};
				dwGraphMode = SendDlgItemMessage(hDlg, IDC_GRAPH, BM_GETCHECK, 0, 0);
				dwUnderBar = SendDlgItemMessage(hDlg, IDC_UNDER, BM_GETCHECK, 0, 0);
				GetDlgItemText(hDlg, IDC_FIRSTHOUR, TimeBuf, sizeof(TimeBuf) / sizeof(TCHAR) - 1);
				if(ConvTimeToDword(&TimeVal, TimeBuf) == FALSE)
					dwStartTime = 8;	// Invalid Time Then Set 8:00
				else
				{
					if(TimeVal > GetTime(12, 0))	// If 12H Mode then 1:00 Returns 13:00
						TimeVal -= GetTime(12, 0);	// Adjust Simple Hour
					dwStartTime = TimeVal / 64;	// Get Only Hour
					if(dwStartTime > 12)	// Start Time Over Noon?
						dwStartTime = 12;	// Force Set Noon
				}
				GetDlgItemText(hDlg, IDC_LASTHOUR, TimeBuf, sizeof(TimeBuf) / sizeof(TCHAR) - 1);
				if(ConvTimeToDword(&TimeVal, TimeBuf) == FALSE)	// Get Only Hour
					dwFinalTime = 24;	// Force Set MidNight
				else
					dwFinalTime = TimeVal / 64;	// Get Only Hour
//				if(dwFinalTime < dwStartTime + DayLineCount)	// Final Time Less Screen?
//					dwFinalTime = dwStartTime + DayLineCount;	// Force Set Right Final Time
				if(dwFinalTime > 31)	// Check Max Limit
					dwFinalTime = 31;	// Max is 31
				return TRUE;
			}
		}
	}

	return FALSE;
}
//
//	Font Setup User CallBack Proc
//
static BOOL CALLBACK FontSetProc(ENUMLOGFONT *lpelf, TEXTMETRIC *lpntm, int FontType, LPARAM hDlg)
{
	if(FontType == 0 || FontType & RASTER_FONTTYPE)	// Vector or Raster Font?
	{
		if(lpelf->elfLogFont.lfFaceName[0] == TEXT('@'))
			return TRUE;	// Skip @ Font
		if(SendDlgItemMessage((HWND )hDlg, IDC_SMALLFONT, CB_ADDSTRING, 0, (LPARAM )lpelf->elfLogFont.lfFaceName) < 0)
			return FALSE;	// Memory Full then Break
		if(SendDlgItemMessage((HWND )hDlg, IDC_MIDFONT, CB_ADDSTRING, 0, (LPARAM )lpelf->elfLogFont.lfFaceName) < 0)
			return FALSE;	// Memory Full then Break
		if(SendDlgItemMessage((HWND )hDlg, IDC_BIGFONT, CB_ADDSTRING, 0, (LPARAM )lpelf->elfLogFont.lfFaceName) < 0)
			return FALSE;	// Memory Full then Break
	}
	else
	{
		if(lpelf->elfFullName[0] == TEXT('@'))
			return TRUE;	// Skip @ Font
		if(SendDlgItemMessage((HWND )hDlg, IDC_SMALLFONT, CB_ADDSTRING, 0, (LPARAM )lpelf->elfFullName) < 0)
			return FALSE;	// Memory Full then Break
		if(SendDlgItemMessage((HWND )hDlg, IDC_MIDFONT, CB_ADDSTRING, 0, (LPARAM )lpelf->elfFullName) < 0)
			return FALSE;	// Memory Full then Break
		if(SendDlgItemMessage((HWND )hDlg, IDC_BIGFONT, CB_ADDSTRING, 0, (LPARAM )lpelf->elfFullName) < 0)
			return FALSE;	// Memory Full then Break
	}
	return TRUE;	// Continue;
}
//
//	Font Size Setup
//
static void SetupFontSize(HWND hDlg, DWORD FontID, DWORD SizeID, DWORD Index, DWORD FontSize)
{
LOGFONT	LogFont;
TEXTMETRIC	tm;
HFONT	hFont;
TCHAR	FontName[LF_FULLFACESIZE], NumBuf[8];
int		i;
HDC		hDC;
	if(Index == CB_ERR)
		Index = 0;	// Force Set Top of List (Auto)
	SendDlgItemMessage(hDlg, SizeID, CB_RESETCONTENT, 0, 0);// Clear Size List Box
	EnableWindow(GetDlgItem(hDlg, SizeID), (Index)? 1: 0);// Gray Size Box if Auto
	if(Index)	// Setup Font Size List
	{
		SendDlgItemMessage(hDlg, FontID, CB_GETLBTEXT, Index, (LPARAM )FontName);// Clear List Box
		for(i = 8; i < 75/*33*/; i++)	// Repeat Each Size
		{
			memset(&LogFont, 0, sizeof(LogFont));	// Clear LOGFONT struct
			_tcscpy(LogFont.lfFaceName, FontName);	// Set Font Name to be Searched
			LogFont.lfHeight = i;// Set Font Height
			hFont = CreateFontIndirect(&LogFont);// Get Font Object
			if(hFont == NULL)
				continue;
			hDC = GetDC(hDlg);	// Get DC
			SelectObject(hDC, hFont);		// Set Font
			GetTextMetrics(hDC, &tm);	// Get Font Metrics
			ReleaseDC(hDlg, hDC);	// Release DC
			DeleteObject(hFont);	// Release Immidately				
			if(i != tm.tmHeight)	// Just Size?
				continue;	// No, Search Next Size
			wsprintf(NumBuf, TEXT("%d"), i);// Make Number String
			if(SendDlgItemMessage(hDlg, SizeID, CB_ADDSTRING, 0, (LPARAM )NumBuf) < 0)
				break;
		}
		wsprintf(NumBuf, TEXT("%d"), FontSize);// Make Number String
		i = SendDlgItemMessage(hDlg, SizeID, CB_FINDSTRING, -1, (LPARAM )NumBuf);
		if(i == CB_ERR)	// Not Found?
			i = 0;		// Force Set First Item on the List
		SendDlgItemMessage(hDlg, SizeID, CB_SETCURSEL, i, 0);
	}
}
//
//	Fonts Sheet Proc
//
static BOOL CALLBACK OptionFontProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
DWORD	SizeID, FontID, FontSize, i;
HDC		hDC;
	switch(message)
	{
//	Init Style DialogBox
	case WM_INITDIALOG:
//	Set NULL String for Default Font Select
		SendDlgItemMessage(hDlg, IDC_SMALLFONT, CB_RESETCONTENT, 0, 0);
//		SendDlgItemMessage(hDlg, IDC_SMALLFONT, CB_ADDSTRING, 0, (LPARAM )TEXT(" Auto"));
		SendDlgItemMessage(hDlg, IDC_SMALLFONT, CB_ADDSTRING, 0, (LPARAM )TEXT(" 自動"));	// by uminchu
		SendDlgItemMessage(hDlg, IDC_MIDFONT, CB_RESETCONTENT, 0, 0);
//		SendDlgItemMessage(hDlg, IDC_MIDFONT, CB_ADDSTRING, 0, (LPARAM )TEXT(" Auto"));
		SendDlgItemMessage(hDlg, IDC_MIDFONT, CB_ADDSTRING, 0, (LPARAM )TEXT(" 自動"));		// by uminchu
		SendDlgItemMessage(hDlg, IDC_BIGFONT, CB_RESETCONTENT, 0, 0);
//		SendDlgItemMessage(hDlg, IDC_BIGFONT, CB_ADDSTRING, 0, (LPARAM )TEXT(" Auto"));
		SendDlgItemMessage(hDlg, IDC_BIGFONT, CB_ADDSTRING, 0, (LPARAM )TEXT(" 自動"));		// by uminchu
		hDC = GetDC(hDlg);	// Get DC
		EnumFontFamilies(hDC, NULL, (FONTENUMPROC )FontSetProc, (LPARAM )hDlg);// Get Font List
		ReleaseDC(hDlg, hDC);	// Release DC
		i = SendDlgItemMessage(hDlg, IDC_SMALLFONT, CB_SELECTSTRING, -1, (LPARAM )szSmallFontName);
		SetupFontSize(hDlg, IDC_SMALLFONT, IDC_SMALLSIZE, i, dwSmallFontSize);
		i = SendDlgItemMessage(hDlg, IDC_MIDFONT, CB_SELECTSTRING, -1, (LPARAM )szMidFontName);
		SetupFontSize(hDlg, IDC_MIDFONT, IDC_MIDSIZE, i, dwMidFontSize);
		i = SendDlgItemMessage(hDlg, IDC_BIGFONT, CB_SELECTSTRING, -1, (LPARAM )szBigFontName);
		SetupFontSize(hDlg, IDC_BIGFONT, IDC_BIGSIZE, i, dwBigFontSize);
		return TRUE;
	case WM_COMMAND:
		switch(GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDC_SMALLFONT:
			SizeID = IDC_SMALLSIZE;
			FontID = IDC_SMALLFONT;
			FontSize = dwSmallFontSize;
			goto SizeSet;
		case IDC_MIDFONT:
			SizeID = IDC_MIDSIZE;
			FontID = IDC_MIDFONT;
			FontSize = dwMidFontSize;
			goto SizeSet;
		case IDC_BIGFONT:
			SizeID = IDC_BIGSIZE;
			FontID = IDC_BIGFONT;
			FontSize = dwBigFontSize;
SizeSet:
			if(HIWORD(wParam) != CBN_SELCHANGE)	// Font Change?
				break;	// No Operation
			i = SendDlgItemMessage(hDlg, FontID, CB_GETCURSEL, 0, 0);// Get Selection
			SetupFontSize(hDlg, FontID, SizeID, i, FontSize);	// Setup Font Size
			return TRUE;
		}
		break;
	case WM_NOTIFY:
		{
		LPNMHDR pnmh = (LPNMHDR )lParam;
			switch(pnmh->code)
			{
			case PSN_APPLY:
				i = SendDlgItemMessage(hDlg, IDC_SMALLFONT, CB_GETCURSEL, 0, 0);
				SendDlgItemMessage(hDlg, IDC_SMALLFONT, CB_GETLBTEXT, i, (LPARAM )szSmallFontName);
				if(i)// Not Auto?
					dwSmallFontSize = GetDlgItemInt(hDlg, IDC_SMALLSIZE, NULL, 0);
				i = SendDlgItemMessage(hDlg, IDC_MIDFONT, CB_GETCURSEL, 0, 0);
				SendDlgItemMessage(hDlg, IDC_MIDFONT, CB_GETLBTEXT, i, (LPARAM )szMidFontName);
				if(i)// Not Auto?
					dwMidFontSize = GetDlgItemInt(hDlg, IDC_MIDSIZE, NULL, 0);
				i = SendDlgItemMessage(hDlg, IDC_BIGFONT, CB_GETCURSEL, 0, 0);
				SendDlgItemMessage(hDlg, IDC_BIGFONT, CB_GETLBTEXT, i, (LPARAM )szBigFontName);
				if(i)// Not Auto?
					dwBigFontSize = GetDlgItemInt(hDlg, IDC_BIGSIZE, NULL, 0);
			case PSN_RESET:
				return TRUE;
			}
		}
	}
	return FALSE;
}
//
//	Alarm Sheet Proc
//
static BOOL CALLBACK OptionAlarmProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
DWORD	i;
TCHAR	FullFileName[MAX_PATH], FileName[MAX_PATH];	// File Name Buffer
OPENFILENAME	FN;	// File Open Structure
DWORD	OrgVolume;
TCHAR	WaveName[MAX_PATH];
DWORD	VolVal;
	switch(message)
	{
//	Init Style DialogBox
	case WM_INITDIALOG:
//	Setup Radio Buttons
		for(i = 0; i < 2; i++)
			SendDlgItemMessage(hDlg, AlarmButton[i], BM_SETCHECK, (i == dwAlarmMode), 0);
//	Setup EditBoxs
		SetDlgItemInt(hDlg, IDC_COUNT, dwLoopCount, 0);
		SetDlgItemInt(hDlg, IDC_ALARMTIME, dwAlarmTime, 0);
		SetDlgItemInt(hDlg, IDC_SNOOZE, dwAlarmSnooze, 0);
		SetDlgItemText(hDlg, IDC_WAVENAME, szWaveFileName);
//	Setup Track Bar
		SendDlgItemMessage(hDlg, IDC_VOLUME, TBM_SETRANGE, 0, 0x0a0000);
		SendDlgItemMessage(hDlg, IDC_VOLUME, TBM_SETPOS, TRUE, dwVolume);
		return TRUE;
//	Ok Button
	case WM_NOTIFY:
		{
		LPNMHDR pnmh = (LPNMHDR )lParam;
			switch(pnmh->code)
			{
			case PSN_APPLY:
				for(i = 0; i < 2; i++)
					if(SendDlgItemMessage(hDlg, AlarmButton[i], BM_GETCHECK, 0, 0))
						{dwAlarmMode = i; break;};
//	Save EditBoxs(Int)
				dwLoopCount = GetDlgItemInt(hDlg, IDC_COUNT, 0, 0);
				dwAlarmTime = GetDlgItemInt(hDlg, IDC_ALARMTIME, 0, 0);
				dwAlarmSnooze = GetDlgItemInt(hDlg, IDC_SNOOZE, 0, 0);
				if(dwAlarmSnooze < 3)	// Check Snooze Time
					dwAlarmSnooze = 3;	// Min Snooze is 3
//	Save Track Bar
				dwVolume = SendDlgItemMessage(hDlg, IDC_VOLUME, TBM_GETPOS, 0, 0);
//	Save EditBox
				GetDlgItemText(hDlg, IDC_WAVENAME, szWaveFileName, MAX_PATH - 1);
				return TRUE;
			}
		}
		break;
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
//	Browse File
		case IDC_BROWSE:
			if(FileOpenDialog(hDlg, 0, &FN,
				FullFileName,
				FileName,
				szWaveDir,
				TEXT("Wave File\0*.wav\0All Files\0*.*\0\0"),
				TEXT("wav")) == FALSE)				// Get Open File Name
				return TRUE;						// Cancel Pressed
			_tcscpy(szWaveFileName, &FullFileName[FN.nFileOffset]);// Save Current Name
			SetDlgItemText(hDlg, IDC_WAVENAME, szWaveFileName);
			_tcscpy(szWaveDir, FullFileName);	// Save Current FileName & Path
			*(szWaveDir + FN.nFileOffset) = TEXT('\0');	// Cut Only Dir Name
			return TRUE;
		case IDC_TEST:
			waveOutGetVolume(NULL, &OrgVolume);	// Get Original Volume
			VolVal = ((0xffff / 10) * SendDlgItemMessage(hDlg, IDC_VOLUME, TBM_GETPOS, 0, 0));
			VolVal |= VolVal * 0x10000;
			waveOutSetVolume(NULL, VolVal);
			sndPlaySound(NULL, SND_ASYNC | SND_FILENAME);// Stop Current Sound
			if(SendDlgItemMessage(hDlg, IDC_BEEP, BM_GETCHECK, 0, 0))
				_tcscpy(WaveName, TEXT(""));// Default Sound
			else
			{	// Wave Mode
				_tcscpy(WaveName, szWaveDir);
				_tcscat(WaveName, szWaveFileName);
			}
			sndPlaySound(WaveName, SND_ASYNC | SND_FILENAME);// Loop Play
			while(sndPlaySound(WaveName, SND_ASYNC | SND_NOSTOP | SND_FILENAME) == 0)
				;	// Check Playing?
			sndPlaySound(NULL, SND_ASYNC | SND_FILENAME);// Stop Current Sound
			waveOutSetVolume(NULL, OrgVolume);
			return TRUE;
		}
	}
	return FALSE;
}
//
//	Directry Sheet Proc
//
static BOOL CALLBACK OptionDirProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
TCHAR	FullFileName[MAX_PATH], FileName[MAX_PATH];	// File Name Buffer
OPENFILENAME	FN;	// File Open Structure
	switch(message)
	{
//	Init Style DialogBox
	case WM_INITDIALOG:
//	Set Edit Box
		SetDlgItemText(hDlg, IDC_EXECNAME, szExecFileName);
		SetDlgItemText(hDlg, IDC_LEFTKEY_DEF, szDefNameL);		// [iwad]
		SetDlgItemText(hDlg, IDC_LEFTKEY_OPEN, szOpenNameL);	// [iwad]
		SetDlgItemText(hDlg, IDC_RIGHTKEY_DEF, szDefNameR);		// [iwad]
		SetDlgItemText(hDlg, IDC_RIGHTKEY_OPEN, szOpenNameR);	// [iwad]
#ifndef	_WIN32_WCE
		SetDlgItemText(hDlg, IDC_NOTIFYNAME, szNotifyFileName);
#endif
		return TRUE;
//	Ok Button
	case WM_NOTIFY:
		{
		LPNMHDR pnmh = (LPNMHDR )lParam;
			switch(pnmh->code)
			{
			case PSN_APPLY:
				GetDlgItemText(hDlg, IDC_EXECNAME, szExecFileName, MAX_PATH - 1);
				GetDlgItemText(hDlg, IDC_LEFTKEY_DEF, szDefNameL, MAX_PATH - 1);	// [iwad]
				GetDlgItemText(hDlg, IDC_LEFTKEY_OPEN, szOpenNameL, MAX_PATH - 1);	// [iwad]
				GetDlgItemText(hDlg, IDC_RIGHTKEY_DEF, szDefNameR, MAX_PATH - 1);	// [iwad]
				GetDlgItemText(hDlg, IDC_RIGHTKEY_OPEN, szOpenNameR, MAX_PATH - 1);	// [iwad]
#ifndef	_WIN32_WCE
				GetDlgItemText(hDlg, IDC_NOTIFYNAME, szNotifyFileName, MAX_PATH - 1);
#endif
				return TRUE;
			}
		}
		break;
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		DWORD	Index;
//	Browse File
		case IDC_BROWSEEXEC:
			Index = 0;
#ifndef	_WIN32_WCE
			goto Browse;
		case IDC_BROWSENOTIFY:
			Index = 1;
Browse:
#endif
			if(FileOpenDialog(hDlg, 0, &FN,
				FullFileName,
				FileName,
				szCurentDir,
				TEXT("Exe File\0*.exe\0All Files\0*.*\0\0"),
				TEXT("exe")) == FALSE)				// Get Open File Name
				return TRUE;						// Cancel Pressed
			if(Index == 0)	// Exec File Mode?
			{
				_tcscpy(szExecFileName, FullFileName);// Save Current Name
				SetDlgItemText(hDlg, IDC_EXECNAME, FullFileName);
				_tcscpy(szDefNameL, FullFileName);// Save Current Name
				SetDlgItemText(hDlg, IDC_LEFTKEY_DEF, FullFileName);	// [iwad]
				_tcscpy(szOpenNameL, FullFileName);// Save Current Name
				SetDlgItemText(hDlg, IDC_LEFTKEY_OPEN, FullFileName);	// [iwad]
				_tcscpy(szDefNameR, FullFileName);// Save Current Name
				SetDlgItemText(hDlg, IDC_RIGHTKEY_DEF, FullFileName);	// [iwad]
				_tcscpy(szOpenNameR, FullFileName);// Save Current Name
				SetDlgItemText(hDlg, IDC_RIGHTKEY_OPEN, FullFileName);	// [iwad]
			}
#ifndef	_WIN32_WCE
			else
			{
				_tcscpy(szNotifyFileName, FullFileName);// Save Current Name
				SetDlgItemText(hDlg, IDC_NOTIFYNAME, FullFileName);
			}
#endif
			return TRUE;
		}
	}
	return FALSE;
}
//
//	Action Sheet Proc
//
static BOOL CALLBACK OptionActionProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
DWORD	i;
	switch(message)
	{
//	Init Style DialogBox
	case WM_INITDIALOG:
//	Setup Radio Buttons
		for(i = 0; i < 3; i++)
			SendDlgItemMessage(hDlg, EscButton[i], BM_SETCHECK, (i == dwExitMode), 0);
//	Setup CheckBoxs
		SendDlgItemMessage(hDlg, IDC_BACKGROUND, BM_SETCHECK, dwMulti, 0);
		SendDlgItemMessage(hDlg, IDC_CASH, BM_SETCHECK, dwFileMode, 0);
		SendDlgItemMessage(hDlg, IDC_CONFIRM, BM_SETCHECK, dwConfirm, 0);
		SendDlgItemMessage(hDlg, IDC_MULTI, BM_SETCHECK, dwMultiConfirm, 0);
		SendDlgItemMessage(hDlg, IDC_SYNC, BM_SETCHECK, dwWeekSync, 0);
		SendDlgItemMessage(hDlg, IDC_FLOAT, BM_SETCHECK, dwWeekFloat, 0);
		SendDlgItemMessage(hDlg, IDC_GRAYBACK, BM_SETCHECK, dwGrayBack, 0);
		SendDlgItemMessage(hDlg, IDC_DATEFORMAT, BM_SETCHECK, dwDateFormat, 0);
		return TRUE;
//	Ok Button
	case WM_NOTIFY:
		{
		LPNMHDR pnmh = (LPNMHDR )lParam;
			switch(pnmh->code)
			{
			case PSN_APPLY:
//	Save Radio Button
				for(i = 0; i < 3; i++)
					if(SendDlgItemMessage(hDlg, EscButton[i], BM_GETCHECK, 0, 0))
						{dwExitMode = i; break;};
//	Save CheckBoxs
				dwMulti = SendDlgItemMessage(hDlg, IDC_BACKGROUND, BM_GETCHECK, 0, 0);
				dwFileMode = SendDlgItemMessage(hDlg, IDC_CASH, BM_GETCHECK, 0, 0);
				dwConfirm = SendDlgItemMessage(hDlg, IDC_CONFIRM, BM_GETCHECK, 0, 0);
				dwMultiConfirm = SendDlgItemMessage(hDlg, IDC_MULTI, BM_GETCHECK, 0, 0);
				dwWeekSync = SendDlgItemMessage(hDlg, IDC_SYNC, BM_GETCHECK, 0, 0);
				dwWeekFloat = SendDlgItemMessage(hDlg, IDC_FLOAT, BM_GETCHECK, 0, 0);
				dwGrayBack = SendDlgItemMessage(hDlg, IDC_GRAYBACK, BM_GETCHECK, 0, 0);
				dwDateFormat = SendDlgItemMessage(hDlg, IDC_DATEFORMAT, BM_GETCHECK, 0, 0);
				return TRUE;
			}
		}
		break;
	}
	return FALSE;
}
//
//	Defaults Sheet Proc
//
static BOOL CALLBACK OptionDefProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
DWORD	i, j;
DWORD	TimeVal;
TCHAR	TimeBuf[16];
	switch(message)
	{
//	Init Style DialogBox
	case WM_INITDIALOG:
//	Setup Radio Buttons
		SendDlgItemMessage(hDlg, IDC_WEEKVIEW, BM_SETCHECK, dwDefWeek, 0);
		SendDlgItemMessage(hDlg, IDC_MONTHVIEW, BM_SETCHECK, dwDefMonth, 0);
		SendDlgItemMessage(hDlg, IDC_DAYVIEW, BM_SETCHECK, dwDefDay, 0);
		SendDlgItemMessage(hDlg, IDC_GRAYVIEW, BM_SETCHECK, dwDefGray, 0);
		SendDlgItemMessage(hDlg, IDC_ENABLE, BM_SETCHECK, dwDefEnable, 0);
//	Setup Week Day
		for(i = 0, j = 1; i < 7; i++, j *= 2)
			if(dwHolyDay & j)
				SendDlgItemMessage(hDlg, DayID[i], BM_SETCHECK, TRUE, 0);
//	Setup EditBox
		SetDlgItemText(hDlg, IDC_DURATION, GetSimpleTimeString(dwDefLength));
		SetDlgItemInt(hDlg, IDC_LEADTIME, dwDefLeadTime, 0);// 3 Years
		return TRUE;
//	Ok Button
	case WM_NOTIFY:
		{
		LPNMHDR pnmh = (LPNMHDR )lParam;
			switch(pnmh->code)
			{
			case PSN_APPLY:
				dwDefWeek = SendDlgItemMessage(hDlg, IDC_WEEKVIEW, BM_GETCHECK, 0, 0);
				dwDefMonth = SendDlgItemMessage(hDlg, IDC_MONTHVIEW, BM_GETCHECK, 0, 0);
				dwDefDay = SendDlgItemMessage(hDlg, IDC_DAYVIEW, BM_GETCHECK, 0, 0);
				dwDefGray = SendDlgItemMessage(hDlg, IDC_GRAYVIEW, BM_GETCHECK, 0, 0);
				dwDefEnable = SendDlgItemMessage(hDlg, IDC_ENABLE, BM_GETCHECK, 0, 0);
//	Restore HolyDay Data
				for(i = 0, j= 1; i < 7; i++, j *= 2)
					if(SendDlgItemMessage(hDlg, DayID[i], BM_GETCHECK, 0, 0))
					dwHolyDay |= j;
				else
					dwHolyDay &= j ^ MAXDWORD;

				GetDlgItemText(hDlg, IDC_DURATION, TimeBuf, sizeof(TimeBuf) / sizeof(TCHAR) - 1);
				if(ConvTimeToDword(&TimeVal, TimeBuf) == FALSE)	// Get Time Hour
					dwDefLength = 64;	// 1 Hour
				else
				{
					if(TimeVal > GetTime(12, 0))	// If 12H Mode then 1:00 Returns 13:00
						TimeVal -= GetTime(12, 0);	// Adjust Simple Hour
					dwDefLength = TimeVal;	// Get Time Hour
					if(dwDefLength > GetTime(4, 0))	// Max Default Duration is 4 Hour
						dwDefLength = GetTime(4, 0);	// Force Set 4 Hour
				}
				dwDefLeadTime = GetDlgItemInt(hDlg, IDC_LEADTIME, 0, 0);// Get Number
				if(dwDefLeadTime > 240)
					dwDefLeadTime = 240;
				return TRUE;
			}
		}
	}
	return FALSE;
}
//
//	Private Sheet Proc
//
static BOOL CALLBACK OptionPrivateProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
DWORD	i, j;
DWORD	TimeVal;
TCHAR	TimeBuf[16];
	switch(message)
	{
//	Init Style DialogBox
	case WM_INITDIALOG:
//	Setup Radio Buttons
		SendDlgItemMessage(hDlg, IDC_WEEKVIEW, BM_SETCHECK, dwPDefWeek, 0);
		SendDlgItemMessage(hDlg, IDC_MONTHVIEW, BM_SETCHECK, dwPDefMonth, 0);
		SendDlgItemMessage(hDlg, IDC_DAYVIEW, BM_SETCHECK, dwPDefDay, 0);
		SendDlgItemMessage(hDlg, IDC_GRAYVIEW, BM_SETCHECK, dwPDefGray, 0);
		SendDlgItemMessage(hDlg, IDC_ENABLE, BM_SETCHECK, dwPDefEnable, 0);
//	Setup Week Day
		for(i = 0, j = 1; i < 7; i++, j *= 2)
			if(dwPHolyDay & j)
				SendDlgItemMessage(hDlg, DayID[i], BM_SETCHECK, TRUE, 0);
//	Setup EditBox
		SetDlgItemText(hDlg, IDC_DURATION, GetSimpleTimeString(dwPDefLength));
		SetDlgItemInt(hDlg, IDC_LEADTIME, dwPDefLeadTime, 0);// 3 Years
		return TRUE;
//	Ok Button
	case WM_NOTIFY:
		{
		LPNMHDR pnmh = (LPNMHDR )lParam;
			switch(pnmh->code)
			{
			case PSN_APPLY:
				dwPDefWeek = SendDlgItemMessage(hDlg, IDC_WEEKVIEW, BM_GETCHECK, 0, 0);
				dwPDefMonth = SendDlgItemMessage(hDlg, IDC_MONTHVIEW, BM_GETCHECK, 0, 0);
				dwPDefDay = SendDlgItemMessage(hDlg, IDC_DAYVIEW, BM_GETCHECK, 0, 0);
				dwPDefGray = SendDlgItemMessage(hDlg, IDC_GRAYVIEW, BM_GETCHECK, 0, 0);
				dwPDefEnable = SendDlgItemMessage(hDlg, IDC_ENABLE, BM_GETCHECK, 0, 0);
//	Restore HolyDay Data
				for(i = 0, j= 1; i < 7; i++, j *= 2)
					if(SendDlgItemMessage(hDlg, DayID[i], BM_GETCHECK, 0, 0))
					dwPHolyDay |= j;
				else
					dwPHolyDay &= j ^ MAXDWORD;

				GetDlgItemText(hDlg, IDC_DURATION, TimeBuf, sizeof(TimeBuf) / sizeof(TCHAR) - 1);
				if(ConvTimeToDword(&TimeVal, TimeBuf) == FALSE)	// Get Time Hour
					dwDefLength = 64;	// 1 Hour
				else
				{
					if(TimeVal > GetTime(12, 0))	// If 12H Mode then 1:00 Returns 13:00
						TimeVal -= GetTime(12, 0);	// Adjust Simple Hour
					dwPDefLength = TimeVal;	// Get Time Hour
					if(dwPDefLength > GetTime(4, 0))	// Max Default Duration is 4 Hour
						dwPDefLength = GetTime(4, 0);	// Force Set 4 Hour
				}
				dwPDefLeadTime = GetDlgItemInt(hDlg, IDC_LEADTIME, 0, 0);// Get Number
				if(dwPDefLeadTime > 240)
					dwPDefLeadTime = 240;
				return TRUE;
			}
		}
	}
	return FALSE;
}
//
//	ToDo Sheet Proc
//
static BOOL CALLBACK OptionToDoProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
//	Init Style DialogBox
	case WM_INITDIALOG:
//	Setup Check Box
		SendDlgItemMessage(hDlg, IDC_WEEKVIEW, BM_SETCHECK, dwTDefWeek, 0);
		SendDlgItemMessage(hDlg, IDC_MONTHVIEW, BM_SETCHECK, dwTDefMonth, 0);
		SendDlgItemMessage(hDlg, IDC_DAYVIEW, BM_SETCHECK, dwTDefDay, 0);
		SendDlgItemMessage(hDlg, IDC_GRAYVIEW, BM_SETCHECK, dwTDefGray, 0);
		SendDlgItemMessage(hDlg, IDC_TODODAYDISP, BM_SETCHECK, dwToDoDay, 0);
		SendDlgItemMessage(hDlg, IDC_TODOHALFDISP, BM_SETCHECK, dwToDoHalf, 0);
		SendDlgItemMessage(hDlg, IDC_TODOSORT, BM_SETCHECK, dwToDoSort, 0);
		SendDlgItemMessage(hDlg, IDC_POPUP, BM_SETCHECK, dwToDoPopUp, 0);
//	Setup EditBox
		SetDlgItemInt(hDlg, IDC_RANGE, dwToDoRange, 0);// 3 Years
		return TRUE;
//	Ok Button
	case WM_NOTIFY:
		{
		LPNMHDR pnmh = (LPNMHDR )lParam;
			switch(pnmh->code)
			{
			case PSN_APPLY:
				dwTDefWeek = SendDlgItemMessage(hDlg, IDC_WEEKVIEW, BM_GETCHECK, 0, 0);
				dwTDefMonth = SendDlgItemMessage(hDlg, IDC_MONTHVIEW, BM_GETCHECK, 0, 0);
				dwTDefDay = SendDlgItemMessage(hDlg, IDC_DAYVIEW, BM_GETCHECK, 0, 0);
				dwTDefGray = SendDlgItemMessage(hDlg, IDC_GRAYVIEW, BM_GETCHECK, 0, 0);
//	Restore Range
				dwToDoRange = GetDlgItemInt(hDlg, IDC_RANGE, 0, 0);// Get Number
//	Restore CheckBox
				dwToDoDay = SendDlgItemMessage(hDlg, IDC_TODODAYDISP, BM_GETCHECK, 0, 0);
				dwToDoHalf = SendDlgItemMessage(hDlg, IDC_TODOHALFDISP, BM_GETCHECK, 0, 0);
				dwToDoSort = SendDlgItemMessage(hDlg, IDC_TODOSORT, BM_GETCHECK, 0, 0);
				dwToDoPopUp = SendDlgItemMessage(hDlg, IDC_POPUP, BM_GETCHECK, 0, 0);
				return TRUE;
			}
		}
	}
	return FALSE;
}
//
//	Color Sheet Proc
//
static BOOL CALLBACK OptionColorProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
DWORD	i, j;
DWORD	EditEnable, TempColor;
TCHAR	ValuePtr[16];
	switch(message)
	{
//	Init Style DialogBox
	case WM_INITDIALOG:
//	Setup Check Box
		for(i = 0; i < 4; i++)
		{
			EditEnable = ((*(DWORD *)ColorVar[i][1]) != 0);
			SendDlgItemMessage(hDlg, ColorVar[i][0], BM_SETCHECK, !EditEnable, 0);
			for(j = 0; j < 3; j++)
			{
				if(EditEnable)
				{
					wsprintf(ValuePtr, TEXT("%d"), (*(DWORD *)ColorVar[i][1] / ColorShift[j]) & 255);
					SetDlgItemText(hDlg, ColorValue[i][j], ValuePtr);
				}
				EnableWindow(GetDlgItem(hDlg, ColorValue[i][j]), EditEnable);// Gray Size Box if Auto
			}
		}
//	Setup EditBox
		return TRUE;
	case WM_COMMAND:
		switch(GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDC_BACKDEF:
		case IDC_GRAYDEF:
		case IDC_TEXTDEF:
		case IDC_LINEDEF:
			for(i = 0; i < 4; i++)
			{
				EditEnable = SendDlgItemMessage(hDlg, ColorVar[i][0], BM_GETCHECK, 0, 0);
				for(j = 0; j < 3; j++)
					EnableWindow(GetDlgItem(hDlg, ColorValue[i][j]), !EditEnable);// Gray Size Box if Auto
			}
			break;
		}
		return TRUE;
//	Ok Button
	case WM_NOTIFY:
		{
		LPNMHDR pnmh = (LPNMHDR )lParam;
			switch(pnmh->code)
			{
			case PSN_APPLY:
				for(i = 0; i < 4; i++)
				{
					EditEnable = !SendDlgItemMessage(hDlg, ColorVar[i][0], BM_GETCHECK, 0, 0);
					if(EditEnable)
					{
						TempColor = 0;
						for(j = 0; j < 3; j++)
							TempColor |= GetDlgItemInt(hDlg, ColorValue[i][j], NULL, 0) * ColorShift[j];
						*((DWORD *)ColorVar[i][1]) = TempColor;
					}
					else
						*((DWORD *)ColorVar[i][1]) = 0;
				}
				SelectObject(hMemDC, (HPEN )GetStockObject(BLACK_PEN));
				SelectObject(hMemDC, (HPEN )GetStockObject(WHITE_BRUSH));
				if(hPen)
					DeleteObject(hPen);
				if(hBrush)
					DeleteObject(hBrush);
				if(hGrayBrush)
					DeleteObject(hGrayBrush);
				hPen = NULL;
				hBrush = NULL;
				hGrayBrush = NULL;
				SetupBrush(hMemDC);
				InvalidateRect(hMainWnd, NULL, FALSE);
				return TRUE;
			}
		}
	}
	return FALSE;
}
//
//	[iwad] About Proc - アバウトのバージョン情報の表示
//
static BOOL CALLBACK OptionAboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR		szVerBuf[64];	// [iwad] Buffer for szVerNum

	switch(message)
	{
//	Init About DialogBox
	case WM_INITDIALOG:
//	Setup Version Number
		wsprintf(szVerBuf, TEXT("Oyajin Appd Ver %s\n2007 Modified by いわだら"), szVerNum);
		SetDlgItemText(hDlg, IDC_VERSION, szVerBuf);
		return TRUE;
	}
	return FALSE;
}

