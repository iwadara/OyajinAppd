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
BOOL	CALLBACK OyajiDateEditProc(HWND, UINT, WPARAM, LPARAM);
DWORD	GetCurrentDate(void);
LPCTSTR	GetDateString(DWORD);
BOOL	ConvDateToDword(LPDWORD, LPTSTR);
void	GetNextDate(LPDWORD);
BOOL	RemoveRecord(void);
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
void	SetupBackVars(void);
void	SetupBackWorks(void);
BOOL	SetupClipData(BOOL);
void	GetUniCode(char *, LPTSTR);
void	UnderCut(LPTSTR);
BOOL	RestoreClipData(HWND, BOOL);
BOOL	AddRecord(BOOL, BOOL);
BOOL	FileOpenDialog(HWND, BOOL, OPENFILENAME *, LPTSTR, LPTSTR, LPTSTR, LPTSTR, LPTSTR);
DWORD	GetSJIS(LPTSTR, char *, DWORD);
BOOL	isKanji(unsigned char);
//
//	Works
//
static DWORD	AfterDate;	// After Day
static DWORD	BeforeDate;	// Before Day
static DWORD	After, Before;// Check Flag
static DWORD	ButtonID[] = {IDC_BOTH, IDC_MAIN, IDC_PRIVATE, IDC_REV};
static DWORD	ToDoButtonID[] = {IDC_TODOBOTH, IDC_APP, IDC_TODO, IDC_TODOREV};
//
//	Repeat Dialog Proc
//
BOOL CALLBACK RangeDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
DWORD	i;				// Counter
TCHAR	DateBuf[16];	// Date String Buffer
switch(message)
	{
//	Init Repeat Dialog
	case WM_INITDIALOG:
//	Setup Dialog Control Text
		AfterDate = GetCurrentDate();
		BeforeDate = AfterDate;
		GetNextDate(&BeforeDate);
		SetDlgItemText(hDlg, IDC_AFTERDATE, GetDateString(AfterDate));
		SetDlgItemText(hDlg, IDC_BEFOREDATE, GetDateString(BeforeDate));// 3 Years
		EnableWindow(GetDlgItem(hDlg, IDC_AFTERDATE), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_BEFOREDATE), FALSE);
		if(lpEdit == NULL)	// Not Set Yet?
			lpEdit = (WNDPROC )GetWindowLong(GetDlgItem(hDlg, IDC_AFTERDATE), GWL_WNDPROC);// Store Edit Handler Adrs
		SetWindowLong(GetDlgItem(hDlg, IDC_AFTERDATE), GWL_WNDPROC, (LONG )OyajiDateEditProc);	// Set Hook Handler Adrs
		SetWindowLong(GetDlgItem(hDlg, IDC_BEFOREDATE), GWL_WNDPROC, (LONG )OyajiDateEditProc);	// Set Hook Handler Adrs
		dwCategory = 0;
		dwToDoCategory = 0;
		SendDlgItemMessage(hDlg, IDC_BOTH, BM_SETCHECK, TRUE, 0);
		SendDlgItemMessage(hDlg, IDC_TODOBOTH, BM_SETCHECK, TRUE, 0);
		return TRUE;
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDC_AFTER:
			EnableWindow(GetDlgItem(hDlg, IDC_AFTERDATE), SendDlgItemMessage(hDlg, IDC_AFTER, BM_GETCHECK, 0, 0));
			break;
		case IDC_BEFORE:
			EnableWindow(GetDlgItem(hDlg, IDC_BEFOREDATE), SendDlgItemMessage(hDlg, IDC_BEFORE, BM_GETCHECK, 0, 0));
			break;
		case IDOK:
			After = SendDlgItemMessage(hDlg, IDC_AFTER, BM_GETCHECK, 0, 0);
			Before = SendDlgItemMessage(hDlg, IDC_BEFORE, BM_GETCHECK, 0, 0);
			if(After == 0 && Before == 0)	// Both Blank
			{
				EndDialog(hDlg, FALSE);	// Same As Cancel
				return TRUE;
			}
			if(After)
			{
				GetDlgItemText(hDlg, IDC_AFTERDATE, DateBuf, sizeof(DateBuf) / sizeof(TCHAR) - 1);
				if(ConvDateToDword(&AfterDate, DateBuf) == FALSE)
					goto InvalidDate;
			}
			if(Before)
			{
				GetDlgItemText(hDlg, IDC_BEFOREDATE, DateBuf, sizeof(DateBuf) / sizeof(TCHAR) - 1);
				if(ConvDateToDword(&BeforeDate, DateBuf) == FALSE)
				{
InvalidDate:
//					MessageBox(hDlg, TEXT("Invalid Date"), TEXT("Range"), MB_OK);
					MessageBox(hDlg, TEXT("–³Œø‚ÈÃÞ°À‚Å‚·"), TEXT("Range"), MB_OK);	// by uminchu
					return TRUE;
				}
			}
			for(i = 0; i < 3; i++)
				if(SendDlgItemMessage(hDlg, ButtonID[i], BM_GETCHECK, 0, 0))
					break;
			dwCategory = i;
			for(i = 0; i < 3; i++)
				if(SendDlgItemMessage(hDlg, ToDoButtonID[i], BM_GETCHECK, 0, 0))
					break;
			dwToDoCategory = i;
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
//	Repeat Dialog Proc
//
BOOL CALLBACK ImportDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
DWORD	i;				// Counter
switch(message)
	{
//	Init Repeat Dialog
	case WM_INITDIALOG:
//	Setup Dialog Control Text
		dwCategory = 0;
		dwToDoCategory = 0;
		SendDlgItemMessage(hDlg, IDC_BOTH, BM_SETCHECK, TRUE, 0);
		SendDlgItemMessage(hDlg, IDC_TODOBOTH, BM_SETCHECK, TRUE, 0);
		return TRUE;
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDOK:
			for(i = 0; i < 4; i++)
				if(SendDlgItemMessage(hDlg, ButtonID[i], BM_GETCHECK, 0, 0))
					break;
			dwCategory = i;
			for(i = 0; i < 4; i++)
				if(SendDlgItemMessage(hDlg, ToDoButtonID[i], BM_GETCHECK, 0, 0))
					break;
			dwToDoCategory = i;
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
//	Test Arg Date is in Range?
//
BOOL TestRange(DWORD Date)
{
	if(RangeMode)
	{	// Spilit Mode
		if(Date < dwStartDate)
			return TRUE;
		if(Date >= dwEndDate)
			return TRUE;
		return FALSE;
	}
	else	// BetWeen Mode
		return (Date >= dwStartDate && Date < dwEndDate);
}
//
//	Setup Range Work for TestRange()
//	RangeMode	0:BetWeen
//				1:Split
void SetupRange(void)
{
	RangeMode = 0;	//	Between
	if(After && Before)
	{
		if(AfterDate >= BeforeDate)	// Includes Same Date
		{
			RangeMode = 1;	// Split Mode
			dwStartDate = BeforeDate;
			dwEndDate = AfterDate;
			return;
		}
		dwStartDate = AfterDate;
		dwEndDate = BeforeDate;
		return;
	}
	else
	{
		if(After)	// No Before
		{
			dwStartDate = AfterDate;
			dwEndDate = 0xffff;
			return;
		}
		else
		{			// No After
			dwStartDate = 0;
			dwEndDate = BeforeDate;
			return;
		}
	}
}
//
//	Import CSV Sub
//
static void ImportSub(HANDLE hFile, HWND hWnd)
{
#define	MAXINPUTBUF	8192	// Enough Long
LPSTR	StorePtr;		// char Store Ptr
char	Temp;			// Temp Char
LPTSTR	UniPtr;		// Unicode Ptr
TCHAR	UniTemp;		// Temp UnicChar
LPTSTR	CopyPtr = NULL;		// Copy Ptr
DWORD	ByteRead;		// Byte Read From File
DWORD	StoreCount;		// Total Store Count
BOOL	Inside;			// Inside "" Flag
BOOL	StringFlag;		// Inside "" Flag
LPTSTR	FieldName[256];	// Field Name List
LPTSTR	FieldList = NULL;
LPSTR	CharBuf = NULL;	// Char Line Buf
LPTSTR	UniBuf = NULL;	// Char Line Buf
DWORD	FieldMax = 0;
DWORD	FieldNum;
BOOL	FirstLine = TRUE;
	if(hFile == NULL)	// Valid File Handle?
		return;			// Exit
	CsvBuf = (LPTSTR )LocalAlloc(LMEM_FIXED, MAXINPUTBUF);
	if(CsvBuf == NULL)	// Allocated?
		return;			// Can't Allocate
//	Alloc Field Name String Area
	FieldList =(LPTSTR )LocalAlloc(LMEM_FIXED, MAXINPUTBUF);
	if(FieldList == NULL)
		goto EndOfFile;
	CharBuf =(LPSTR )LocalAlloc(LMEM_FIXED, MAXINPUTBUF);
	if(CharBuf == NULL)
		goto EndOfFile;
	UniBuf =(LPTSTR )LocalAlloc(LMEM_FIXED, MAXINPUTBUF);
	if(UniBuf == NULL)
		goto EndOfFile;
//	Repeat Untile End of File
	while(TRUE)			// Forever for Record
	{
//	Read 1 Line From CSV File (char)
		StoreCount = 0;		// Clear Stored Count
		StorePtr = CharBuf;	// Set Buffer Start
		while(TRUE)	//	1 Line Input
		{	// One Line Input
			ReadFile(hFile, &Temp, 1, &ByteRead, 0);
			if(ByteRead == 0)			// End of File?
				goto EndOfFile;
			if(Temp == '\012')		// 0ah?
				continue;				// Skip
			if(Temp == '\015')
				break;
			*StorePtr++ = Temp;
			if(++StoreCount >= MAXINPUTBUF - 2)// Max Long?
				break;
		}
		*StorePtr = '\0';	// Set EOS
		if(FirstLine)	// Just Begining Line?
		{
			GetUniCode(CharBuf, FieldList);
			StringFlag = FALSE;
			Inside = FALSE;
			for(UniPtr = FieldList; *UniPtr; UniPtr++)
			{
				UniTemp = *UniPtr;
				if(UniTemp == TEXT('"'))	// " ?
				{
					if(Inside)
						*UniPtr = TEXT('\0');
					Inside = (Inside == 0);	// Flip Inside Flag
					continue;
				}
				else if((UniTemp == TEXT('\015'))
					|| (Inside == FALSE && UniTemp == TEXT(',')))
				{
					*UniPtr = TEXT('\0');
					if(StringFlag == FALSE)
						FieldName[FieldMax++] = UniPtr;
					StringFlag = FALSE;
				}
				else if(StringFlag == FALSE)
				{
					FieldName[FieldMax++] = UniPtr;
					StringFlag = TRUE;
				}
				if(UniTemp == TEXT('\\'))// Esc Char?
				{
					UniPtr++;		// Skip 2nd Char
					continue;
				}
#ifndef	_WIN32_WCE
				if(isKanji(UniTemp))
				{
					UniPtr++;	// Skip Kanji
					continue;
				}
#endif
			}
			FirstLine = FALSE;
			continue;	// Get Next Line
		}
		GetUniCode(CharBuf, UniBuf);
		_tcscpy(CsvBuf, TEXT(""));// Clear Clip Buffer
		FieldNum = 0;
		Inside = FALSE;
		StringFlag = FALSE;
		for(UniPtr = UniBuf; *UniPtr; UniPtr++)
		{
/*			UniTemp = *UniPtr;
#ifndef	_WIN32_WCE
			if(isKanji(UniTemp))
			{
				UniPtr++;	// Skip Kanji
				continue;
			}
#endif
			if(Inside == 0 && UniTemp == TEXT(' '))// OutSide Space?
				continue;				// Skip
			else if(UniTemp == '"')	// " ?
			{
				if(Inside == FALSE)
				{
					_tcscat(CsvBuf, FieldName[FieldNum++]);
					_tcscat(CsvBuf, TEXT(":"));
					CopyPtr = UniPtr + 1;
				}// Set Field Name Ptr
				else
				{
					*UniPtr = TEXT('\0');	// Set Terminator
					_tcscat(CsvBuf, CopyPtr);
					_tcscat(CsvBuf, TEXT("\015\012"));
				}
				Inside = (Inside == 0);	// Flip Inside Flag
				continue;				// Skip
			}
			else if(UniTemp == TEXT('\\'))// Esc Char?
				UniPtr++;		// Skip 2nd Char
*/			UniTemp = *UniPtr;
			if(StringFlag == FALSE)
			{
				_tcscat(CsvBuf, FieldName[FieldNum++]);
				_tcscat(CsvBuf, TEXT(":"));
				CopyPtr = UniPtr;
				StringFlag = TRUE;
			}
#ifndef	_WIN32_WCE
			if(isKanji(UniTemp))
				UniPtr++;	// Skip Kanji
			else
#endif
			if(UniTemp == TEXT('\\'))// Esc Char?
				UniPtr++;		// Skip 2nd Char
			else if(UniTemp == TEXT('"'))	// " ?
			{
				if(Inside == FALSE)
					CopyPtr++;	// Skip "
				else
					*UniPtr = TEXT('\0');
				Inside = (Inside == 0);	// Flip Inside Flag
			}
			else if(Inside == FALSE && UniTemp == TEXT(','))
			{
				*UniPtr = TEXT('\0');
				_tcscat(CsvBuf, CopyPtr);
				_tcscat(CsvBuf, TEXT("\015\012"));
				StringFlag = FALSE;
			}
		}
		_tcscat(CsvBuf, CopyPtr);
		_tcscat(CsvBuf, TEXT("\015\012"));
		if(RestoreClipData(hWnd, TRUE) == FALSE)	// Alloc Succeeds ?
			break;
//		dwOrgDate = dwDlgDate;	// Begin from Target Day
		dwGenID = 0;	// Clear Gen ID (New Mode)
		dwDlgMode = 0;	// New Add Mode
		switch(dwCategory)
		{
		case 1:	// Force Main
			dwDlgPrivate = FALSE;
			break;
		case 2:	// Force Private
			dwDlgPrivate = TRUE;
			break;
		case 3:	// Reverse
			dwDlgPrivate = (dwDlgPrivate == FALSE);
		}
		switch(dwToDoCategory)
		{
		case 1:	// Force App
			dwDlgToDo = FALSE;
			break;
		case 2:	// Force Private
			dwDlgToDo = TRUE;
			break;
		case 3:	// Reverse
			dwDlgToDo = (dwDlgToDo == FALSE);
		}
		AddRecord(TRUE, FALSE);// All Change Mode,but Return Val ignore
	}
EndOfFile:
	if(UniBuf)
		LocalFree(UniBuf);
	if(CharBuf)
		LocalFree(CharBuf);
	if(FieldList)
		LocalFree(FieldList);
	if(CsvBuf)
	{
		LocalFree(CsvBuf);
		CsvBuf = NULL;
	}
}
//
//	Import CSV File
//
BOOL ImportFile(HWND hWnd)
{
OPENFILENAME	FN;	// File Open Structure
TCHAR	FullFileName[MAX_PATH], FileName[MAX_PATH];	// File Name Buffer
HANDLE	hFile;		// Temp File Handle
	if(FileOpenDialog(hWnd, 0, &FN,
		FullFileName, FileName, szCurentDir,
		TEXT("CSV File\0*.csv\0All Files\0*.*\0\0"), TEXT("csv")) == FALSE)
		return FALSE;
	if(DialogBox(hInst, MAKEINTRESOURCE(IDD_IMPORTDLG), hWnd, (DLGPROC )ImportDlgProc) == FALSE)
		return FALSE;
	OpenClipboard(hWnd);
	EmptyClipboard();
	CloseClipboard();
	ClipBuf = 0;
	hFile = CreateFile(FullFileName,
			GENERIC_READ,	// Read & Write
			FILE_SHARE_READ,	// No Share
			NULL,	// No Seculity Attributes
			OPEN_EXISTING,	// If File not exists then Fail
			FILE_ATTRIBUTE_NORMAL |		// Normal File
			FILE_FLAG_WRITE_THROUGH,	// Write to Cash Immidiate
			NULL);						// No Template
	if(hFile == INVALID_HANDLE_VALUE)
		return FALSE;	// Can't Create Exists
#ifdef	_WIN32_WCE
	SetCursor((HCURSOR )IDC_WAIT);
#endif
	ImportSub(hFile, hWnd);
	CloseHandle(hFile);
#ifdef	_WIN32_WCE
	SetCursor(NULL);
#endif
	return TRUE;
}
//
//	Export CSV Sub
//
static void ExportSub(HANDLE hFile)
{
int		i;
DWORD	Length, ByteWrote;
DWORD	FieldAttr;
LPTSTR	FieldList = (LPTSTR )LocalAlloc(LMEM_FIXED, 16384);
LPSTR	OutBuf = (LPSTR )LocalAlloc(LMEM_FIXED, 8192);
TCHAR	TempString[256];
TCHAR	String[256];
FIELD	*fptr;
	if(FieldList == NULL)
		return;
	if(OutBuf == NULL)
	{
		LocalFree(FieldList);
		return;
	}
	_tcscpy(FieldList, TEXT(""));	// Clear Field List
//	Out Field Names
	for(fptr = MainWork.FieldList; fptr->Attr; fptr++)
	{	// Repeat for All Field
InvalidFlow:
		FieldAttr = fptr->Attr;	// Get Attr
		if(fptr->Width == 0)	// Blank Field?
			continue;		// Skip
		switch(FieldAttr)
		{
		case FLRADIO:
		case FLCHECK:
			fptr++;	// Get Next Field
			if(fptr->Attr != FLSTATIC)	// Right Order?
				goto InvalidFlow;	// Invalid Order
		case FLSTATIC:
			GetUniCode((char *)fptr->Ptr, String);
			UnderCut(String);
			wsprintf(TempString, TEXT("\"%s\","), String);
			_tcscat(FieldList, TempString);
			break;
		}
	}
	Length = _tcslen(FieldList);
	if(Length)
		_tcscpy(FieldList + Length - 1, TEXT("\015\012"));	// Cut Last ,
	GetSJIS(FieldList, OutBuf, 8192);
	Length = strlen(OutBuf);
	WriteFile(hFile, OutBuf, Length, &ByteWrote, 0);
	LocalFree(FieldList);
	if(ByteWrote != Length)
	{
		LocalFree(OutBuf);
		return;
	}
	dwDlgStartTime = 0;// Search From Beginning of the Day
	dwAllDay = 0;		// Scan Only One Shot
	dwGenID = 0;		// Generation ID
	for(i = (RangeMode != 0); i >= 0; i--)
	{
		if(i)	// First Part of Split?
			dwDlgDate = 0;		// Top Record
		else
		{
			if(RangeMode)
				dwDlgDate = dwEndDate;	// Set Split Second Part
			else
				dwDlgDate = dwStartDate;	// Set BetWeen Start
		}
		OyajiSearchRecord(&MainWork);	// Seek Anyway
		if(MainWork.EndOfFile)	// End of File?
			return;	// End Of Remove
		while(TRUE)
		{
			if(OyajiGetNextRecord(&MainWork) == FALSE)
				return;	// EOF or Error
			if(TestRange(dwDlgDate) == FALSE)
				break;	// BetWeen Date Over
			if(
			((dwCategory == 0)
			|| (dwCategory == 1 && dwDlgPrivate == FALSE)
			|| (dwCategory == 2 && dwDlgPrivate == TRUE))
			&&
			((dwToDoCategory == 0)
			|| (dwToDoCategory == 1 && dwDlgToDo == FALSE)
			|| (dwToDoCategory == 2 && dwDlgToDo == TRUE))
			&&
			(dwDlgDate == dwOrgDate)
			)
			{	// Delete Record Found
				if(SetupClipData(TRUE) == FALSE)
					return;
				GetSJIS(ClipBuf, OutBuf, 8192);
				Length = strlen(OutBuf);
				WriteFile(hFile, OutBuf, Length, &ByteWrote, 0);
				if(ClipBuf)
					LocalFree(ClipBuf);
				// [iwad] ClipBuf == NULL;
				ClipBuf = NULL;
				if(ByteWrote != Length)	// Check Write Result
				{
					LocalFree(OutBuf);
					return;
				}
			}
		}	
	}
	if(ClipBuf)
		LocalFree(ClipBuf);
	LocalFree(OutBuf);
	// [iwad] ClipBuf == NULL;
	ClipBuf = NULL;
}
//
//	Export CSV File
//
BOOL ExportFile(HWND hWnd)
{
OPENFILENAME	FN;	// File Open Structure
TCHAR	FullFileName[MAX_PATH], FileName[MAX_PATH];	// File Name Buffer
HANDLE	hFile;		// Temp File Handle
	if(FileOpenDialog(hWnd, 1, &FN,
		FullFileName, FileName, szCurentDir,
		TEXT("CSV File\0*.csv\0All Files\0*.*\0\0"), TEXT("csv")) == FALSE)
		return FALSE;
	if(DialogBox(hInst, MAKEINTRESOURCE(IDD_RANGEDLG), hWnd, (DLGPROC )RangeDlgProc) == FALSE)
		return FALSE;
	SetupRange();
	OpenClipboard(hWnd);
	EmptyClipboard();
	CloseClipboard();
	ClipBuf = 0;
	hFile = CreateFile(FullFileName,
			GENERIC_WRITE,	// Write Only
			0,	// No Share
			NULL,	// No Seculity Attributes
			CREATE_ALWAYS,		// If File exists then Delete
			FILE_ATTRIBUTE_NORMAL,		// Normal File
			NULL);						// No Template
	if(hFile == INVALID_HANDLE_VALUE)
		return FALSE;	// Can't Create Exists
#ifdef	_WIN32_WCE
	SetCursor((HCURSOR )IDC_WAIT);
#endif
	ExportSub(hFile);
	CloseHandle(hFile);
#ifdef	_WIN32_WCE
	SetCursor(NULL);
#endif
	return TRUE;
}
//
//	Remove Records
//
static BOOL RemoveRecordSub(void)
{
int	i;
	FileResult = TRUE;	// Clear Error Flag
	BackStartTime = 0;// Search From Beginning of the Day
	BackAllDay = 0;		// Scan Only One Shot
	BackGenID = 0;		// Generation ID
	for(i = (RangeMode != 0); i >= 0; i--)
	{
		if(i)	// First Part of Split?
			BackDate = 0;		// Top Record
		else
		{
			if(RangeMode)
				BackDate = dwEndDate;	// Set Split Second Part
			else
				BackDate = dwStartDate;	// Set BetWeen Start
		}
		OyajiSearchRecord(&BackWork);	// Seek Anyway
		if(BackWork.EndOfFile)	// End of File?
			return TRUE;	// End Of Remove
		while(Abort == FALSE)
		{
			if(OyajiGetNextRecord(&BackWork) == FALSE)
				return TRUE;	// EOF or Error
			if(TestRange(BackDate) == FALSE)
				break;	// BetWeen Date Over
			if(
			((dwCategory == 0)
			|| (dwCategory == 1 && BackPrivate == FALSE)
			|| (dwCategory == 2 && BackPrivate == TRUE))
			&&
			((dwToDoCategory == 0)
			|| (dwToDoCategory == 1 && BackToDo == FALSE)
			|| (dwToDoCategory == 2 && BackToDo == TRUE))
			)
			{	// Delete Record Found
				if(OyajiDeleteRecord(&BackWork, FALSE) == FALSE)
					return FALSE;
				OyajiSearchRecord(&BackWork);	// Seek Anyway
				if(BackWork.EndOfFile)	// End of File?
					return TRUE;	// End Of Remove
			}
		}	
	}
	return TRUE;
}
//
//	Remove Record From File
//
BOOL RemoveFile()
{
	SetupRange();
	FileLock = 0;
	HideGenID = 0;	// Clear Hide ID
	SetupBackWorks();	// Setup Works for BackGround
	if(OyajiCopyFile(&MainWork, &BackWork, FALSE) == FALSE)// Create Temp File 1st
		return FALSE;
	SetupBackVars();	// Setup Vars of Backgruound
#ifdef	_WIN32_WCE
	SetCursor((HCURSOR )IDC_WAIT);
#endif
	if(RemoveRecordSub() == FALSE)	// Success?
	{
#ifdef	_WIN32_WCE
		SetCursor(NULL);
#endif
		return FALSE;
	}
	OyajiSaveFile(&MainWork, &BackWork, FALSE);// Change Result
	SetAlarm();
#ifdef	_WIN32_WCE
	SetCursor(NULL);
#endif
	return TRUE;
}
