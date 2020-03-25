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
#ifdef		_WIN32_WCE
#include	<Notify.h>	// Only for Windows CE
#endif
//
//	ProtTypes
//
DWORD	OyajiSearchRecord(FILEWORKS *);
BOOL	OyajiGetNextRecord(FILEWORKS *);
void	GetToday(void);
void	SetupBackWorks(void);
void	GetPrevDate(LPDWORD);
void	GetNextDate(LPDWORD);
DWORD	GetDate(DWORD, DWORD, DWORD);
DWORD	GetTime(DWORD, DWORD);
DWORD	AddTime(DWORD, DWORD);
DWORD	SubTime(DWORD, DWORD);
DWORD	ConvMinToTime(DWORD);
DWORD	ConvDateToWeekDay(DWORD);
void	GetUniCode(char *, LPTSTR);
//
//	Works
//

//
//	RunAppAtTime for Windows95
//
#ifndef	_WIN32_WCE
void RunAppAtTime(LPTSTR ExecName, SYSTEMTIME *Stime, DWORD Kill)
{
TCHAR	Arg[MAX_PATH];
TCHAR	Hex[128];
TCHAR	HexBuf[8];
DWORD	i;
STARTUPINFO	Info;
PROCESS_INFORMATION	Stat;
unsigned char *Src;
	if(szNotifyFileName[0] == TEXT('\0'))
		return;
	memset(&Info, 0, sizeof(Info));
	memset(&Stat, 0, sizeof(Stat));
	_tcscpy(Arg, szNotifyFileName);	// Set CallBack Program Name
	if(Kill == 0)
	{	// No Kill Mode then Set Args
		_tcscat(Arg, TEXT(" \""));	// Set Delmitter
		_tcscat(Arg, ExecName);	// Set CallBack Program Name
		_tcscat(Arg, TEXT("\" "));	// Set Delmitter
		Src = (unsigned char *)Stime;	// Set Source Pointer
		Hex[0] = TEXT('\0');	// Set "
		for(i = 0; i < sizeof(SYSTEMTIME); i++)
		{
			wsprintf(HexBuf, "%2.2X", *Src++);	// Get SYSTEMTIME Data
			_tcscat(Hex, HexBuf);	// Add Hex Digit
		}
		_tcscat(Arg, Hex);	// Add Args
	}
	Info.cb = sizeof(STARTUPINFO);
	CreateProcess(NULL, Arg, NULL, NULL, FALSE,
				CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS, NULL, NULL,
				&Info, &Stat);
	CloseHandle(Stat.hProcess);
	CloseHandle(Stat.hThread);
}
#endif
//
//	Search Next Alarm Record Thread
//
static BOOL SearchAlarm(void)
{
DWORD	CurTime;
	if(MainWork.FileHandle == NULL)
		return FALSE;
	GetToday();	// Get Time Now
	CurTime = GetTime(dwTodayHour, dwTodayMinute);	// Get Current Time
	memcpy(&BackWork, &MainWork, sizeof(FILEWORKS));	// Copy Works
	BackWork.FieldList = BackField;	// Data Base Field Definition List for BackGround
	BackDate = GetDate(dwTodayYear, dwTodayMonth, dwTodayDay);// Get Today
	GetPrevDate(&BackDate);
	BackStartTime = GetTime(24, 0);// Search From Beginning of the Day
	BackAllDay = 1;		// Scan Only One Shot
	BackGenID = 0;		// Generation ID
	OyajiSearchRecord(&BackWork);	// Seek Anyway
	if(BackWork.EndOfFile)	// End of File?
		return FALSE;	// No Set Alarm
	while(OyajiGetNextRecord(&BackWork))
	{
		if(BackEnable == 0)	// Set Alarm Check Box?
			continue;	// No Continue
		AlarmDate = BackDate;	// Get Date
		AlarmTime = SubTime(BackStartTime, ConvMinToTime(BackLeadTime));
		if(AlarmTime & 0x80000000)	// Start YesterDay?
		{
			AlarmTime += GetTime(24, 0);// Get Real Time
			GetPrevDate(&AlarmDate);
		}
		if(AlarmTime >= GetTime(24, 0))	// Over Mid Night?
		{
			AlarmTime -= GetTime(24, 0);// Get Real Time
			GetNextDate(&AlarmDate);
		}
		if(AlarmDate > GetDate(dwTodayYear, dwTodayMonth, dwTodayDay))
			return TRUE;	// Alarm Found (Different Day)
		else if(AlarmDate < GetDate(dwTodayYear, dwTodayMonth, dwTodayDay))
			continue;	// Search Next Alarm (Prev Day)
		else
		{	// Same Day
			if(AlarmTime <= CurTime)	// Past?
				continue;
//			if(SubTime(AlarmTime, CurTime) < 1)// Too Shrot Alarm?
//				continue;
			return TRUE;	// Alarm Found
		}
	}
	return FALSE;	// Meet EOF then No Alarm Set
}
//
//	Kill Alarm
//
void KillAlarm(void)
{
#ifdef	_WIN32_WCE
#if	_WIN32_WCE > 199
	CeRunAppAtEvent(szExecFileName, NOTIFICATION_EVENT_NONE);
#else
	PegRunAppAtEvent(szExecFileName, NOTIFICATION_EVENT_NONE);
#endif
#else
	RunAppAtTime(NULL, NULL, TRUE);// Kill Mode
#endif
}
//
//	Setup Wakeup Time
//
static void SetupTimer(DWORD Date, DWORD Time)
{
DWORD	dow;	// Day of Week
SYSTEMTIME	SysTime;
	memset(&SysTime, 0, sizeof(SysTime));
	dow = ConvDateToWeekDay(Date);
	SysTime.wYear = (USHORT )(Date / 512 + 1950);
	SysTime.wMonth = (USHORT )((Date / 32) & 15);
	SysTime.wDay = (USHORT )(Date & 31);
	SysTime.wDayOfWeek = (USHORT )dow;
	SysTime.wHour = (USHORT )(Time / 64);
	SysTime.wMinute = (USHORT )(Time & 63);
	SysTime.wSecond = 0;
	SysTime.wMilliseconds = 0;
#ifdef	_WIN32_WCE
#if	_WIN32_WCE > 199
	CeRunAppAtTime(szExecFileName, &SysTime);
#else
	PegRunAppAtTime(szExecFileName, &SysTime);
#endif
#else
	RunAppAtTime(szExecFileName, &SysTime, FALSE);// No Killo Mode
#endif
}
//
//	Search Alarm Thread
//
void SetAlarm(void)
{
	if(szExecFileName[0] == TEXT('\0'))
	{
//		MessageBox(hMainWnd, TEXT("Set Appointment Program Name Anyway"), TEXT("Initial Setting"), MB_OK);
		MessageBox(hMainWnd, TEXT("appd.exe‚ÌÊß½–¼‚ðŽw’è‚µ‚Ä‰º‚³‚¢"), TEXT("Initial Setting"), MB_OK); // by uminchu
		return;
	}
	KillAlarm();	// Kill Current Alarm
	if(SearchAlarm() == FALSE)	// Alarm Found?
		return;		// No Alarm Set
	GetUniCode(BackTitle, AlarmTitle);	// Set Alarm Title
	if(BackLocation[0])	// Location Exists?
	{	// Add Location
	TCHAR	StrBuf[256];
		_tcscat(AlarmTitle, TEXT(" @"));
		GetUniCode(BackLocation, StrBuf);
		_tcscat(AlarmTitle, StrBuf);
	}
	SetupTimer(AlarmDate, AlarmTime);
}
//
//	Alarm Exec Sub
//
void AlarmExec(HWND hWnd)
{
PROCESS_INFORMATION	Stat;
TCHAR	Arg[MAX_PATH];
DWORD	Result;
#ifdef	_WIN32_WCE
TCHAR	ExeName[MAX_PATH];
LPTSTR	BlankPtr;
	memset(&Stat, 0, sizeof(Stat));
	_tcscpy(ExeName, &AlarmTitle[1]);		// Skip Top |
	BlankPtr = _tcschr(ExeName, TEXT(' '));	// Blank Found?
	if(BlankPtr)	// Blank Found?
	{
		*BlankPtr = TEXT('\0');		// Change Blank to \0
		_tcscpy(Arg, BlankPtr + 1);	// Copy Arg
	}
	else						// No Argments
		_tcscpy(Arg, TEXT(""));
	Result = CreateProcess(ExeName, Arg, NULL, NULL, FALSE,
				0,
				NULL, NULL,
				NULL,
				&Stat);
#else
STARTUPINFO	Info;
	memset(&Stat, 0, sizeof(Stat));
	memset(&Info, 0, sizeof(Info));
	Info.cb = sizeof(STARTUPINFO);
	_tcscpy(Arg, &AlarmTitle[1]);
	Result = CreateProcess(NULL, Arg, NULL, NULL, FALSE,
				CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS,
				NULL, NULL,
				&Info,
				&Stat);
#endif
	if(Stat.hProcess)
		CloseHandle(Stat.hProcess);
	if(Stat.hThread)
		CloseHandle(Stat.hThread);
	if(Result == FALSE)
	{
//		wsprintf(Arg, TEXT("Can't Execute %s"), &AlarmTitle[1]);
		wsprintf(Arg, TEXT("%s‚ÌŽÀs‚ÉŽ¸”s‚µ‚Ü‚µ‚½"), &AlarmTitle[1]); // by uminchu
		MessageBox(hWnd, Arg, TEXT("Alarm Execute"), MB_OK);
	}
}
//
//	Alarm Dialog Proc
//
BOOL CALLBACK AlarmDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
TCHAR	WaveName[MAX_PATH];
TCHAR	ButtonCaption[16];
static	DWORD	LoopCount;
static	DWORD	OrgVolume;
static	DWORD	TimeOut;
DWORD	VolVal;
DWORD	AddMin;
	switch(message)
	{
//	Init About DialogBox
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_ALARMTITLE, AlarmTitle);
//		wsprintf(ButtonCaption, TEXT("%d Min."), dwAlarmSnooze);
		wsprintf(ButtonCaption, TEXT("%d •ª"), dwAlarmSnooze); // by uminchu
		SetDlgItemText(hDlg, IDC_CUSTOM, ButtonCaption);
		LoopCount = 0;
		TimeOut = 0;
		SetTimer(hDlg, 2, 100, NULL);	// Activate Timer
		if(dwVolume)
		{
			waveOutGetVolume(NULL, &OrgVolume);	// Get Original Volume
			LoopCount = dwLoopCount;
//			waveOutSetVolume(NULL, dwVolume);	// Set Alarm Volume
		}
		sndPlaySound(NULL, SND_ASYNC | SND_FILENAME);// Stop Current Sound
		return TRUE;
	case WM_PAINT:
		SetForegroundWindow(hDlg);	// For Power On & Already Exist & Hide Mode
		break;
