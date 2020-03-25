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
#include	"struct.h"
//
//	Macros
//
#define	MAGIC	0x6a61794F
//
//	ProtTypes
//
BOOL	OyajiFileOpen(LPTSTR, FILEWORKS *, DWORD);
void	OyajiFileClose(FILEWORKS *);
BOOL	OyajiSeekGetRecord(FILEWORKS *);
BOOL	OyajiGetRecord(FILEWORKS *);
BOOL	OyajiGetNextRecord(FILEWORKS *);
DWORD	OyajiSearchRecord(FILEWORKS *);
DWORD	OyajiSearchRecordMemory(FILEWORKS *);
BOOL	OyajiMakeRecord(FILEWORKS *);
BOOL	OyajiReleaseRecord(FILEWORKS *);
BOOL	OyajiUndoFile(FILEWORKS *);
static BOOL	OutErrorMsg(HWND, LPTSTR, DWORD, DWORD);
static int OyajiAddRecordFile(FILEWORKS *);
static int OyajiAddRecordMemory(FILEWORKS *);
static int OyajiDelRecordFile(FILEWORKS *);
static int OyajiDelRecordMemory(FILEWORKS *);
static BOOL	MoveToSortField(FILEWORKS *, DWORD, DWORD);
static DWORD	MoveToSortFieldMemory(FILEWORKS *, DWORD);
static BOOL	CreateField(HANDLE, FILEWORKS *);
static BOOL	WriteDword(HANDLE, DWORD);
static BOOL	WriteUshort(HANDLE, DWORD);
static DWORD	GetSortKeyLen(DWORD);
static BOOL	GetFilePointer(FILEWORKS *);
static BOOL	GetDateList(FILEWORKS *);
static HANDLE	OpenTempFile(FILEWORKS *);
static BOOL	RenameTempFile(FILEWORKS *);

//
//	Open Appointment File
//
static HANDLE OpenAppFile(LPTSTR FileName)
{
DWORD	i;
HANDLE	hFile;
	for(i = 0; i < 5; i++)	// For WindowsCE Compact Flash File
		if((hFile = CreateFile(FileName,
		GENERIC_READ,	// Read & Write
		FILE_SHARE_READ,	// No Share
		NULL,	// No Seculity Attributes
		OPEN_EXISTING,	// If File not exists then Fail
		FILE_ATTRIBUTE_NORMAL |		// Normal File
		FILE_FLAG_WRITE_THROUGH,	// Write to Cash Immidiate
		NULL)) == INVALID_HANDLE_VALUE)
			Sleep(200);						// No Template
		else
			return hFile;
	return INVALID_HANDLE_VALUE;
}
//
//	Create New Appointment File
//
static HANDLE CreateAppFile(LPTSTR FileName)
{
DWORD	Headder = MAGIC;//	'Oyaj'
DWORD	Zero	= 0, i;
HANDLE	hFile;
		hFile = CreateFile(FileName,
		GENERIC_WRITE,	// Write Only
		0,	// No Share
		NULL,	// No Seculity Attributes
		CREATE_ALWAYS,		// If File exists then Delete
		FILE_ATTRIBUTE_NORMAL,		// Normal File
		NULL);						// No Template
		if(hFile == INVALID_HANDLE_VALUE)
			return INVALID_HANDLE_VALUE;	// Already Exists
//	Write Headder
		if(WriteDword(hFile, Headder) == FALSE)
			return INVALID_HANDLE_VALUE;	// Already Exists
		for(i = 0; i < 4; i++)
			if(WriteDword(hFile, 0) == FALSE)
				return INVALID_HANDLE_VALUE;	// Write Error
		return hFile;
}

