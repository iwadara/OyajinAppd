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
DWORD	GetTime(DWORD, DWORD);
BOOL	OyajiSeekGetRecord(FILEWORKS *);
DWORD	OyajiSearchRecord(FILEWORKS *);
BOOL	OyajiGetNextRecord(FILEWORKS *);
DWORD	GetSJIS(LPTSTR, char *, DWORD);
DWORD	ConvHalfHour(DWORD);
void	JumpDate(DWORD);
void	GetNextDate(LPDWORD);
//
//	Works
//

//
//	Find Dialog Proc
//
BOOL CALLBACK FindDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
BOOL	F1, F2, F3;	// Temp Check Box Results
TCHAR	Temp[256];
DWORD	RetVal;
	switch(message)
	{
//	Init Jump DialogBox
	case WM_INITDIALOG:
//	Setup Check Box
		SendDlgItemMessage(hDlg, IDC_FINDTITLE, BM_SETCHECK, dwFindTitle, 0);
		SendDlgItemMessage(hDlg, IDC_FINDLOCATION, BM_SETCHECK, dwFindLocation, 0);
		SendDlgItemMessage(hDlg, IDC_FINDNOTE, BM_SETCHECK, dwFindNote, 0);
		SendDlgItemMessage(hDlg, IDC_FINDTEXT, EM_SETLIMITTEXT, 255, 0);
		SetDlgItemText(hDlg, IDC_FINDTEXT, szFindText);
		return TRUE;
//	Buttons
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDC_FINDNEXT:
			RetVal = 2;
			goto FindButton;
		case IDOK:
			if(GetKeyState(VK_CONTROL) < 0)
				RetVal = 2;	// Find Next (Shift Enter)
			else
				RetVal = 1;	// Find First
FindButton:
			F1 = SendDlgItemMessage(hDlg, IDC_FINDTITLE, BM_GETCHECK, 0, 0);
			F2 = SendDlgItemMessage(hDlg, IDC_FINDLOCATION, BM_GETCHECK, 0, 0);
			F3 = SendDlgItemMessage(hDlg, IDC_FINDNOTE, BM_GETCHECK, 0, 0);
			GetDlgItemText(hDlg, IDC_FINDTEXT, Temp, 255);
			if((F1 == 0 && F2 == 0 && F3 ==0) || Temp[0] == TEXT('\0'))
			{	// No check Box Checked
//				MessageBox(hDlg, TEXT("No Search Data"), TEXT("Find Error"), MB_OK);
				MessageBox(hDlg, TEXT("ŒŸõÃÞ°À‚ª‚ ‚è‚Ü‚¹‚ñ"), TEXT("Find Error"), MB_OK);	// by uminchu
				break;
			}
			dwFindTitle = F1;	// Save CheckBox Result
			dwFindLocation = F2;
			dwFindNote = F3;
			_tcscpy(szFindText, Temp);
			EndDialog(hDlg, RetVal);
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
//	Setup Current Date Time AllDay & GenID
//
void SetupCurrentPos(void)
{
	if(DispMode != 5)	// Not ToDo?
		dwDlgDate = GetCurrentDate();	// Set Date
	else	// ToDo Screen
		dwDlgDate = DayOrgDateBuf[dwDayLine];	// Set Date
	if(DispMode != 4)	// For Search
		dwGenID = 0;
	if(DispMode == 0 || DispMode == 3 || DispMode == 5)
	{
		dwDlgStartTime = DayOrgTimeBuf[dwDayLine];// Set Time
		dwGenID = DayOrgIDBuf[dwDayLine];	// Set Record ID
		dwAllDay = DayOrgAllDayBuf[dwDayLine];// Set All Day Flag
	}
	else if(DispMode == 1)
	{
		dwDlgStartTime = GetTime(dwStartTime, 0);// Set Start Time
		dwAllDay = 1;		// Set All Day Flag
	}
	else if(DispMode == 2)
	{
		dwDlgStartTime = WeekTimeBuf[dwCurX][WeekCurY];// Set Time
		dwGenID = WeekGenIDBuf[dwCurX][WeekCurY];	// Set Record ID
		dwAllDay = WeekAllDayBuf[dwCurX][WeekCurY];// Set All Day Flag
	}
}
//
//	Change String to Lower
//
static void ConvLower(char *String)
{
char	*TempPtr;
	for(TempPtr = String; *TempPtr; TempPtr++)
		if(*TempPtr >= 'A' && *TempPtr <= 'Z')
			*TempPtr += 0x20;
}
//
//	Find Proc
//
BOOL FindRecord(DWORD Action, FILEWORKS *fptr)
{
char	TempStr[256];
	GetSJIS(szFindText, TempStr, 255);	// Get 'char' Find Text
//	Change To Lower Case
	ConvLower(TempStr);
	if(Action == 2)	// Find Next Mode?
	{
		SetupCurrentPos();	// Set Current Pos
		if(DispMode == 1)	// Month Screen?
		{
			GetNextDate(&dwDlgDate);
			dwAllDay = 0;
		}
		OyajiSeekGetRecord(fptr);	// Seek Just After Current Record
	}
	else
	{
		dwDlgDate = 0;
		dwDlgStartTime = 0;
		dwGenID = 0;
		dwAllDay = 0;
		OyajiSearchRecord(fptr);	// Seek To Top Record
	}
	while(OyajiGetNextRecord(fptr))
	{
		if(DispMode == 1 && dwDlgMonthView == 0)	// Month Checked?
			continue;
		if(DispMode == 2 && dwDlgWeekView == 0)		// Week Checked?
			continue;
		if(DispMode == 3 && dwDlgDayView == 0)		// Week Checked?
			continue;
		ConvLower(lpDlgTitle);
		if(dwFindTitle && strstr(lpDlgTitle, TempStr) != NULL)
			goto Found;
		ConvLower(lpDlgLocation);
		if(dwFindLocation && strstr(lpDlgLocation, TempStr) != NULL)
			goto Found;
		ConvLower(lpDlgNote + 2);
		if(dwFindNote && strstr(lpDlgNote + 2, TempStr) != NULL)
			goto Found;
		continue;	// Search Next Record
Found:
		JumpDate(dwDlgDate);
		if(DispMode == 4)	// Input Screen?
			OyajiSeekGetRecord(fptr);	// Read Again(Cancel tolower)
		if(dwAllDay == 0)
			CurrentTime = MAXDWORD;	// All Day Schedule
		else
			CurrentTime = ConvHalfHour(dwDlgStartTime);
		return TRUE;
	}
//	Show Not Found Message
//	MessageBox(fptr->WindowHandle, TEXT("Not Found"), TEXT("Find"), MB_OK);
	MessageBox(fptr->WindowHandle, TEXT("Œ©‚Â‚©‚è‚Ü‚¹‚ñ"), TEXT("Find"), MB_OK); // by uminchu
	SetupCurrentPos();	// Set Current Pos
	return FALSE;
}