//	Ok Button
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDC_CUSTOM:
			AddMin = dwAlarmSnooze;// Add Custom Min
			goto SetSnooze;
		case IDC_5MIN:
			AddMin = 5;
SetSnooze:
			GetToday();	// Get Time Now
			AlarmTime = AddTime(GetTime(dwTodayHour, dwTodayMinute), AddMin);	// Add 5 Min
			AlarmDate = GetDate(dwTodayYear, dwTodayMonth, dwTodayDay);
			if(AlarmTime >= GetTime(24, 0))
			{
				AlarmTime -= GetTime(24, 0);
				GetNextDate(&AlarmDate);
			}
			SetupTimer(AlarmDate, AlarmTime);
			goto ExitAlarm;
		case IDOK:
EndAlarmDialog:
			SetAlarm();
ExitAlarm:
			KillTimer(hDlg, 2);	// Stop Timer
			sndPlaySound(NULL, SND_ASYNC | SND_FILENAME);// Stop Play
			waveOutSetVolume(NULL, OrgVolume);	// Restore Org Volume
			EndDialog(hDlg, TRUE);
			return TRUE;
		}
		break;
//	Timer for Loop
	case WM_TIMER:
		if(dwAlarmTime)
		{
			TimeOut++;
			if(TimeOut / 5 == dwAlarmTime)
			{
				PostMessage(hDlg, WM_COMMAND, IDOK, 0);	// Auto Exit
				return TRUE;
			}
		}
		if(LoopCount == 0 || dwVolume == 0)	// Already Finish or Mute?
			return TRUE;	// No Operation
		VolVal = ((0xffff / 10) * dwVolume);
		VolVal |= VolVal * 0x10000;
		waveOutSetVolume(NULL, VolVal);	// Set Volume
		if(dwAlarmMode == 0)	// Beep Mode?
			_tcscpy(WaveName, TEXT(""));// Default Sound
		else
		{	// Wave Mode
			_tcscpy(WaveName, szWaveDir);
			_tcscat(WaveName, szWaveFileName);
		}
		if(sndPlaySound(WaveName, SND_ASYNC | SND_NOSTOP | SND_FILENAME) == 0)	// Check Playing?
			return TRUE;	// Now Playing
		sndPlaySound(WaveName, SND_ASYNC | SND_FILENAME);// Loop Play
		LoopCount--;
		return TRUE;
//	X Button
	case WM_CLOSE:
		goto EndAlarmDialog;
	}
	return FALSE;
}