//
//	Check the file is Appt File?
//
static BOOL CheckAppFile(HANDLE hFile)
{
	DWORD	Headder;
	DWORD	ByteRead;
	if(ReadFile(hFile, &Headder, 4, &ByteRead, NULL) == FALSE)// Get Top 4
		return FALSE;
	if(ByteRead != sizeof(DWORD))
		return FALSE;
	return (Headder == MAGIC);	// 'Oyaj' Magic No?
}
//
//	File Open Dialog Box Sub
//
BOOL FileOpenDialog(HWND hWnd, BOOL Mode, OPENFILENAME *FN,
			LPTSTR FullFileName,
			LPTSTR FileName,
			LPTSTR DefDir,
			LPTSTR Filter, LPTSTR DefExt)
{
	memset(FN, 0, sizeof(OPENFILENAME));	// Clear FileOpen structure
	memset(FullFileName, 0, MAX_PATH * sizeof(TCHAR));	// Clear FullPath
	memset(FileName, 0, MAX_PATH * sizeof(TCHAR));	// Clear FileTitle
	FN->lStructSize = sizeof(OPENFILENAME);	// Size of Structure
	FN->hwndOwner = hWnd;		// WND Handle of parent
	FN->lpstrFilter = Filter;// Filter Strings
	FN->nFilterIndex = 1;					// Index of Filter
	FN->lpstrFile = FullFileName;			// Full Path Buffer
	FN->nMaxFile = MAX_PATH;					// Size of Full Path Buffer
	FN->lpstrFileTitle = FileName;			// File Title Buffer
	FN->nMaxFileTitle = MAX_PATH;			// Size of File Title	
	FN->lpstrInitialDir = DefDir;			// Current Path
	FN->Flags = OFN_HIDEREADONLY;
	if(Mode == 0)	// Read Mode?
		FN->Flags |= OFN_FILEMUSTEXIST;	// File must Exists (Open Mode)
	FN->lpstrDefExt = DefExt;	// Default Extension
	if(Mode)	// Write Mode
		return GetSaveFileName(FN);// Get Export File Name
	else		// Read Mode
		return GetOpenFileName(FN);// Get Import File Name
}
//
//	File Open Procedure
//	Mode 0:Open 1:Save 2:New
//
BOOL OyajiFileOpen(LPTSTR OpenName, FILEWORKS *fptr, DWORD Mode)
{
TCHAR	FullFileName[MAX_PATH], FileName[MAX_PATH];	// File Name Buffer
OPENFILENAME	FN;	// File Open Structure
HANDLE	hTempFile;		// Temp File Handle
HWND	hWnd = fptr->WindowHandle;
	if(OpenName != NULL)
		_tcscpy(FullFileName, OpenName);
	else	// No File Specified (Ask File Name)
	{
		if(FileOpenDialog(hWnd, Mode, &FN,
			FullFileName, FileName, fptr->CurrentDir,
			fptr->Filter, TEXT("adb")) == FALSE)
			return FALSE;
	}
	if(Mode == 1)
	{
	TCHAR	OldFileName[MAX_PATH];
	HANDLE	hFile = fptr->FileHandle;
		if(hFile == NULL)
			return FALSE;	// No File Open
		_tcscpy(OldFileName, fptr->CurrentDir);
		_tcscat(OldFileName, fptr->CurrentName);
		return CopyFile(OldFileName, FullFileName, FALSE);
	}
	else if(Mode == 2)	// New File Mode?
	{
//	Create File Anyway
		hTempFile = CreateAppFile(FullFileName);// New Create Appt File
		if(hTempFile == INVALID_HANDLE_VALUE)	// Cant't Create?
			goto CreateError;
		OyajiFileClose(fptr);
		fptr->FileAge = 1;					// Clear File Generation
		memset(fptr->Password, 0, sizeof(fptr->Password));// Clear Password
		*fptr->DataBaseName = TEXT('\0');	// Clear DataBase Name
		if(CreateField(hTempFile, fptr) == FALSE)		// Create Field Definition Block
		{
			CloseHandle(hTempFile);	// Save Once
			DeleteFile(FullFileName);// Clean up
CreateError:
			OutErrorMsg(hWnd, FullFileName, 0, 0);// Can't Create (File Create)
			return FALSE;
		}
		CloseHandle(hTempFile);	// Save Once
	}
//	Open Mode
	hTempFile = OpenAppFile(FullFileName);	// Open Appt File
	if(hTempFile == INVALID_HANDLE_VALUE)	// Can't Open
	{
		OutErrorMsg(hWnd, FullFileName, 1, 1);// Not Found (File Open)
		return FALSE;
	}
	if(CheckAppFile(hTempFile) == FALSE)	// Check is it a Right File?
	{
		OutErrorMsg(hWnd, FullFileName, 2, 1);//Not Appointment (File Open)
		CloseHandle(hTempFile);	// Close File
		return FALSE;
	}
	OyajiFileClose(fptr);
	fptr->FileHandle = hTempFile;	// Set Temp File Handle
	if(GetFilePointer(fptr) == FALSE)	// Get File Seek Ptrs
		goto InvalidFile;	// Invalid Pointers
	if(GetDateList(fptr) == FALSE)	// Get Date List from Open File
	{
InvalidFile:
		OutErrorMsg(hWnd, FullFileName, 3, 1);// Invalid File (File Open)
		CloseHandle(fptr->FileHandle);
		fptr->FileHandle = NULL;
		return FALSE;
	}
	if(OpenName == NULL)
	{
		_tcscpy(fptr->CurrentName, &FullFileName[FN.nFileOffset]);// Save Current Name
		_tcscpy(fptr->CurrentDir, FullFileName);	// Save Current FileName & Path
		*(fptr->CurrentDir + FN.nFileOffset) = TEXT('\0');	// Cut Only Dir Name
	}
	return TRUE;	// Success
}
//
//	File Close Proc (to Release Memory)
//
void OyajiFileClose(FILEWORKS *fptr)
{
	if(fptr->FileHandle)
		CloseHandle(fptr->FileHandle);// Close File
	fptr->FileHandle = NULL;
	if(fptr->IndexList)
		LocalFree(fptr->IndexList);// Release Index Table Memory
	fptr->IndexList = NULL;
}
//
//	Create Field Definition Block into File
//	(Create Blank Body Record File)
//	(Write Only Field Definition & Null Body & Index)
//
static BOOL CreateField(HANDLE hFile, FILEWORKS *fptr)
{
FIELD	*rec;
DWORD	FieldCount = 0, FilePos;
DWORD	ByteWrote, TextLen, i;
	SetFilePointer(hFile, 16, NULL, FILE_BEGIN);
	WriteFile(hFile, fptr->Password, sizeof(fptr->Password), &ByteWrote, 0);// Write Password
	if(ByteWrote != sizeof(fptr->Password))
		return FALSE;
	if(WriteDword(hFile, fptr->FileAge) == FALSE)	// File Generation
		return FALSE;
	TextLen = 1;	// File Version (0:Alfa 1:Beta)
	if(WriteFile(hFile, &TextLen, 1, &ByteWrote, 0) == FALSE)
		return FALSE;
	if(ByteWrote != 1)
		return FALSE;
	if(WriteFile(hFile, &fptr->SortKeyNum, 1, &ByteWrote, 0) == FALSE)
		return FALSE;
	if(ByteWrote != 1)
		return FALSE;
	for(i = 0; i < fptr->SortKeyNum; i++)
	{
		if(WriteFile(hFile, fptr->SortKeyPos + i, 1, &ByteWrote, 0) == FALSE)
			return FALSE;
		if(ByteWrote != 1)
			return FALSE;
	}
	if(WriteDword(hFile, 0) == FALSE)// Reserve 6 Bytes
		return FALSE;
	if(WriteUshort(hFile, 0) == FALSE)
		return FALSE;
	if(fptr->DataBaseName)
	{
		TextLen = strlen(fptr->DataBaseName);
		if(WriteFile(hFile, &TextLen, 1, &ByteWrote, 0) == FALSE)
			return FALSE;
		if(ByteWrote != 1)
			return FALSE;
		if(TextLen)
		{
			if(WriteFile(hFile, fptr->DataBaseName, TextLen, &ByteWrote, 0) == FALSE)
				return FALSE;
			if(ByteWrote != TextLen)
				return FALSE;
		}
	}
//	Write Each Field definitions
	for(rec = fptr->FieldList; rec->Attr; rec++)	// End of Record Def?
	{
		if(WriteUshort(hFile, rec->MaxLen * 0x100 + rec->Attr) == FALSE)
			return FALSE;
		if(WriteUshort(hFile, rec->Left) == FALSE)
			return FALSE;
		if(WriteUshort(hFile, rec->Top) == FALSE)
			return FALSE;
		if(WriteUshort(hFile, rec->Width) == FALSE)
			return FALSE;
		if(WriteUshort(hFile, rec->Hight) == FALSE)
			return FALSE;
		if(rec->Attr == FLSTATIC && rec->Ptr)
		{
		char	*Caption = (char *)rec->Ptr;
			TextLen = strlen(Caption);
			if(WriteFile(hFile, &TextLen, 1, &ByteWrote, 0) == FALSE)
				return FALSE;
			if(ByteWrote != 1)
				return FALSE;
			if(WriteFile(hFile, Caption, TextLen, &ByteWrote, 0) == FALSE)
				return FALSE;
			if(ByteWrote != TextLen)
				return FALSE;
		}
		else
		{
			TextLen = 0;
			if(WriteFile(hFile, &TextLen, 1, &ByteWrote, 0) == FALSE)
				return FALSE;
			if(ByteWrote != 1)
				return FALSE;
		}
	}
	FilePos = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);// Get Current Pos
	if(WriteDword(hFile, 0) == FALSE)	// Write Null Body & Index Table Mark
		return FALSE;
	SetFilePointer(hFile, 4, NULL, FILE_BEGIN);	// Set Pointer Position
	if(WriteDword(hFile, 16) == FALSE)
		return FALSE;
	if(WriteDword(hFile, FilePos) == FALSE)
		return FALSE;
	if(WriteDword(hFile, FilePos + 2) == FALSE)
		return FALSE;
	return TRUE;
}
//
//	Get File Pointers
//	(Set Position & Length Info)
//
static BOOL GetFilePointer(FILEWORKS *fptr)
{
DWORD	Str, Record, Index;	// Temp Work
DWORD	NumRead;
DWORD	TextLen = 0;
HANDLE	hFile = fptr->FileHandle;
	if(hFile == NULL)
		return FALSE;
	SetFilePointer(hFile, 4, NULL, FILE_BEGIN);// Seek to Pointer Pos
	ReadFile(hFile, &Str, 4, &NumRead, 0);// Read Struct Seek Ptr
	if(NumRead != 4)
		return FALSE;// Read Error
	ReadFile(hFile, &Record, 4, &NumRead, 0);// Read Record Seek Ptr
	if(NumRead != 4)
		return FALSE;// Read Error
	ReadFile(hFile, &Index, 4, &NumRead, 0);// Read Index Seek Ptr
	if(NumRead != 4)
		return FALSE;// Read Error
	fptr->StructPtr = Str;	// Save Struct Seek Ptr
	fptr->RecordPtr = Record;// Save Record Seek Ptr
	fptr->IndexPtr = Index;	// Save Index Table Seek Ptr
	fptr->StructLen = Record - Str;	// Calc Struct Len
	fptr->RecordLen = Index - Record;// Calc Record Body Len
	fptr->IndexLen = SetFilePointer(hFile, 0, NULL, FILE_END) - Index;
	SetFilePointer(hFile, Str, NULL, FILE_BEGIN);// Seek to Pointer Pos
	ReadFile(hFile, fptr->Password, sizeof(fptr->Password), &NumRead, 0);// Read Password
	if(NumRead != sizeof(fptr->Password))
		return FALSE;// Read Error
	ReadFile(hFile, &fptr->FileAge, 4, &NumRead, 0);// Read File Generation
	if(NumRead != 4)
		return FALSE;// Read Error
	SetFilePointer(hFile, 12, NULL, FILE_CURRENT);// Seek to Pointer Pos
	ReadFile(hFile, &TextLen, 1, &NumRead, 0);// Read DataBase Name Len
	if(NumRead != 1)
		return FALSE;// Read Error
	if(TextLen)
	{
		ReadFile(hFile, fptr->DataBaseName, TextLen, &NumRead, 0);// Read Password
		if(NumRead != TextLen)
			return FALSE;// Read Error
	}
	*(fptr->DataBaseName + TextLen) = TEXT('\0');	// Set Last '\0'
	return TRUE;
}
//
//	Alloc Memory & Copy Date List from File
//	(Get Sort Index Table to Heap)
//
static BOOL GetDateList(FILEWORKS *fptr)
{
DWORD	NumRead, KeyType;// For Read Check
DWORD	*StorePtr;	// Store Pointer
DWORD	ListNum = 0;// Date Index List Total Count
HANDLE	hFile = fptr->FileHandle;
FIELD	*TempField = fptr->FieldList;	// Sort Index1
 	if(hFile == NULL)
		return FALSE;
	if(fptr->IndexList)		// Already Allocated?
		LocalFree(fptr->IndexList);	// Release Last Memory
	fptr->IndexList = NULL;	// Set not Allocated
	SetFilePointer(hFile, fptr->IndexPtr, NULL, FILE_BEGIN);// Seek to Index
	ReadFile(hFile, &ListNum, 2, &NumRead, 0);// Get Date List Count
	if(NumRead != 2)
		return FALSE;	// Read Error
	fptr->IndexCount = ListNum;	// Save Date List Table Count to Struct
	StorePtr = (DWORD *)LocalAlloc(LPTR, fptr->IndexLen + 12);// 12 byte for Scan Stop
	if(StorePtr == NULL)	// Fail to Alloc
		return FALSE;
	if(ListNum && fptr->IndexLen > 2)// If Date List Exists
	{
		ReadFile(hFile, StorePtr, fptr->IndexLen - 2, &NumRead, 0);
		if(NumRead != fptr->IndexLen - 2)// Read Error?
			{LocalFree(StorePtr); return FALSE;}// Release Memory
	}
	fptr->IndexList = (unsigned char *)StorePtr;// Save Date List Ptr to Struct
//	Set Final Record to Sort Index Table
	TempField += *(fptr->SortKeyPos);// Get 1st Sort Key Field Ptr
	KeyType = TempField->Attr;
	if(KeyType < FLQWORD)
	{	// DWORD Index List	
		StorePtr += ListNum * 2;	// Point to Final Extra Field
		*StorePtr++ = MAXDWORD;	// Indicator of Max Date(for Scan Stop)
		*StorePtr++ = fptr->RecordLen - 2;// Set Last Body Record Ptr (Last 0x0000)
	}
	else if(KeyType < FLSTRING)
	{	// QWORD Index List
		StorePtr += ListNum * 3;	// Point to Final Extra Field
		*StorePtr++ = MAXDWORD;	// Indicator of Max Date(for Scan Stop)
		*StorePtr++ = MAXDWORD;	// Indicator of Max Date(for Scan Stop)
		*StorePtr++ = fptr->RecordLen - 2;// Set Last Body Record Ptr (Last 0x0000)
	}
	else if(KeyType < FLSTATIC)
	{	// String Index List
		*StorePtr++ = 0xFF01;	// Max String Value
		*StorePtr++ = fptr->RecordLen - 2;// Set Last Body Record Ptr (Last 0x0000)
	}
	else
		return FALSE;	// Invalid Key Field Type
	return TRUE;
}
//
//	Release Record Buf
//
BOOL OyajiReleaseRecord(FILEWORKS *fptr)
{
	if(fptr->RecordBuf)
	{
		LocalFree(fptr->RecordBuf);	// Release Memory
		fptr->RecordBuf = NULL;	// Clear Ptr
		fptr->RecordBufLen = 0;	// Clear Len
	}
	return TRUE;	// Allways TRUE
}
//
//	Make 1 Record
//	(Set 1 Record Memory Image to Heap)
//
BOOL OyajiMakeRecord(FILEWORKS *fptr)
{
DWORD	RecLength;
unsigned char	*TempPtr, *Source;
DWORD	i, StoreLen, LastAttr, Temp;
DWORD	CheckBuf = 0, RadioBuf = 0;
FIELD	*rec;
	OyajiReleaseRecord(fptr);
//	Calc Total Rec Length
	RecLength = 0;
	LastAttr = 0;
	for(rec = fptr->FieldList; rec->Attr; rec++)	// End of Record Def?
	{
		switch(rec->Attr)
		{
		case FLBYTE:	// Byte
			RecLength++;
			break;
		case FLCHECK:	// Short
			if(LastAttr != FLCHECK)
				RecLength++;
			break;
		case FLRADIO:	// Radio
			if(LastAttr != FLRADIO)
				RecLength++;
			break;
		case FLSHORT:	// Short
		case FLDATE:	// Date
		case FLTIME:	// Time
			RecLength += 2;
			break;
		case FLDWORD:	// DWORD
		case FLSINGLE:	// Single
			RecLength += 4;
			break;
		case FLQWORD:	// QWORD
		case FLDOUBLE:// Double
		case FLCURRENCY:// Currency
			RecLength += 8;
		case FLSTRING:// String
		case FLDROPDOWN:// Drop Down
		case FLLISTBOX:// List Box
			RecLength += strlen((char *)rec->Ptr) + 1;
			break;
		case FLREPEAT:// Repeat Status
			RecLength += *((char *)rec->Ptr) + 1;
			break;
		case FLMULTIEDIT:// Multi String
		case FLBINARY:// General Purpose
			if(rec->Ptr == NULL)
				RecLength += 2;	// No definition
			else
				RecLength += *((unsigned short *)rec->Ptr) + 2;
			break;
		}
		if(rec->Attr != FLSTATIC && rec->Attr != FLGROUPBOX)
			LastAttr = rec->Attr;
	}
	TempPtr = (unsigned char *)LocalAlloc(LMEM_FIXED, RecLength);// Alloc Record Buffer
	if(TempPtr == NULL)		// Fail?
	{
		OutErrorMsg(fptr->WindowHandle, TEXT("Heap"), 5, 3);// Can't Allocate(Alloc Memory)
		return FALSE;	// Fail return (Record Len = 0)
	}
	fptr->RecordBuf = TempPtr;	// Save Record Buffer Ptr
	fptr->RecordBufLen = RecLength;	// Save Record Buffer Length
//	Store Records
	StoreLen = 0;
	LastAttr = 0;
	for(rec = fptr->FieldList; rec->Attr; rec++)	// End of Record Def?
	{
		if(LastAttr == FLCHECK && rec->Attr != FLSTATIC
					&& rec->Attr != FLGROUPBOX && rec->Attr != FLCHECK)
			*TempPtr++ = LOBYTE(CheckBuf);
		if(LastAttr == FLRADIO && rec->Attr != FLSTATIC
					&& rec->Attr != FLGROUPBOX && rec->Attr != FLRADIO)
			*TempPtr++ = LOBYTE(RadioBuf);
		switch(rec->Attr)
		{
		case FLBYTE:	// Byte
			*TempPtr++ = LOBYTE(*(DWORD *)rec->Ptr);
			break;
		case FLCHECK:	// Check Box
			if(LastAttr != FLCHECK)
				CheckBuf = 0;	// Clear CheckBox Buf
			if(*(DWORD *)rec->Ptr)
				CheckBuf |= rec->MaxLen;
			break;
		case FLRADIO:	// Radio Box
			if(LastAttr != FLRADIO)
				RadioBuf = 0;	// Clear CheckBox Buf
			if(*(DWORD *)rec->Ptr)
				RadioBuf |= rec->MaxLen;
			break;
		case FLSHORT:	// Short
		case FLTIME:	// Time
		case FLDATE:	// Date
			*TempPtr++ = LOBYTE(*(DWORD *)rec->Ptr);
			*TempPtr++ = HIBYTE(*(DWORD *)rec->Ptr);
			break;
		case FLDWORD:	// DWORD
		case FLSINGLE:	// Single
			Temp = *(DWORD *)rec->Ptr;
			*TempPtr++ = LOBYTE(Temp);
			*TempPtr++ = HIBYTE(Temp);
			Temp /= 0x10000;
			*TempPtr++ = LOBYTE(Temp);
			*TempPtr++ = HIBYTE(Temp);
			break;
		case FLQWORD:	// QWORD
		case FLDOUBLE:// Double
		case FLCURRENCY:// Currency
			Temp = *(DWORD *)rec->Ptr;
			*TempPtr++ = LOBYTE(Temp);
			*TempPtr++ = HIBYTE(Temp);
			Temp /= 0x10000;
			*TempPtr++ = LOBYTE(Temp);
			*TempPtr++ = HIBYTE(Temp);
			Temp = *((DWORD *)rec->Ptr + 1);
			*TempPtr++ = LOBYTE(Temp);
			*TempPtr++ = HIBYTE(Temp);
			Temp /= 0x10000;
			*TempPtr++ = LOBYTE(Temp);
			*TempPtr++ = HIBYTE(Temp);
			break;
		case FLSTRING:	// String
		case FLDROPDOWN:	// DropDown
		case FLLISTBOX:	// ListBox
			StoreLen = strlen((char *)rec->Ptr);
			*TempPtr++ = LOBYTE(StoreLen);
			Source = (unsigned char *)rec->Ptr;
			for(i = 0; i < StoreLen; i++)
				*TempPtr++ = *Source++;
			break;
		case FLREPEAT:	// Repeat Status(Byte Len)
			if(rec->Ptr == NULL)
			{
				*TempPtr++ = 0;	// Clear 1Byte
			}
			else
			{
				StoreLen = *((unsigned char *)rec->Ptr);
				Source = (unsigned char *)rec->Ptr + 1;
				*TempPtr++ = LOBYTE(StoreLen);	// Store Length
				for(i = 0; i < StoreLen; i++)
					*TempPtr++ = *Source++;
			}
			break;
		case FLMULTIEDIT:	// Val Length(Short Len)
		case FLBINARY:	// General Purpose
			if(rec->Ptr == NULL)
			{
				*TempPtr++ = 0;	// Clear Short
				*TempPtr++ = 0;
			}
			else
			{
				StoreLen = *((unsigned short *)rec->Ptr);
				Source = (unsigned char *)rec->Ptr + 2;
				*TempPtr++ = LOBYTE(StoreLen);
				*TempPtr++ = HIBYTE(StoreLen);
				for(i = 0; i < StoreLen; i++)
					*TempPtr++ = *Source++;
			}
			break;
		}
		if(rec->Attr != FLSTATIC && rec->Attr != FLGROUPBOX)
			LastAttr = rec->Attr;
	}
	if(LastAttr == FLCHECK)
		*TempPtr++ = LOBYTE(CheckBuf);
	if(LastAttr == FLRADIO)
		*TempPtr++ = LOBYTE(RadioBuf);
	return TRUE;	// Success
}
//
//	Open Temp File
//
static HANDLE OpenTempFile(FILEWORKS *fptr)
{
TCHAR	FullNewName[MAX_PATH];	// For New Temp File Name
TCHAR	FileName[MAX_PATH];	// For Temp File Name (.$$$)
TCHAR	*DotPtr;		// For File Name Dot Cut
HANDLE	hTempFile;		// Handle for Temp File
	_tcscpy(FullNewName, fptr->CurrentDir);	// Copy Curent Dir
	_tcscpy(FileName, fptr->CurrentName);	// Create Temp FIle Name
	DotPtr = _tcschr(FileName, TEXT('.'));	// Serarch Dot
	if(DotPtr)
		*DotPtr = TEXT('\0');	// Erase After Dot
	_tcscat(FileName, TEXT(".$$$"));	// New Extension
	_tcscat(FullNewName, FileName);	// Full Temp Name
	DeleteFile(FullNewName);
	hTempFile = CreateAppFile(FullNewName);	// Make Temp File
	if(hTempFile == INVALID_HANDLE_VALUE)
		goto TempCreateError;
	if(++fptr->FileAge == 0)	// Inc File Generation
		fptr->FileAge = 1;		// Set 1 (Set But 0)
	if(CreateField(hTempFile, fptr) == FALSE)		// Create Field Definition Block
	{
		CloseHandle(hTempFile);
		DeleteFile(FullNewName);
TempCreateError:
		OutErrorMsg(fptr->WindowHandle, FullNewName, 0, 2);// Can't Create (Temp File)
		return FALSE;
	}
	return hTempFile;
}
//
//	Close Temp File and Rename File
//
static BOOL RenameTempFile(FILEWORKS *fptr)
{
TCHAR	FullFileName[MAX_PATH];// For Current File Name
TCHAR	FullNewName[MAX_PATH];	// For New Temp File Name
TCHAR	FileName[MAX_PATH];	// For Temp File Name (.$$$)
TCHAR	*DotPtr;		// For File Name Dot Cut
	_tcscpy(FullFileName, fptr->CurrentDir);	// Copy Curent Dir
	_tcscpy(FileName, fptr->CurrentName);	// Create Temp FIle Name
	DotPtr = _tcschr(FileName, TEXT('.'));	// Serarch Dot
	if(DotPtr)
		*DotPtr = TEXT('\0');	// Erase After Dot
	_tcscat(FileName, TEXT(".tmp"));	// New Extension
	_tcscat(FullFileName, FileName);	// Full Temp Name
	_tcscpy(FullNewName, fptr->CurrentDir);	// Copy Curent Dir
	_tcscpy(FileName, fptr->CurrentName);	// Create Temp FIle Name
	DotPtr = _tcschr(FileName, TEXT('.'));	// Serarch Dot
	if(DotPtr)
		*DotPtr = TEXT('\0');	// Erase After Dot
	_tcscat(FileName, TEXT(".$$$"));	// New Extension
	_tcscat(FullNewName, FileName);	// Full Temp Name
	DeleteFile(FullFileName);			// Delete Old File
	if(MoveFile(FullNewName, FullFileName) == FALSE)	// Rename File
	{
		OutErrorMsg(fptr->WindowHandle, FullFileName, 4, 2);// Can't Rename (Temp File)
		return FALSE;
	}
	return OyajiFileOpen(FullFileName, fptr, 0);	// Open New File
}
//
//	Release Memory File Heap
//
void ReleaseBackMemory(FILEWORKS *fptr)
{
	if(fptr->FileHandle)
	{
		LocalFree(fptr->FileHandle);
		fptr->FileHandle = NULL;
	}
	if(fptr->IndexList)
	{
		LocalFree(fptr->IndexList);
		fptr->IndexList = NULL;
	}
}
//
//	Copy Main File to Memory or File
//
BOOL OyajiCopyFile(FILEWORKS *fptr, FILEWORKS *bptr, DWORD Mode)
{
DWORD	Len, ByteRead;
HANDLE	hFile = fptr->FileHandle;
LPVOID	CopyBuf;
	if(Mode)
	{	// Memory Mode
		if(hFile == NULL)	// Open?
			return FALSE;
//	Allocate Index Table
		bptr->FileHandle = hFile;	// Set Org File Handle (Only for Get Sort Indx Table)
		if(GetDateList(bptr) == FALSE)
			return FALSE;
//	Allocate Record Body
		Len = fptr->RecordLen;	// Get Body Length
		CopyBuf = LocalAlloc(LMEM_FIXED, Len);	// Reserve Memory
		if(CopyBuf == NULL)	// Fail?
			goto NoMemory;
		SetFilePointer(hFile, fptr->RecordPtr, NULL, FILE_BEGIN);// Seek
		ReadFile(hFile, CopyBuf, Len, &ByteRead, 0);	// Read All Record Body 
		if(ByteRead != Len)	// Read All?
		{
			LocalFree(CopyBuf);	// Release Memory
			OyajiFileClose(fptr);	// Close Main File
NoMemory:	OutErrorMsg(fptr->WindowHandle, TEXT("Heap"), 5, 3);// Can't Allocate (Alloc Memory)
			return FALSE;
		}
		bptr->FileHandle = CopyBuf;	// Set Body memory Adrs
		return TRUE;
	}
	else	// File Mode
	{
	TCHAR	OldFileName[MAX_PATH];
	TCHAR	NewFileName[MAX_PATH];
	TCHAR	FileName[MAX_PATH];
	TCHAR	*DotPtr;
		_tcscpy(OldFileName, fptr->CurrentDir);
		_tcscat(OldFileName, fptr->CurrentName);
		_tcscpy(NewFileName, fptr->CurrentDir);
		_tcscpy(FileName, fptr->CurrentName);	// Create Temp FIle Name
		DotPtr = _tcschr(FileName, TEXT('.'));	// Serarch Dot
		if(DotPtr)
			*DotPtr = TEXT('\0');			// Erase After Dot
		_tcscat(FileName, TEXT(".tmp"));	// New Extension
		_tcscat(NewFileName, FileName);		// Full Temp Name
		DeleteFile(NewFileName);
		if(CopyFile(OldFileName, NewFileName, FALSE) == FALSE)	// Copy File
		{
			OutErrorMsg(fptr->WindowHandle, NewFileName, 0, 2);// Can't Create (Temp File)
			return FALSE;
		}
		return OyajiFileOpen(NewFileName, bptr, 0);
	}
}	
//
//	Save (Restore) Temp File or Memory to Main File
//
BOOL OyajiSaveFile(FILEWORKS *fptr, FILEWORKS *bptr, DWORD Mode)
{
TCHAR	OldFileName[MAX_PATH];
TCHAR	NewFileName[MAX_PATH];
TCHAR	FileName[MAX_PATH];
TCHAR	*DotPtr;
HANDLE	hTempFile, hFile;
LPDWORD	CopyBuf;
DWORD	ByteRead;
	_tcscpy(OldFileName, fptr->CurrentDir);
	_tcscat(OldFileName, fptr->CurrentName);
	if(Mode)	// Memory Mode?
	{	// Create .tmp File
		hFile = fptr->FileHandle;	// Get Org File Handle
		_tcscpy(NewFileName, fptr->CurrentDir);
		_tcscpy(FileName, fptr->CurrentName);	// Create Temp File Name
		DotPtr = _tcschr(FileName, TEXT('.'));	// Serarch Dot
		if(DotPtr)
			*DotPtr = TEXT('\0');			// Erase After Dot
		_tcscat(FileName, TEXT(".tmp"));	// New Extension
		_tcscat(NewFileName, FileName);		// Full Temp Name
		hTempFile = CreateFile(NewFileName,
		GENERIC_WRITE,	// Write Only
		0,	// No Share
		NULL,	// No Seculity Attributes
		CREATE_ALWAYS,		// If File exists then Delete
		FILE_ATTRIBUTE_NORMAL,	// Normal File
		NULL);					// No Template
		if(hTempFile == INVALID_HANDLE_VALUE)
		{
			OutErrorMsg(fptr->WindowHandle, NewFileName, 0, 2);// Can't Create (Temp File)
			ReleaseBackMemory(bptr);
			return FALSE;
		}
		CopyBuf = (LPDWORD )LocalAlloc(LMEM_FIXED, fptr->RecordPtr);// Alloc Headder Buffer
		if(CopyBuf == NULL)	// Success?
		{
			OutErrorMsg(fptr->WindowHandle, TEXT("Heap"),5, 3);// Can't Alloc (Alloc Memory)
			goto OutFileError;
		}
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);	// Seek to Top
		ReadFile(hFile, CopyBuf, fptr->RecordPtr, &ByteRead, 0);	// Read Headder
		if(ByteRead != fptr->RecordPtr)
		{
			LocalFree(CopyBuf);
			OutErrorMsg(fptr->WindowHandle, OldFileName, 6, 2);// Can't Read (Temp File)
			OyajiFileClose(fptr);
			goto OutFileError;
		}
		*(CopyBuf + 3) = bptr->RecordLen + fptr->RecordPtr;	// Set New Index Table Ptr
		(*(CopyBuf + 6))++;	// Increase File Gen
		if(*(CopyBuf + 6) == 0)
			(*(CopyBuf + 6))++;	// Skip Zero
		WriteFile(hTempFile, CopyBuf, fptr->RecordPtr, &ByteRead, 0);	// Write Headder
		LocalFree(CopyBuf);	// Release Memory for Headder
		if(ByteRead != fptr->RecordPtr)
			goto WriteError;
		WriteFile(hTempFile, bptr->FileHandle, bptr->RecordLen, &ByteRead, 0);	// Write Body
		LocalFree(bptr->FileHandle);	// Release Memory for Headder
		bptr->FileHandle = NULL;	// Indicates Released
		if(ByteRead != bptr->RecordLen)
			goto WriteError;
		WriteFile(hTempFile, &bptr->IndexCount, 2, &ByteRead, 0);// Write Count of Sort Index Table
		if(ByteRead != 2)
			goto WriteError;
		if(bptr->IndexCount)	// Sort Key Table Exists?
		{
			WriteFile(hTempFile, bptr->IndexList, bptr->IndexCount * 8, &ByteRead, 0);	// Write New Index Table
			if(ByteRead != bptr->IndexCount * 8)
			{
WriteError:
				OutErrorMsg(fptr->WindowHandle, NewFileName, 7, 2);// Can't Write (Temp File)
OutFileError:
				ReleaseBackMemory(bptr);
				CloseHandle(hTempFile);
				DeleteFile(NewFileName);
				return FALSE;
			}
		}
		ReleaseBackMemory(bptr);
		CloseHandle(hTempFile);	// Close New File
	}
	else	// File Mode
		OyajiFileClose(bptr);	// Close Current Back File (Release Memory 1st)
	OyajiFileClose(fptr);	// Close Current Main File
	OyajiReleaseRecord(fptr);	// Release Record Memory
	_tcscpy(NewFileName, fptr->CurrentDir);
	_tcscpy(FileName, fptr->CurrentName);	// Create Temp FIle Name
	DotPtr = _tcschr(FileName, TEXT('.'));	// Serarch Dot
	if(DotPtr)
		*DotPtr = TEXT('\0');			// Erase After Dot
	_tcscat(FileName, TEXT(".prv"));	// New Extension
	_tcscat(NewFileName, FileName);		// Full Temp Name
	DeleteFile(NewFileName);			// Delete .prv File Before Rename
	if(MoveFile(OldFileName, NewFileName) == FALSE)	// Rename .abd to .prv
		goto RenameError;
	_tcscpy(NewFileName, fptr->CurrentDir);
	_tcscpy(FileName, fptr->CurrentName);	// Create Temp File Name
	DotPtr = _tcschr(FileName, TEXT('.'));	// Serarch Dot
	if(DotPtr)
		*DotPtr = TEXT('\0');			// Erase After Dot
	_tcscat(FileName, TEXT(".tmp"));	// New Extension
	_tcscat(NewFileName, FileName);		// Full Temp Name
	if(MoveFile(NewFileName, OldFileName) == FALSE)	// Rename .tmp to .adb
	{
RenameError:
		OutErrorMsg(fptr->WindowHandle, OldFileName, 4, 2);// Can't Rename (Temp File)
		return FALSE;
	}
	return OyajiFileOpen(OldFileName, fptr, 0);
}
//
//	Undo File (Swap .adb File and .prv File)
//
BOOL OyajiUndoFile(FILEWORKS *fptr)
{
TCHAR	PrvFileName[MAX_PATH];
TCHAR	TmpFileName[MAX_PATH];
TCHAR	OrgFileName[MAX_PATH];
TCHAR	FileName[MAX_PATH];
TCHAR	*DotPtr;
//	Setup File Name
	_tcscpy(OrgFileName, fptr->CurrentDir);
	_tcscat(OrgFileName, fptr->CurrentName);// Create Org File Name
	_tcscpy(PrvFileName, fptr->CurrentDir);
	_tcscpy(FileName, fptr->CurrentName);	// Create Prv File Name
	DotPtr = _tcschr(FileName, TEXT('.'));	// Serarch Dot
	if(DotPtr)
		*DotPtr = TEXT('\0');			// Erase After Dot
	_tcscat(FileName, TEXT(".prv"));	// New Extension
	_tcscat(PrvFileName, FileName);		// Full Temp Name
	_tcscpy(TmpFileName, fptr->CurrentDir);
	_tcscpy(FileName, fptr->CurrentName);	// Create Temp File Name
	DotPtr = _tcschr(FileName, TEXT('.'));	// Serarch Dot
	if(DotPtr)
		*DotPtr = TEXT('\0');			// Erase After Dot
	_tcscat(FileName, TEXT(".$$$"));	// New Extension
	_tcscat(TmpFileName, FileName);		// Full Temp Name
	DeleteFile(TmpFileName);
//	Move .prv to .$$$
	if(MoveFile(PrvFileName, TmpFileName) == FALSE)
		return FALSE;
	OyajiFileClose(fptr);
//	Move .adb to .prv
	if(MoveFile(OrgFileName, PrvFileName) == FALSE)
		return FALSE;
//	Move .$$$ to .adb
	if(MoveFile(TmpFileName, OrgFileName) == FALSE)
		return FALSE;
	return OyajiFileOpen(OrgFileName, fptr, 0);	// Open New File
}
//
//	Add Record to File
//
BOOL OyajiAddRecord(FILEWORKS *fptr, DWORD Mode)
{
	if(Mode)
		return OyajiAddRecordMemory(fptr);
	else
		return OyajiAddRecordFile(fptr);
}
//
//	Add Record (Memory Mode)
//
static BOOL OyajiAddRecordMemory(FILEWORKS *fptr)
{
DWORD	*DateSeekPtr;	// Main Date List Seek Pointer
DWORD	RecPos;			// Current File Pos
DWORD	InsertLen = fptr->RecordBufLen + 2;	// Insert Record Length
DWORD	BodyLen = fptr->RecordLen;	// Record Body Length (Get From Source File)
DWORD	Temp;			// Work Number
DWORD	ListNum;		// Total Date List Table Count
DWORD	StoreDay;		// Day of Storing
unsigned char *Source;	// Copy Ptr (Source)
unsigned char *Dest;	// Copy Ptr (Destination)
unsigned char *Target;	// Copy Ptr (Stop Compair)
DWORD	i;	// Pre Insert Record Number
DWORD	MatRes;		// Match Result
DWORD	SortKey = 0;		// Sort Key Data (Enum)
DWORD	SortKeyType;
FIELD	*SortKeyPtr;
void	*TempPtr;
HLOCAL	NewMemory;
//	Expand Memory First
	NewMemory = LocalReAlloc((HLOCAL )fptr->FileHandle, BodyLen + InsertLen, LMEM_MOVEABLE);
	if(NewMemory == NULL)	// Fail?
	{
		OutErrorMsg(fptr->WindowHandle, TEXT("Heap"), 5, 3);// Can't Allocate (Alloc Memory)
		return FALSE;
	}
	fptr->FileHandle = (HANDLE )NewMemory;
//	Get Index Key to be Inserted
	SortKeyPtr = fptr->FieldList;	// Get Field List Top
	SortKeyPtr += *fptr->SortKeyPos;	// Move to Key1 FIELD
	SortKeyType = SortKeyPtr->Attr;// Get Sort Key1 Type
	TempPtr = SortKeyPtr->Ptr;
	switch(GetSortKeyLen(SortKeyType))
	{
	case 1:
		SortKey = *(unsigned char *)TempPtr;	// Get Sort Key(Enum)
		break;
	case 2:
		SortKey = *(unsigned short *)TempPtr;	// Get Sort Key(Enum)
		break;
	case 4:
		SortKey = *(DWORD *)TempPtr;	// Get Sort Key(Enum)
	}
	MatRes = OyajiSearchRecordMemory(fptr);// Perfect Match?
	RecPos = fptr->BufferPos;	// Save Position to be Inserted
//	Insert Target Record
	Source = (unsigned char *)fptr->FileHandle + BodyLen - 1;// Get Source
	Dest = Source + InsertLen;	// Get Dest.
	Target = (unsigned char *)fptr->FileHandle + RecPos;	// Stop Comapir Ptr
	while(Source >= Target)
		*Dest-- = *Source--;	// Move (Insert) Other Record
	*Target++ = LOBYTE(InsertLen - 2);	// Set Length
	*Target++ = HIBYTE(InsertLen - 2);	// Set Length
	Source = fptr->RecordBuf;	// Set Source (Record Buf)
	for(i = 0; i < InsertLen - 2; i++)
		*Target++ = *Source++;	// Copy Target Record
//	Write Date List
	ListNum = fptr->IndexCount;
	Temp = ListNum;
	if(MatRes == 0)	// Already Exist?
	{
		Temp++;	// Add One Date List
		NewMemory = LocalReAlloc((HLOCAL )fptr->IndexList, Temp * 8 + 8, LMEM_MOVEABLE);
		if(NewMemory == NULL)	// Expand Index Table
		{
			OutErrorMsg(fptr->WindowHandle, TEXT("Heap"), 5, 3);// Can't Allocate (Alloc Memory)
			return FALSE;
		}
		fptr->IndexList = (unsigned char *)NewMemory;
	}
	fptr->IndexCount = Temp;	// Write Date Table Num
	DateSeekPtr = (DWORD *)fptr->IndexList;	// Set Date List (Memory) Seek Ptr
//	Write Before Date List
	for(i = 0; i < ListNum; i++)
	{
		if(*(DateSeekPtr + 1) >= RecPos)	// Target Day Passed?
			break;
		DateSeekPtr++; DateSeekPtr++;
	}
//	Write Current Date List
	if(MatRes == 0)	// If Brand New Date then Write New Date to Date Table
	{
		fptr->IndexLen += 8;
		Target = (unsigned char *)DateSeekPtr;
		Source = fptr->IndexList + Temp * 8 - 1;
		Dest = (unsigned char *)Source + 8;
		while(Source >= Target)
			*Dest-- = *Source--;
		*DateSeekPtr++ = SortKey;// Write Date
		*DateSeekPtr++ = RecPos;	// Write Record Pointer
	}
	for(; i < ListNum; i++)
	{
		StoreDay = *DateSeekPtr++;	// Get Storing Day
		Temp = *DateSeekPtr;
		if(StoreDay != SortKey)	// No adjust if Same Day Entry
			Temp += InsertLen;	// Shift Post Record Pos
		*DateSeekPtr++ = Temp;
	}
	*DateSeekPtr++ = MAXDWORD;	// Last Mark
	fptr->RecordLen += InsertLen;// Add Body Length
	*DateSeekPtr = fptr->RecordLen - 2;
	return TRUE;
}
//
//	Add Record (File Mode)
//
static BOOL OyajiAddRecordFile(FILEWORKS *fptr)
{
DWORD	*DateSeekPtr;	// Main Date List Seek Pointer
DWORD	ByteWrote;		// For Read & Write Check
DWORD	RecPos;			// Current File Pos
DWORD	InsertLen = fptr->RecordBufLen;	// Record Length
DWORD	BodyLen = fptr->RecordLen;	// Record Body Length (Get From Source File)
DWORD	Temp;			// Work Number
DWORD	ListNum;		// Total Date List Table Count
DWORD	StoreDay;		// Day of Storing
char	*CopyBuf;		// File Copy Buffer
DWORD	CopyLen = 0;	// Copy Length
DWORD	DelLen = 0;		// Delete Length
DWORD	DelDate = 0;	// DateList Delete Day
DWORD	SortKey = 0;		// Sort Key Data (Enum)
DWORD	i;	// Pre Insert Record Number
DWORD	MatRes;		// Match Result
DWORD	SortKeyType;
FIELD	*SortKeyPtr;
HANDLE	hTempFile;
void	*TempPtr;
HANDLE	hFile = fptr->FileHandle;
//	Setup Main File & Temp File
	if(hFile == NULL)
		return FALSE;
	SortKeyPtr = fptr->FieldList;	// Get Field List Top
	SortKeyPtr += *fptr->SortKeyPos;	// Move to Key1 FIELD
	SortKeyType = SortKeyPtr->Attr;// Get Sort Key1 Type
	TempPtr = SortKeyPtr->Ptr;
	switch(GetSortKeyLen(SortKeyType))
	{
	case 1:
		SortKey = *(unsigned char *)TempPtr;	// Get Sort Key(Enum)
		break;
	case 2:
		SortKey = *(unsigned short *)TempPtr;	// Get Sort Key(Enum)
		break;
	case 4:
		SortKey = *(DWORD *)TempPtr;	// Get Sort Key(Enum)
	}
	MatRes = OyajiSearchRecord(fptr);// Perfect Match?
	hTempFile = OpenTempFile(fptr);	// Open Temp File & Get Handle
	if(hTempFile == NULL)		// Success?
		return FALSE;
//	Move to Body Record Start
	SetFilePointer(hTempFile, fptr->RecordPtr, NULL, FILE_BEGIN);// Set to Top of File (Start of Body)
	CopyLen = SetFilePointer(hFile, 0, NULL, FILE_CURRENT) - fptr->RecordPtr;// Get Target Rel Pos
	SetFilePointer(hFile, fptr->RecordPtr, NULL, FILE_BEGIN);// Set Source File Ptr (Start of Body)
//	Insert Prev Record
	if(CopyLen)
	{	// If Prev Date Record exists then Copy Records(Different date only)
		CopyBuf	= (char *)LocalAlloc(LMEM_FIXED, CopyLen);// Allocate Copy Buffer
		if(CopyBuf == NULL)	// Memory Full?
			goto WriteError;
		ReadFile(hFile, CopyBuf, CopyLen, &ByteWrote, 0);// Read
		if(ByteWrote != CopyLen)
			{LocalFree(CopyBuf); goto ReadError;}
		WriteFile(hTempFile, CopyBuf, CopyLen, &ByteWrote, 0);// Write
		if(ByteWrote != CopyLen)
			{LocalFree(CopyBuf); goto WriteError;}
		LocalFree(CopyBuf);
	}
//	Insert Target Record
	RecPos = SetFilePointer(hTempFile, 0, NULL, FILE_CURRENT);	// Get Current Pos
	RecPos -= fptr->RecordPtr;	// Conv to Relative Seek Point
	if(WriteUshort(hTempFile, InsertLen) == FALSE)// Write Length
		goto WriteError;
	WriteFile(hTempFile, fptr->RecordBuf,
		InsertLen, &ByteWrote, 0);// Write Record Body
	if(ByteWrote != InsertLen)
		goto WriteError;
//	Insert Post Records
	CopyLen = BodyLen - CopyLen;// Total - Pre 
	CopyBuf	= (char *)LocalAlloc(LMEM_FIXED, CopyLen);// Allocate Copy Buffer
	if(CopyBuf == NULL)	// Memory Full?
		goto WriteError;
	ReadFile(hFile, CopyBuf, CopyLen, &ByteWrote, 0);// Read
	if(ByteWrote != CopyLen)
		{LocalFree(CopyBuf); goto ReadError;}
	WriteFile(hTempFile, CopyBuf, CopyLen, &ByteWrote, 0);// Write
	if(ByteWrote != CopyLen)
		{LocalFree(CopyBuf); goto WriteError;}
	LocalFree(CopyBuf);
//	Write Date List
	ListNum = fptr->IndexCount;
	Temp = ListNum;
	if(MatRes == 0)	// Already Exist?
		Temp++;	// Add One Date List
	if(WriteUshort(hTempFile, Temp) == FALSE)// Write Date Table Num
		goto WriteError;
	DateSeekPtr = (DWORD *)fptr->IndexList;	// Set Date List (Memory) Seek Ptr
//	Write Before Date List
	for(i = 0; i < ListNum; i++)
	{
		if(*(DateSeekPtr + 1) >= RecPos)	// Target Day Passed?
			break;
		WriteFile(hTempFile, DateSeekPtr, 8, &ByteWrote, 0);// Write Prev Date List
		if(ByteWrote != 8)
			goto WriteError;
		DateSeekPtr++; DateSeekPtr++;
	}
//	Write Current Date List
	if(MatRes == 0)	// If Brand New Date then Write New Date to Date Table
	{
		if(WriteDword(hTempFile, SortKey) == FALSE)// Write Date
			goto WriteError;
		if(WriteDword(hTempFile, RecPos) == FALSE)// Write Record Pointer
			goto WriteError;
	}
	for(; i < ListNum; i++)
	{
		StoreDay = *DateSeekPtr++;	// Get Storing Day
		if(WriteDword(hTempFile, StoreDay) == FALSE)// Write Prev Date
			goto WriteError;
		Temp = *DateSeekPtr++;
		if(StoreDay != SortKey)	// No adjust if Same Day Entry
			Temp += InsertLen + 2;	// Shift Post Record Pos
		if(WriteDword(hTempFile, Temp) == FALSE)// Write Prev Date Seek Ptr
			goto WriteError;
	}
//	Set New Index Seek Ptr
	SetFilePointer(hTempFile, 12, NULL, FILE_BEGIN);	// Pos of Body Length
	Temp = fptr->IndexPtr + InsertLen + 2;	// Add Body Length
	if(WriteDword(hTempFile, Temp) == FALSE)// Write New Body Length
	{
WriteError:
		CloseHandle(hTempFile);
		OutErrorMsg(fptr->WindowHandle, TEXT("Temp File"), 7, 2);// Can't Write (Temp File)
		return FALSE;
	}
	CloseHandle(hTempFile);
	OyajiFileClose(fptr);
	return RenameTempFile(fptr);	// Rename TempFile to MainFile

ReadError:
	OutErrorMsg(fptr->WindowHandle, TEXT("Temp File"), 6, 2);// Can't Read (Temp File)
	CloseHandle(hTempFile);// Close TempFile
	OyajiFileClose(fptr);	// Close Main File
	return FALSE;
}
//
//	Delete Record from File
//
BOOL OyajiDeleteRecord(FILEWORKS *fptr, DWORD Mode)
{
	if(Mode)
		return OyajiDelRecordMemory(fptr);
	else
		return OyajiDelRecordFile(fptr);
}
//
//	Delete Record (Memory Mode)
//
static BOOL OyajiDelRecordMemory(FILEWORKS *fptr)
{
DWORD	*DateSeekPtr;	// Main Date List Seek Pointer
DWORD	*DateStorePtr;	// Main Date List Seek Pointer
DWORD	DelLen;
unsigned char *Source;	// Copy Ptr (Source)
unsigned char *Dest;	// Copy Ptr (Destination)
unsigned char *EndPtr;	// Copy Ptr (Stop Compair)
DWORD	ShiftDate = MAXDWORD;// Begin Date must be Shift SeekPos
DWORD	DelDate;	// DateList Delete Day
DWORD	Target;
DWORD	ListNum;		// Total Date List Table Count
DWORD	i;	// Pre Insert Record Number
DWORD	Temp;
	if(OyajiSearchRecordMemory(fptr) != fptr->SortKeyNum)// 1 = Back (Perfect Match?)
		return TRUE;	// Specified Record Not Found = Already Deleted = TRUE
	Target = fptr->BufferPos;	// Set Relative Seek Point of Record to be Deleted
	Dest = (unsigned char *)fptr->FileHandle + fptr->BufferPos;
	DelLen = *Dest + *(Dest + 1) * 256 + 2;	// Get Delete Record Len
	Source = Dest + DelLen;	// Poit to Next Record
	EndPtr = (unsigned char *)fptr->FileHandle + fptr->RecordLen;// Set End of Body Adrs
	while(Source < EndPtr)
		*Dest++ = *Source++;	// Transfer
//	Write Date List
	ListNum = fptr->IndexCount;	// Get Sort Index Table count
	Temp = ListNum;
	DelDate = 0;	// Clear Delete Date
	DateSeekPtr = (DWORD *)fptr->IndexList;	// Set Date List (Memory) Seek Ptr
	for(i = 0; i < ListNum; i++)
	{	// Check one Record in the Day
		if(*(DateSeekPtr + 1) > Target)
		{
			ShiftDate = *DateSeekPtr;
			break;
		}
		if(*(DateSeekPtr + 1) == Target	// Check Target Record
			&& *(DateSeekPtr + 3) == (Target + DelLen))
		{// Check Target Record Contains Just one Record
			DelDate = *DateSeekPtr;	// Mark Date to be Deleteted
			Temp--;	// Decrement Num will be Wrote
			ShiftDate = *(DateSeekPtr + 2);
			break;
		}
		DateSeekPtr++; DateSeekPtr++;
	}
	DateSeekPtr = (DWORD *)fptr->IndexList;	// Set Date List (Memory) Seek Ptr
	DateStorePtr = DateSeekPtr;
//	Write After Date List
	for(i = 0; i < ListNum + 1; i++)	// +1 means Include last FFFFFFFF
	{
		if(*DateSeekPtr == DelDate)	// Marked Day?
		{
			DateSeekPtr++; DateSeekPtr++;
			continue;	// Skip Write Date
		}
		if(*DateSeekPtr < ShiftDate)// Date Before Deleted Date
		{
			*DateStorePtr++ = *DateSeekPtr++;
			*DateStorePtr++ = *DateSeekPtr++;
		}
		else
		{	// Write Record After Deleted Record(Must Shift)
			*DateStorePtr++ = *DateSeekPtr++;
			*DateStorePtr++ = *DateSeekPtr++ - DelLen;
		}
	}
	fptr->IndexCount = Temp;	
	fptr->RecordLen -= DelLen;
	return TRUE;
}
//
//	Delete Record (File Mode)
//
static BOOL OyajiDelRecordFile(FILEWORKS *fptr)
{
DWORD	*DateSeekPtr;	// Main Date List Seek Pointer
DWORD	ByteWrote;		// For Read & Write Check
DWORD	Temp;			// Work Number
DWORD	ListNum;		// Total Date List Table Count
char	*CopyBuf;		// File Copy Buffer
DWORD	CopyLen = 0;	// Copy Length
DWORD	DelLen = 0;		// Delete Length
DWORD	DelDate = 0;	// DateList Delete Day
DWORD	ShiftDate = MAXDWORD;// Begin Date must be Shift SeekPos
DWORD	i;	// Pre Insert Record Number
HANDLE	hTempFile;
HANDLE	hFile = fptr->FileHandle;
DWORD	Target;
	if(hFile == NULL)
		return FALSE;
	if(OyajiSearchRecord(fptr) != fptr->SortKeyNum)// 1 = Back (Perfect Match?)
		return TRUE;	// Specified Record Not Found = Already Deleted = TRUE
//	Setup Temp File
	hTempFile = OpenTempFile(fptr);	// Open Temp File & Get Handle
	if(hTempFile == NULL)		// Success?
		return FALSE;	// Error return
//	Move to Body Record Start
	SetFilePointer(hTempFile, fptr->RecordPtr, NULL, FILE_BEGIN);// Set Temp Ptr to Record Top
	CopyLen = SetFilePointer(hFile, 0, NULL, FILE_CURRENT) - fptr->RecordPtr;// Get Target Rel Pos
	Target = CopyLen;
	SetFilePointer(hFile, fptr->RecordPtr, NULL, FILE_BEGIN);// Set Source Ptr to Record Top
//	Insert Prev Record
	if(CopyLen)
	{	// If Prev Date Record exists then Copy Records(Different date only)
		CopyBuf	= (char *)LocalAlloc(LMEM_FIXED, CopyLen);// Allocate Copy Buffer
		if(CopyBuf == NULL)	// Memory Full?
			goto WriteError;
		ReadFile(hFile, CopyBuf, CopyLen, &ByteWrote, 0);// Read
		if(ByteWrote != CopyLen)
			{LocalFree(CopyBuf); goto ReadError;}
		WriteFile(hTempFile, CopyBuf, CopyLen, &ByteWrote, 0);// Write
		if(ByteWrote != CopyLen)
			{LocalFree(CopyBuf); goto WriteError;}
		LocalFree(CopyBuf);
	}
	ReadFile(hFile, &DelLen, 2, &ByteWrote, 0);// Get Delete Record Length
	if(ByteWrote != 2)
		goto ReadError;
	SetFilePointer(hFile, DelLen, NULL, FILE_CURRENT);// Skip Delete Record
	DelLen += 2;	// Add 2 Byte(Length)
//	Insert Post Records
	CopyLen = fptr->RecordLen - CopyLen - DelLen;// Total - Pre - Delete 
	CopyBuf	= (char *)LocalAlloc(LMEM_FIXED, CopyLen);// Allocate Copy Buffer
	if(CopyBuf == NULL)	// Memory Full?
		goto WriteError;
	ReadFile(hFile, CopyBuf, CopyLen, &ByteWrote, 0);// Read
	if(ByteWrote != CopyLen)
		{LocalFree(CopyBuf); goto ReadError;}
	WriteFile(hTempFile, CopyBuf, CopyLen, &ByteWrote, 0);// Write
	if(ByteWrote != CopyLen)
		{LocalFree(CopyBuf); goto WriteError;}
	LocalFree(CopyBuf);
//	Write Date List
	ListNum = fptr->IndexCount;	// Get Sort Index Table count
	Temp = ListNum;
	DelDate = 0;	// Clear Delete Date
	DateSeekPtr = (DWORD *)fptr->IndexList;	// Set Date List (Memory) Seek Ptr
	for(i = 0; i < ListNum; i++)
	{	// Check one Record in the Day
		if(*(DateSeekPtr + 1) > Target)
		{
			ShiftDate = *DateSeekPtr;
			break;
		}
		if(*(DateSeekPtr + 1) == Target	// Check Target Record
			&& *(DateSeekPtr + 3) == (Target + DelLen))
		{// Check Target Record Contains Just one Record
			DelDate = *DateSeekPtr;	// Mark Date to be Deleteted
			Temp--;	// Decrement Num will be Wrote
			ShiftDate = *(DateSeekPtr + 2);
			break;
		}
		DateSeekPtr++; DateSeekPtr++;
	}
	if(WriteUshort(hTempFile, Temp) == FALSE)// Write Date Table Num
		goto WriteError;
	DateSeekPtr = (DWORD *)fptr->IndexList;	// Set Date List (Memory) Seek Ptr
//	Write After Date List
	for(i = 0; i < ListNum; i++)
	{
		if(*DateSeekPtr == DelDate)	// Marked Day?
		{
			DateSeekPtr++; DateSeekPtr++;
			continue;	// Skip Write Date
		}
		if(*DateSeekPtr < ShiftDate)// Date Before Deleted Date
		{
			WriteFile(hTempFile, DateSeekPtr, 8, &ByteWrote, 0);// Write Prev Date List
			if(ByteWrote != 8)
				goto WriteError;
			DateSeekPtr++; DateSeekPtr++;
		}
		else
		{	// Write Record After Deleted Record(Must Shift)
			WriteFile(hTempFile, DateSeekPtr, 4, &ByteWrote, 0);// Write Prev Date
			if(ByteWrote != 4)
				goto WriteError;
			DateSeekPtr++;
			Temp = *DateSeekPtr++;
			Temp -= DelLen;	// Shift Post Record Pos
			if(WriteDword(hTempFile, Temp) == FALSE)// Write Prev Date Seek Ptr
				goto WriteError;
		}
	}
//	Set New Index Seek Ptr
	SetFilePointer(hTempFile, 12, NULL, FILE_BEGIN);	// Pos of Body Length
	Temp = fptr->IndexPtr - DelLen;	// Add Body Length
	if(WriteDword(hTempFile, Temp) == FALSE)// Write New Body Length
	{
WriteError:
		OutErrorMsg(fptr->WindowHandle, TEXT("Temp File"), 7, 2);// Can't Write (Temp File)
		CloseHandle(hTempFile);
		return FALSE;
	}
	CloseHandle(hTempFile);
	OyajiFileClose(fptr);
	return RenameTempFile(fptr);

ReadError:
	OutErrorMsg(fptr->WindowHandle, TEXT("Temp File"), 6, 2);// Can't Read (Temp File)
	CloseHandle(hTempFile);
	OyajiFileClose(fptr);	// Close
	return FALSE;
}
//
//	Get Memory Pos by Key
//	(For Multi Insert)
//
LPVOID OyajiGetFieldPos(FILEWORKS *fptr, DWORD Key)
{
FIELD	*rec;
unsigned char *Result;
DWORD	i, LastAttr;
DWORD	TextLen;
	if(fptr->RecordBuf == NULL)
		return NULL;
	Result = fptr->RecordBuf;	// Get Top Adrs of Record
	rec = fptr->FieldList;	// Set Field List Top
	LastAttr = 0;
	for(i = 0; i < Key; i++)	// Repeat from Head
	{
		if(rec->Attr == 0)
			break;
		else if(rec->Attr == FLBYTE)	// Byte Data?
			Result++;
		else if(rec->Attr == FLCHECK)	// Check Box?
		{
			if(LastAttr != FLCHECK)	// Continue CheckBox?
			Result++;
		}
		else if(rec->Attr == FLRADIO)	// Radio Button?
		{
			if(LastAttr != FLRADIO)	// Continue Radio Button?
			Result++;
		}
		else if(rec->Attr < FLDWORD)	// short?
			Result += 2;// Skip 2 Byte
		else if(rec->Attr < FLQWORD)	// DWORD?
			Result += 4;// Skip 8 Byte
		else if(rec->Attr < FLSTRING)	// QWORD?
			Result += 8;// Skip 8 Byte
		else if(rec->Attr < FLSTATIC	// String?
			|| rec->Attr == FLREPEAT)	// Repeat?
		{
			TextLen = *Result++;	// Get Byte Len
			if(TextLen)
				Result += TextLen;// Skip String
		}
		else if(rec->Attr > FLGROUPBOX)
		{
			TextLen = *Result++;	// Clear High Byte
			TextLen += *Result++ * 256;// Get High Byte
			if(TextLen)
				Result += TextLen;// Skip String
		}
		if(rec->Attr != FLSTATIC && rec->Attr != FLGROUPBOX)
			LastAttr = rec->Attr;	// Save Last Attribute (for CB & RB)
		rec++;
	}
	return (LPVOID )Result;
}
//
//	Set File Read Pointer to SortKey field Pos
//	0:SortKey 1 1:SortKey2
//
static BOOL MoveToSortField(FILEWORKS *fptr, DWORD Pos, DWORD Key)
{
FIELD	*rec;
DWORD	FieldNum, i, LastAttr;
DWORD	TextLen, ByteWrote;
HANDLE	hFile = fptr->FileHandle;
	if(hFile == NULL)	// Open?
		return FALSE;
	FieldNum = *(fptr->SortKeyPos + Key);	// Get Sort Key Pos
	SetFilePointer(hFile, Pos + 2, NULL, FILE_BEGIN);// Move to the Top of Field
	rec = fptr->FieldList;	// Set Field List Top
	LastAttr = 0;
	for(i = 0; i < FieldNum; i++)	// Repeat from Head
	{
		if(rec->Attr == 0)
			break;
		else if(rec->Attr == FLBYTE)	// Byte Data?
			SetFilePointer(hFile, 1, NULL, FILE_CURRENT);// Skip 1 Byte
		else if(rec->Attr == FLCHECK)	// Check Box?
		{
			if(LastAttr != FLCHECK)	// Continue CheckBox?
			SetFilePointer(hFile, 1, NULL, FILE_CURRENT);// Skip 1 Byte
		}
		else if(rec->Attr == FLRADIO)	// Radio Button?
		{
			if(LastAttr != FLRADIO)	// Continue Radio Button?
			SetFilePointer(hFile, 1, NULL, FILE_CURRENT);// Skip 1 Byte
		}
		else if(rec->Attr < FLDWORD)	// short?
			SetFilePointer(hFile, 2, NULL, FILE_CURRENT);// Skip 2 Byte
		else if(rec->Attr < FLQWORD)	// DWORD?
			SetFilePointer(hFile, 4, NULL, FILE_CURRENT);// Skip 8 Byte
		else if(rec->Attr < FLSTRING)	// QWORD?
			SetFilePointer(hFile, 8, NULL, FILE_CURRENT);// Skip 8 Byte
		else if(rec->Attr < FLSTATIC	// String?
			|| rec->Attr == FLREPEAT)	// Repeat?
		{
			TextLen = 0;	// Clear High Byte
			ReadFile(hFile, &TextLen, 1, &ByteWrote, 0);// Get String Len
			if(ByteWrote != 1)
				return FALSE;
			if(TextLen)
				SetFilePointer(hFile, TextLen, NULL, FILE_CURRENT);// Skip String
		}
		else if(rec->Attr > FLGROUPBOX)
		{
			TextLen = 0;	// Clear High Byte
			ReadFile(hFile, &TextLen, 2, &ByteWrote, 0);// Get String Len
			if(ByteWrote != 2)
				return FALSE;
			if(TextLen)
				SetFilePointer(hFile, TextLen, NULL, FILE_CURRENT);// Skip String
		}
		if(rec->Attr != FLSTATIC && rec->Attr != FLGROUPBOX)
			LastAttr = rec->Attr;	// Save Last Attribute (for CB & RB)
		rec++;
	}
	return TRUE;
}
//
//	Set Memory Pointer to SortKey field Pos
//	0:SortKey 1 1:SortKey2
//
static DWORD MoveToSortFieldMemory(FILEWORKS *fptr, DWORD Key)
{
FIELD	*rec;
DWORD	FieldNum, i, LastAttr;
DWORD	TextLen, RetPos = 0;
unsigned char *TempPtr;
	FieldNum = *(fptr->SortKeyPos + Key);	// Get Sort Key Pos
	rec = fptr->FieldList;	// Set Field List Top
	LastAttr = 0;
	for(i = 0; i < FieldNum; i++)	// Repeat from Head
	{
		if(rec->Attr == 0)
			break;
		else if(rec->Attr == FLBYTE)	// Byte Data?
			RetPos++;					// Skip 1 Byte
		else if(rec->Attr == FLCHECK)	// Check Box?
		{
			if(LastAttr != FLCHECK)		// Continue CheckBox?
			RetPos++;					// Skip 1 Byte
		}
		else if(rec->Attr == FLRADIO)	// Radio Button?
		{
			if(LastAttr != FLRADIO)		// Continue Radio Button?
			RetPos++;					// Skip 1 Byte
		}
		else if(rec->Attr < FLDWORD)	// short?
			RetPos += 2;				// Skip 2 Byte
		else if(rec->Attr < FLQWORD)	// DWORD?
			RetPos += 4;				// Skip 8 Byte
		else if(rec->Attr < FLSTRING)	// QWORD?
			RetPos += 8;				// Skip 8 Byte
		else if(rec->Attr < FLSTATIC	// String?
			|| rec->Attr == FLREPEAT)	// Repeat?
		{	// Get String Len
			TextLen = *((unsigned char *)fptr->FileHandle + fptr->BufferPos + 2 + RetPos);
			// Get String Len
			RetPos += TextLen + 1;// Skip String
		}
		else if(rec->Attr > FLGROUPBOX)
		{
			TempPtr = (unsigned char *)fptr->FileHandle + fptr->BufferPos + 2 + RetPos;
			TextLen = *TempPtr + *(TempPtr + 1) * 256;
			RetPos += TextLen + 2;// Skip String
		}
		if(rec->Attr != FLSTATIC && rec->Attr != FLGROUPBOX)
			LastAttr = rec->Attr;	// Save Last Attribute (for CB & RB)
		rec++;
	}
	return RetPos;
}
//
//	Get Sort Key Length
//
static DWORD GetSortKeyLen(DWORD type)
{
	if(type < FLSHORT)
		return 1;
	else if(type < FLDWORD)
		return 2;
	else if(type < FLQWORD)
		return 4;
	else if(type < FLSTRING)
		return 8;
	else return 0;
}
//
//	Get Next Record
//
BOOL OyajiGetNextRecord(FILEWORKS *fptr)
{
	if(fptr->FileHandle == NULL || fptr->EndOfFile)// Not Open or End of File?
		return FALSE;
	return OyajiGetRecord(fptr);
}
//
//	Seek & Get Record Data From Main File
//
BOOL OyajiSeekGetRecord(FILEWORKS *fptr)
{
	if(OyajiSearchRecord(fptr) != fptr->SortKeyNum)
		return FALSE;	// Not Found
	return OyajiGetRecord(fptr);
}
//
//	Wait For Compact Flash Ready
//
#ifdef	_WIN32_WCE
BOOL WaitFlashReady(FILEWORKS *fptr, HWND hWnd)
{
DWORD	i;
HANDLE	hFile = fptr->FileHandle;
	while(1)
	{
		for(i = 0; i < 5; i++)
			if(SetFilePointer(hFile, 0, NULL, FILE_CURRENT) != MAXDWORD)
				return TRUE;	// Success
			else
				Sleep(200);
//		if(MessageBox(hWnd, TEXT("File not Ready"), TEXT("Open Error"), MB_RETRYCANCEL) == IDRETRY)
		if(MessageBox(hWnd, TEXT("Ì§²Ù‚Ì€”õ‚ª‚Å‚«‚Ä‚¢‚Ü‚¹‚ñ"), TEXT("Open Error"), MB_RETRYCANCEL) == IDRETRY)	// by uminchu
			continue;
		else
		{
			fptr->FileHandle = NULL;// Reset File
			return FALSE;
		}
	}
}
#endif
//
//	Search Record
//
//		0:Not Found
//		1:Key 1 Match but Not Found
//		2:Key 2 Match but Not Found
//		3:Key 3 Match (Found)
//
DWORD OyajiSearchRecord(FILEWORKS *fptr)
{
DWORD	*DateSeekPtr;	// Main Date List Seek Pointer
DWORD	ByteWrote;		// For Read & Write Check
DWORD	RecPos;			// Current File Pos
DWORD	RecordLen = 0;	// Record Length
DWORD	RecordVal;	// Record Time
DWORD	ListNum;		// Total Date List Table Count
DWORD	SortKey = 0;// Sort Key Data (Enum)
DWORD	SortKeyType;	// Sort Key Length
DWORD	SortKeyLen;		// Sort Key Length (0:Str 1:Byte 2:Short 4:DWORD)
DWORD	i;	// Pre Insert Record Number
DWORD	RetVal;
FIELD	*SortKeyPtr;	// Temp Ptr to Get Sort Key
void	*TempPtr;
HANDLE	hFile = fptr->FileHandle;		// Fore Ground
HWND	hWnd = fptr->WindowHandle;
	fptr->EndOfFile = FALSE;	// Clear EOF Flag
	if(hFile == NULL)
		return FALSE;
#ifdef	_WIN32_WCE
	if(WaitFlashReady(fptr, hWnd) == FALSE)
		return FALSE;
#endif
//	Move to Body Record Start
	ListNum = fptr->IndexCount;	// Get Date List Count
//	Get Sort Key 1
	SortKeyPtr = fptr->FieldList;	// Get Field List Top
	SortKeyPtr += *fptr->SortKeyPos;	// Move to Key1 FIELD
	SortKeyType = SortKeyPtr->Attr;// Get Sort Key1 Type
	TempPtr = SortKeyPtr->Ptr;	// Get Main Sort Key Ptr(Enum)
	switch(GetSortKeyLen(SortKeyType))
	{
	case 1:
		SortKey = *(unsigned char *)TempPtr;	// Get Byte Sort Key(Enum)
		break;
	case 2:
		SortKey = *(unsigned short *)TempPtr;	// Get Short Sort Key(Enum)
		break;
	case 4:
		SortKey = *(DWORD *)TempPtr;	// Get DWORD Sort Key(Enum)
	}
//	Search Sort Index Table
	DateSeekPtr = (DWORD *)fptr->IndexList;		// Get Date List (Memory) Ptr
	for(i = 0; i < ListNum; i++)
	{	// Repeat for All Days of list
		if(*DateSeekPtr >= SortKey)	// Skip Until Current Date
			break;
		DateSeekPtr++;
		DateSeekPtr++;
	}
//	Check Date is on the List?
	if(*DateSeekPtr == MAXDWORD)
	{	// Seek to  End of Body Record (00 00)
		fptr->EndOfFile = TRUE;
		SetFilePointer(hFile, fptr->IndexPtr - 2, NULL, FILE_BEGIN);
		return 0;// Not Found
	}
	SetFilePointer(hFile, *(DateSeekPtr + 1) + fptr->RecordPtr, NULL, FILE_BEGIN);
	if(*DateSeekPtr != SortKey)
		return 0;// Not Found in Index List
//	Match Sort Key 1 From this Point
	while(TRUE)	// Forever
	{
		RetVal = 1;// Anyway The Result is [Sort Key 1 Match]
		RecPos = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);// Save Current File Pos
		ReadFile(hFile, &RecordLen, 2, &ByteWrote, 0);// Get Record Length
		if(ByteWrote != 2)
			goto ReadError;
		if(RecordLen == 0)	// End of Body Record
		{
			fptr->EndOfFile = TRUE;
			goto NotFound;
		}
		SortKeyPtr = fptr->FieldList;	// Get Field List Top
		SortKeyPtr += *fptr->SortKeyPos;	// Move to Key1 FIELD
		SortKeyType = SortKeyPtr->Attr;// Get Sort Key1 Type
		TempPtr = SortKeyPtr->Ptr;	// Get Main Sort Key Ptr(Enum)
		SortKeyLen = GetSortKeyLen(SortKeyType);
		switch(SortKeyLen)
		{
		case 1:
			SortKey = *(unsigned char *)TempPtr;	// Get Byte Sort Key(Enum)
			break;
		case 2:
			SortKey = *(unsigned short *)TempPtr;	// Get Short Sort Key(Enum)
			break;
		case 4:
			SortKey = *(DWORD *)TempPtr;	// Get DWORD Sort Key(Enum)
		}
		if(MoveToSortField(fptr, RecPos, 0) == FALSE)// Move to SortKey 1
			goto ReadError;
//	This Program is Only for ENUM Sort Key
		RecordVal = 0;	// Clear High Bytes
		ReadFile(hFile, &RecordVal, SortKeyLen, &ByteWrote, 0);// Get Date
		if(ByteWrote != SortKeyLen)
			goto ReadError;
		if(RecordVal != SortKey)	// Next Day ?
			goto NotFound;
SeekNextKey:
		SortKeyPtr = fptr->FieldList;	// Get Field List Top
		SortKeyPtr += *(fptr->SortKeyPos + RetVal);	// Move to Keyn FIELD
		SortKeyType = SortKeyPtr->Attr;// Get Sort Key Type
		TempPtr = SortKeyPtr->Ptr;	// Get Main Sort Key Ptr(Enum)
		SortKeyLen = GetSortKeyLen(SortKeyType);
		switch(SortKeyLen)
		{
		case 1:
			SortKey = *(unsigned char *)TempPtr;	// Get Byte Sort Key(Enum)
			break;
		case 2:
			SortKey = *(unsigned short *)TempPtr;	// Get Short Sort Key(Enum)
			break;
		case 4:
			SortKey = *(DWORD *)TempPtr;	// Get DWORD Sort Key(Enum)
		}
		if(MoveToSortField(fptr, RecPos, RetVal) == FALSE)//  Move to SortKey n
			goto ReadError;
		RecordVal = 0;	// Clear High Bytes
		ReadFile(hFile, &RecordVal, SortKeyLen, &ByteWrote, 0);// Get Time
		if(ByteWrote != SortKeyLen)
			goto ReadError;
		if(RecordVal < SortKey)	// Record Time Past?
			goto SeekNext;
		if(RecordVal > SortKey)	// Record Time Past?
			goto NotFound;
		RetVal++;
		if(RetVal != fptr->SortKeyNum)
			goto SeekNextKey;
NotFound:
		SetFilePointer(hFile, RecPos, NULL, FILE_BEGIN);// Seek to Top Record Pos
		return RetVal;
SeekNext:
		SetFilePointer(hFile, RecPos + RecordLen + 2, NULL, FILE_BEGIN);// Seek to Next Record
	}
ReadError:
	OutErrorMsg(fptr->WindowHandle, TEXT("Cash File"), 6, 4);// Can't Read (File Read)
	OyajiFileClose(fptr);	// Close
	return 0;	// Not Found
}
//
//	Search Record (Memory Mode)
//
//		0:Not Found
//		1:Key 1 Match but Not Found
//
DWORD OyajiSearchRecordMemory(FILEWORKS *fptr)
{
DWORD	*DateSeekPtr;	// Main Date List Seek Pointer
DWORD	RecordLen;	// Record Length
DWORD	ListNum;		// Total Date List Table Count
DWORD	SortKey = 0;		// Sort Key Data (Enum)
DWORD	SortKeyType;	// Sort Key Length
DWORD	SortKeyLen;		// Sort Key Length (0:Str 1:Byte 2:Short 4:DWORD)
DWORD	RecPos;			// Current Memory Ptr Save
DWORD	i;	// Pre Insert Record Number
DWORD	RetVal;
DWORD	RecordVal;		// Key Value
unsigned char	*MemPtr;		// Base Memory Ptr
FIELD	*SortKeyPtr;	// Temp Ptr to Get Sort Key
unsigned char	*TempPtr;
//	Move to Body Record Start
	ListNum = fptr->IndexCount;	// Get Date List Count
	MemPtr = (unsigned char *)fptr->FileHandle;	//
//	Get Sort Key 1
	SortKeyPtr = fptr->FieldList;	// Get Field List Top
	SortKeyPtr += *fptr->SortKeyPos;	// Move to Key1 FIELD
	SortKeyType = SortKeyPtr->Attr;// Get Sort Key1 Type
	TempPtr = (unsigned char *)SortKeyPtr->Ptr;	// Get Main Sort Key Ptr(Enum)
	switch(GetSortKeyLen(SortKeyType))
	{
	case 1:
		SortKey = *TempPtr;	// Get Byte Sort Key(Enum)
		break;
	case 2:
		SortKey = *(unsigned short *)TempPtr;	// Get Short Sort Key(Enum)
		break;
	case 4:
		SortKey = *(DWORD *)TempPtr;	// Get DWORD Sort Key(Enum)
	}
//	Search Sort Index Table
	DateSeekPtr = (DWORD *)fptr->IndexList;		// Get Date List (Memory) Ptr
	for(i = 0; i < ListNum; i++)
	{	// Repeat for All Days of list
		if(*DateSeekPtr >= SortKey)	// Skip Until Current Date
			break;
		DateSeekPtr++;
		DateSeekPtr++;
	}
//	Check Date is on the List?
	if(*DateSeekPtr == MAXDWORD)
	{	// Seek to  End of Body Record (00 00)
		fptr->BufferPos = fptr->RecordLen - 2;	// Set Final Record Pos (00 00)
		return 0;// Not Found
	}
	fptr->BufferPos = *(DateSeekPtr + 1);// Set Memory Seek to Day Top (Should Top of Next Day)
	if(*DateSeekPtr != SortKey)
		return 0;// Not Found in Index List
//	Match Sort Key 1 From this Point
	while(TRUE)	// Forever
	{
		RetVal = 1;// Anyway The Result is [Sort Key 1 Match]
		RecPos = fptr->BufferPos;// Save Current Memory Pos
		TempPtr = MemPtr + RecPos;// Get Record Length
		RecordLen = *TempPtr + *(TempPtr + 1) * 256;// Get Record Length
		if(RecordLen == 0)	// End of Body Record
			goto NotFound;
		SortKeyPtr = fptr->FieldList;	// Get Field List Top
		SortKeyPtr += *fptr->SortKeyPos;	// Move to Key1 FIELD
		SortKeyType = SortKeyPtr->Attr;// Get Sort Key1 Type
		TempPtr = (unsigned char *)SortKeyPtr->Ptr;	// Get Main Sort Key Ptr(Enum)
		SortKeyLen = GetSortKeyLen(SortKeyType);
		switch(SortKeyLen)
		{
		case 1:
			SortKey = *TempPtr;	// Get Byte Sort Key(Enum)
			break;
		case 2:
			SortKey = *(unsigned short *)TempPtr;	// Get Short Sort Key(Enum)
			break;
		case 4:
			SortKey = *(DWORD *)TempPtr;	// Get DWORD Sort Key(Enum)
		}
// Move to SortKey 1
		TempPtr = MemPtr + RecPos + 2 + MoveToSortFieldMemory(fptr, 0);
//	This Program is Only for ENUM Sort Key
		RecordVal = *TempPtr + *(TempPtr + 1) * 256;// Get Date
		if(RecordVal != SortKey)	// Next Day ?
			goto NotFound;
SeekNextKey:
		SortKeyPtr = fptr->FieldList;	// Get Field List Top
		SortKeyPtr += *(fptr->SortKeyPos + RetVal);	// Move to Keyn FIELD
		SortKeyType = SortKeyPtr->Attr;// Get Sort Key Type
		TempPtr = (unsigned char *)SortKeyPtr->Ptr;	// Get Main Sort Key Ptr(Enum)
		SortKeyLen = GetSortKeyLen(SortKeyType);
		switch(SortKeyLen)
		{
		case 1:
			SortKey = *TempPtr;	// Get Byte Sort Key(Enum)
			break;
		case 2:
			SortKey = *(unsigned short *)TempPtr;	// Get Short Sort Key(Enum)
			break;
		case 4:
			SortKey = *(DWORD *)TempPtr;	// Get DWORD Sort Key(Enum)
		}
		TempPtr = MemPtr + RecPos + 2 + MoveToSortFieldMemory(fptr, RetVal);//  Move to SortKey n
		switch(SortKeyLen)
		{
		case 1:
			RecordVal = *TempPtr;	// Get Byte Sort Key(Enum)
			break;
		case 2:
			RecordVal = *TempPtr + *(TempPtr + 1) * 256;	// Get Short Sort Key(Enum)
			break;
		case 4:
			RecordVal = *TempPtr + *(TempPtr + 1) * 0x100
						+ *(TempPtr + 2) * 0x10000 + *(TempPtr + 3) * 0x1000000;
			// Get DWORD Sort Key(Enum)
		}
		if(RecordVal < SortKey)	// Record Time Past?
			goto SeekNext;
		if(RecordVal > SortKey)	// Record Time Past?
			goto NotFound;
		RetVal++;
		if(RetVal != fptr->SortKeyNum)
			goto SeekNextKey;
NotFound:
		return RetVal;
SeekNext:
		TempPtr = MemPtr + RecPos;
		fptr->BufferPos = RecPos + *TempPtr + *(TempPtr + 1) * 256 + 2;
	}
}
//
//	Get Record Data from File
//
BOOL OyajiGetRecord(FILEWORKS *fptr)
{
DWORD	ByteRead;	// For Read Check
DWORD	RecLen = 0;	// Record Length
DWORD	StrLen = 0;
DWORD	CheckBuf = 0;	// Check Box Work
DWORD	RadioBuf = 0;	// Radio Button Work
DWORD	StoreLen = 0;	// String Work
DWORD	Temp = 0;	// Temp Work
DWORD	LastAttr = 0;	// Last Attribute (For Check Box)
HANDLE	hFile = fptr->FileHandle;
FIELD	*rec;
//	Get Field from File
	if(hFile == NULL)
		return FALSE;
	ReadFile(hFile, &RecLen, 2, &ByteRead, 0);	// Get Record Len
	if(ByteRead != 2)
		goto ReadError;
	if(RecLen == 0)	// End of Body Record?
	{
		SetFilePointer(hFile, -2, 0, FILE_CURRENT);// Back Len
		fptr->EndOfFile = TRUE;
		return FALSE;
	}
	for(rec = fptr->FieldList; rec->Attr; rec++)// Repeat All Field Def Ptr
	{
		if(LastAttr != FLCHECK && rec->Attr == FLCHECK)
		{
			ReadFile(hFile, &CheckBuf, 1, &ByteRead, 0);// Get Check Box
			if(ByteRead != 1)
				goto ReadError;
		}
		if(LastAttr != FLRADIO && rec->Attr == FLRADIO)
		{
			ReadFile(hFile, &RadioBuf, 1, &ByteRead, 0);// Get Radio Button
			if(ByteRead != 1)
				goto ReadError;
		}
		switch(rec->Attr)
		{
		case FLBYTE:	// Byte
			ReadFile(hFile, &Temp, 1, &ByteRead, 0);// Get 1 Byte
			if(ByteRead != 1)
				goto ReadError;
			if(rec->Ptr)
				*(unsigned char *)rec->Ptr = LOBYTE(Temp);
			break;
		case FLCHECK:	// Check Box
			if(rec->Ptr)
				*(DWORD *)rec->Ptr = ((CheckBuf & rec->MaxLen) != 0);
			break;
		case FLRADIO:	// Radio Button
			if(rec->Ptr)
				*(DWORD *)rec->Ptr = ((RadioBuf & rec->MaxLen) != 0);
			break;
		case FLSHORT:	// Short
		case FLDATE:	// Date
		case FLTIME:	// Time
			ReadFile(hFile, &Temp, 2, &ByteRead, 0);// Get 2 Byte
			if(ByteRead != 2)
				goto ReadError;
			if(rec->Ptr)
			{
				*(unsigned char *)rec->Ptr = LOBYTE(Temp);
				*((unsigned char *)rec->Ptr + 1) = HIBYTE(Temp);
			}
			break;
		case FLDWORD:	// DWORD
		case FLSINGLE:	// Single
			ReadFile(hFile, &Temp, 4, &ByteRead, 0);	// Get 4 Byte
			if(ByteRead != 4)
				goto ReadError;
			if(rec->Ptr)
				*(DWORD *)rec->Ptr = Temp;
			break;
		case FLQWORD:	// QWORD
		case FLDOUBLE:// Double
		case FLCURRENCY://Currency
			ReadFile(hFile, &Temp, 4, &ByteRead, 0);	// Get 8 Byte
			if(ByteRead != 4)
				goto ReadError;
			if(rec->Ptr)
				*(DWORD *)rec->Ptr = Temp;
			ReadFile(hFile, &Temp, 4, &ByteRead, 0);	// Get Lead Time
			if(ByteRead != 4)
				goto ReadError;
			if(rec->Ptr)
				*((DWORD *)rec->Ptr + 1) = Temp;
			break;
		case FLSTRING:// String
		case FLDROPDOWN:// Drop Down
		case FLLISTBOX:// List Box
			Temp = 0;	// Clear High Bytes
			ReadFile(hFile, &Temp, 1, &ByteRead, 0);// Get String Length
			if(ByteRead != 1)
				goto ReadError;
			if(rec->Ptr == NULL)	// Pointer Assigned?
				goto SkipString;
			StrLen = rec->MaxLen;
			if(StrLen > Temp)
				StrLen = Temp;
			if(StrLen)
			{
				ReadFile(hFile, rec->Ptr, StrLen, &ByteRead, 0);// Read String
				if(ByteRead != StrLen)
					goto ReadError;
			}
			*((unsigned char *)rec->Ptr + StrLen) = TEXT('\0');// Set Null Terminator
			if(Temp - StrLen > 0)
				SetFilePointer(hFile, Temp - StrLen, NULL, FILE_CURRENT);
			break;
// No Pointer Assigned then Just Skip
SkipString:
			if(Temp)
				SetFilePointer(hFile, Temp, NULL, FILE_CURRENT);
			break;
		case FLREPEAT:	// Val Length
			Temp = 0;	// Clear High Bytes
			ReadFile(hFile, &Temp, 1, &ByteRead, 0);// Get String Length
			if(ByteRead != 1)
				goto ReadError;
			if(rec->Ptr == NULL)
				goto SkipString;
			StrLen = rec->MaxLen;
			if(StrLen > Temp)
				StrLen = Temp;
			*((unsigned char *)rec->Ptr) = LOBYTE(StrLen);
			if(StrLen)
			{
				ReadFile(hFile, (unsigned char *)rec->Ptr + 1,
							StrLen, &ByteRead, 0);// Read Repeat
				if(ByteRead != StrLen)
					goto ReadError;
			}
			if(Temp - StrLen > 0)
				SetFilePointer(hFile, Temp - StrLen, NULL, FILE_CURRENT);
			break;
		case FLMULTIEDIT:	// Multi Edit
		case FLBINARY:
			Temp = 0;	// Clear High Bytes
			ReadFile(hFile, &Temp, 2, &ByteRead, 0);// Get String Length
			if(ByteRead != 2)
				goto ReadError;
			if(rec->Ptr == NULL)
				goto SkipString;
			StrLen = rec->MaxLen;
			if(StrLen > Temp)
				StrLen = Temp;
			*((unsigned char *)rec->Ptr) = LOBYTE(StrLen);
			*((unsigned char *)rec->Ptr + 1) = HIBYTE(StrLen);
			if(StrLen)
			{
				ReadFile(hFile, (unsigned char *)rec->Ptr + 2,
							StrLen, &ByteRead, 0);// Read Repeat
				if(ByteRead != StrLen)
					goto ReadError;
			}
			if(rec->Attr == FLMULTIEDIT)	// Multi String?
				*((unsigned char *)rec->Ptr + 2 + StrLen) = '\0';// Set Terminator
			if(Temp - StrLen > 0)
				SetFilePointer(hFile, Temp - StrLen, NULL, FILE_CURRENT);
			break;
		}
		if(rec->Attr != FLSTATIC && rec->Attr != FLGROUPBOX)
			LastAttr = rec->Attr;
	}
	return TRUE;
ReadError:
	OutErrorMsg(fptr->WindowHandle, TEXT("Cash File"), 6, 4);// Can't Read (File Read)
	OyajiFileClose(fptr);
	return FALSE;
}

//
//	Write DWORD to File
//
static BOOL WriteDword(HANDLE hFile, DWORD Val)
{
DWORD	ByteWrote;
	if(WriteFile(hFile, &Val, 4, &ByteWrote, 0) == FALSE)
		return FALSE;
	return (ByteWrote == 4);
}

//
//	Write USHORT to File
//
static BOOL WriteUshort(HANDLE hFile, DWORD Val)
{
DWORD	ByteWrote;
	if(WriteFile(hFile, &Val, 2, &ByteWrote, 0) == FALSE)
		return FALSE;
	return (ByteWrote == 2);
}
//
//	Show Error Message (Message Box)
//
static BOOL OutErrorMsg(HWND hWnd, LPTSTR str, DWORD code, DWORD Caption)
{
TCHAR	msg[256];
LPCTSTR	CapMsg[] = {
	TEXT("File Create"), TEXT("File Open"),
	TEXT("Temp File"), TEXT("Alloc Memory"),
	TEXT("File Read")};
LPCTSTR	ErrMsg[] = {
	TEXT(" Can't Create"), TEXT(" Not Found"),
	TEXT(" is not Appointment File"), TEXT(" contains Invalid Data"),
	TEXT(" Can't Rename"), TEXT(" Can't Allocate"),
	TEXT(" Can't Read"), TEXT(" Can't Write")
};
	_tcscpy(msg, str);
	_tcscat(msg, ErrMsg[code]);
	return MessageBox(hWnd, msg, CapMsg[Caption], MB_OK);
}
