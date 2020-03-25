// --------------------------------------------------------------------------
//
// Copyright (c) 1997, Microsoft Corporation
//
// Module:
//
//     rapi.h
//
// Purpose:
//
//    Master include file for Pegasus Remote API
//
// --------------------------------------------------------------------------

#ifndef RAPI_H
#define RAPI_H

#include <windows.h>

//
// The Pegasus WIN32_FIND_DATA structure differs from the
// Windows WIN32_FIND_DATA stucture so we copy the Pegasus
// definition to here so that both sides match.
//
typedef struct _PEG_FIND_DATA {
    DWORD    dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD    nFileSizeHigh;
    DWORD    nFileSizeLow;
    DWORD    dwOID;
    WCHAR    cFileName[MAX_PATH];
} PEG_FIND_DATA, *LPPEG_FIND_DATA;

typedef PEG_FIND_DATA** LPLPPEG_FIND_DATA;

//
// These are flags for PegFindAllFiles
//
#define FAF_ATTRIBUTES      ((DWORD) 0x01)
#define FAF_CREATION_TIME   ((DWORD) 0x02)
#define FAF_LASTACCESS_TIME ((DWORD) 0x04)
#define FAF_LASTWRITE_TIME  ((DWORD) 0x08)
#define FAF_SIZE_HIGH       ((DWORD) 0x10)
#define FAF_SIZE_LOW        ((DWORD) 0x20)
#define FAF_OID             ((DWORD) 0x40)
#define FAF_NAME            ((DWORD) 0x80)
#define FAF_FLAG_COUNT      ((UINT)  8)
#define FAF_ATTRIB_CHILDREN ((DWORD) 0x1000)
#define FAF_ATTRIB_NO_HIDDEN ((DWORD) 0x2000)
#define FAF_FOLDERS_ONLY    ((DWORD) 0x4000)
#define FAF_NO_HIDDEN_SYS_ROMMODULES ((DWORD) 0x8000)

#define FAD_OID             ((WORD) 0x01)
#define FAD_FLAGS           ((WORD) 0x02)
#define FAD_NAME            ((WORD) 0x04)
#define FAD_TYPE            ((WORD) 0x08)
#define FAD_NUM_RECORDS     ((WORD) 0x10)
#define FAD_NUM_SORT_ORDER  ((WORD) 0x20)
#define FAD_SIZE            ((WORD) 0x40)
#define FAD_LAST_MODIFIED   ((WORD) 0x80)
#define FAD_SORT_SPECS      ((WORD) 0x100)
#define FAD_FLAG_COUNT      ((UINT) 9)

#ifndef FILE_ATTRIBUTE_INROM
#define FILE_ATTRIBUTE_INROM        0x00000040
#endif
#ifndef FILE_ATTRIBUTE_ROMSTATICREF
#define FILE_ATTRIBUTE_ROMSTATICREF 0x00001000
#endif
#ifndef FILE_ATTRIBUTE_ROMMODULE
#define FILE_ATTRIBUTE_ROMMODULE    0x00002000
#endif

//
// The following is not a standard Pegasus File Attribute.
//
#ifndef FILE_ATTRIBUTE_HAS_CHILDREN
#define FILE_ATTRIBUTE_HAS_CHILDREN 0x00010000
#endif

//
// The following definitions are for the client side only,
// because they are already defined on the Pegasus.
//
#ifndef PEG

#include <stddef.h>

typedef struct STORE_INFORMATION {
	DWORD dwStoreSize;
	DWORD dwFreeSize;
} STORE_INFORMATION, *LPSTORE_INFORMATION;

typedef DWORD PEGPROPID;
typedef PEGPROPID *PPEGPROPID;
#define TypeFromPropID(propid) LOWORD(propid)

typedef DWORD PEGOID;
typedef PEGOID *PPEGOID;

typedef struct _PEGFILEINFO {
    DWORD   dwAttributes;
    PEGOID  oidParent;
    WCHAR   szFileName[MAX_PATH];
    FILETIME ftLastChanged;
    DWORD   dwLength;
} PEGFILEINFO;

typedef struct _PEGDIRINFO {
    DWORD   dwAttributes;
    PEGOID  oidParent;
    WCHAR   szDirName[MAX_PATH];
} PEGDIRINFO;

typedef struct _PEGRECORDINFO {
    PEGOID  oidParent;
} PEGRECORDINFO;

#define PEGDB_SORT_DESCENDING         0x00000001
#define PEGDB_SORT_CASEINSENSITIVE    0x00000002
#define PEGDB_SORT_UNKNOWNFIRST       0x00000004
#define PEGDB_SORT_GENERICORDER       0x00000008

typedef struct _SORTORDERSPEC {
    PEGPROPID propid;
    DWORD     dwFlags;
} SORTORDERSPEC;

#define PEGDB_MAXDBASENAMELEN  32
#define PEGDB_MAXSORTORDER     4

#define PEGDB_VALIDNAME     0x0001
#define PEGDB_VALIDTYPE     0x0002
#define PEGDB_VALIDSORTSPEC 0x0004
#define PEGDB_VALIDMODTIME  0x0008

typedef struct _PEGDBASEINFO {
    DWORD   dwFlags;
    WCHAR   szDbaseName[PEGDB_MAXDBASENAMELEN];
    DWORD   dwDbaseType;
    WORD    wNumRecords;
    WORD    wNumSortOrder;
    DWORD   dwSize;
    FILETIME ftLastModified;
    SORTORDERSPEC rgSortSpecs[PEGDB_MAXSORTORDER];
} PEGDBASEINFO;

typedef struct _PEGDB_FIND_DATA {
    PEGOID       OidDb;
    PEGDBASEINFO DbInfo;
} PEGDB_FIND_DATA, *LPPEGDB_FIND_DATA;

typedef PEGDB_FIND_DATA ** LPLPPEGDB_FIND_DATA;

#define OBJTYPE_INVALID     0
#define OBJTYPE_FILE        1
#define OBJTYPE_DIRECTORY   2
#define OBJTYPE_DATABASE    3
#define OBJTYPE_RECORD      4

typedef struct _PEGOIDINFO {
    WORD  wObjType;
    WORD  wPad;
    union {
        PEGFILEINFO   infFile;
        PEGDIRINFO    infDirectory;
        PEGDBASEINFO  infDatabase;
        PEGRECORDINFO infRecord;
    };
} PEGOIDINFO;

#define PEGDB_AUTOINCREMENT     0x00000001

#define PEGDB_SEEK_PEGOID       0x00000001
#define PEGDB_SEEK_BEGINNING    0x00000002
#define PEGDB_SEEK_END          0x00000004
#define PEGDB_SEEK_CURRENT      0x00000008
#define PEGDB_SEEK_VALUESMALLER     0x00000010
#define PEGDB_SEEK_VALUEFIRSTEQUAL  0x00000020
#define PEGDB_SEEK_VALUEGREATER     0x00000040
#define PEGDB_SEEK_VALUENEXTEQUAL   0x00000080

typedef struct _PEGBLOB {
    DWORD       dwCount;
    LPBYTE      lpb;
} PEGBLOB;

#define PEGVT_I2       2
#define PEGVT_UI2      18
#define PEGVT_I4       3
#define PEGVT_UI4      19
#define PEGVT_FILETIME 64
#define PEGVT_LPWSTR   31
#define PEGVT_BLOB     65

typedef union _PEGVALUNION {
    short           iVal;
    USHORT          uiVal;
    long            lVal;
    ULONG           ulVal;
    FILETIME        filetime;
    LPWSTR          lpwstr;
    PEGBLOB         blob;
} PEGVALUNION;

#define PEGDB_PROPNOTFOUND 0x0100
#define PEGDB_PROPDELETE   0x0200
typedef struct _PEGPROPVAL {
    PEGPROPID   propid;
    WORD        wLenData;
    WORD        wFlags;
    PEGVALUNION val;
} PEGPROPVAL, *PPEGPROPVAL;

#define PEGDB_MAXDATABLOCKSIZE  4092
#define PEGDB_MAXPROPDATASIZE   (PEGDB_MAXDATABLOCKSIZE*16)
#define PEGDB_MAXRECORDSIZE     (128*1024)

#define PEGDB_ALLOWREALLOC  0x00000001

#define SYSMEM_CHANGED	0
#define SYSMEM_MUSTREBOOT 1
#define SYSMEM_REBOOTPENDING 2
#define SYSMEM_FAILED 3

typedef struct _PEGOSVERSIONINFO{
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    WCHAR szCSDVersion[ 128 ];
} PEGOSVERSIONINFO, *LPPEGOSVERSIONINFO;

#define AC_LINE_OFFLINE                 0x00
#define AC_LINE_ONLINE                  0x01
#define AC_LINE_BACKUP_POWER            0x02
#define AC_LINE_UNKNOWN                 0xFF

#define BATTERY_FLAG_HIGH               0x01
#define BATTERY_FLAG_LOW                0x02
#define BATTERY_FLAG_CRITICAL           0x04
#define BATTERY_FLAG_CHARGING           0x08
#define BATTERY_FLAG_NO_BATTERY         0x80
#define BATTERY_FLAG_UNKNOWN            0xFF

#define BATTERY_PERCENTAGE_UNKNOWN      0xFF

#define BATTERY_LIFE_UNKNOWN        0xFFFFFFFF

typedef struct _SYSTEM_POWER_STATUS_EX {
    BYTE ACLineStatus;
    BYTE BatteryFlag;
    BYTE BatteryLifePercent;
    BYTE Reserved1;
    DWORD BatteryLifeTime;
    DWORD BatteryFullLifeTime;
    BYTE Reserved2;
    BYTE BackupBatteryFlag;
    BYTE BackupBatteryLifePercent;
    BYTE Reserved3;
    DWORD BackupBatteryLifeTime;
    DWORD BackupBatteryFullLifeTime;
}   SYSTEM_POWER_STATUS_EX, *PSYSTEM_POWER_STATUS_EX, *LPSYSTEM_POWER_STATUS_EX;

typedef struct _RAPIINIT
{
    DWORD cbSize;
    HANDLE heRapiInit;
    HRESULT hrRapiInit;
} RAPIINIT;

STDAPI PegRapiInitEx(RAPIINIT*);

STDAPI PegRapiInit();
STDAPI PegRapiUninit();

STDAPI_(PEGOID) PegCreateDatabase    (LPWSTR, DWORD, WORD, SORTORDERSPEC*);
STDAPI_(BOOL  ) PegDeleteDatabase    (PEGOID);
STDAPI_(BOOL  ) PegDeleteRecord      (HANDLE, PEGOID);
STDAPI_(HANDLE) PegFindFirstDatabase (DWORD);
STDAPI_(PEGOID) PegFindNextDatabase  (HANDLE);
STDAPI_(BOOL  ) PegOidGetInfo        (PEGOID, PEGOIDINFO*);
STDAPI_(HANDLE) PegOpenDatabase      (PPEGOID, LPWSTR, PEGPROPID, DWORD, HWND);
STDAPI_(PEGOID) PegReadRecordProps   (HANDLE, DWORD, LPWORD, PEGPROPID*, LPBYTE*, LPDWORD);
STDAPI_(PEGOID) PegSeekDatabase      (HANDLE, DWORD, LONG, LPDWORD);
STDAPI_(BOOL  ) PegSetDatabaseInfo   (PEGOID, PEGDBASEINFO*);
STDAPI_(PEGOID) PegWriteRecordProps  (HANDLE, PEGOID, WORD, PEGPROPVAL*);
STDAPI_(HANDLE) PegFindFirstFile     (LPCWSTR, LPPEG_FIND_DATA);
STDAPI_(BOOL  ) PegFindNextFile      (HANDLE, LPPEG_FIND_DATA);
STDAPI_(BOOL  ) PegFindClose         (HANDLE);
STDAPI_(DWORD ) PegGetFileAttributes (LPCWSTR);
STDAPI_(BOOL  ) PegSetFileAttributes (LPCWSTR, DWORD);
STDAPI_(HANDLE) PegCreateFile        (LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
STDAPI_(BOOL  ) PegReadFile          (HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
STDAPI_(BOOL  ) PegWriteFile         (HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
STDAPI_(BOOL  ) PegCloseHandle       (HANDLE);
STDAPI_(BOOL  ) PegFindAllFiles      (LPCWSTR, DWORD, LPDWORD, LPLPPEG_FIND_DATA);
STDAPI_(BOOL  ) PegFindAllDatabases  (DWORD, WORD, LPWORD, LPLPPEGDB_FIND_DATA);
STDAPI_(DWORD ) PegGetLastError      (void);
STDAPI          GetRapiError         (void);
STDAPI_(DWORD ) PegSetFilePointer    (HANDLE, LONG, PLONG, DWORD);
STDAPI_(BOOL  ) PegSetEndOfFile      (HANDLE);
STDAPI_(BOOL  ) PegCreateDirectory   (LPCWSTR, LPSECURITY_ATTRIBUTES);
STDAPI_(BOOL  ) PegRemoveDirectory   (LPCWSTR);
STDAPI_(BOOL  ) PegCreateProcess     (LPCWSTR, LPCWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPWSTR, LPSTARTUPINFO, LPPROCESS_INFORMATION);
STDAPI_(BOOL  ) PegMoveFile          (LPCWSTR, LPCWSTR);
STDAPI_(BOOL  ) PegCopyFile          (LPCWSTR, LPCWSTR, BOOL);
STDAPI_(BOOL  ) PegDeleteFile        (LPCWSTR);
STDAPI_(DWORD ) PegGetFileSize       (HANDLE, LPDWORD);
STDAPI_(LONG  ) PegRegOpenKeyEx      (HKEY, LPCWSTR, DWORD, REGSAM, PHKEY);
STDAPI_(LONG  ) PegRegEnumKeyEx      (HKEY, DWORD, LPWSTR, LPDWORD, LPDWORD, LPWSTR, LPDWORD, PFILETIME);
STDAPI_(LONG  ) PegRegCreateKeyEx    (HKEY, LPCWSTR, DWORD, LPWSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);
STDAPI_(LONG  ) PegRegCloseKey       (HKEY);
STDAPI_(LONG  ) PegRegDeleteKey      (HKEY, LPCWSTR);
STDAPI_(LONG  ) PegRegEnumValue      (HKEY, DWORD, LPWSTR, LPDWORD, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
STDAPI_(LONG  ) PegRegDeleteValue    (HKEY, LPCWSTR);
STDAPI_(LONG  ) PegRegQueryInfoKey   (HKEY, LPWSTR, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, PFILETIME);
STDAPI_(LONG  ) PegRegQueryValueEx   (HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
STDAPI_(LONG  ) PegRegSetValueEx     (HKEY, LPCWSTR, DWORD, DWORD, LPBYTE, DWORD);
STDAPI_(BOOL  ) PegGetStoreInformation(LPSTORE_INFORMATION);
STDAPI_(INT   ) PegGetSystemMetrics  (INT);
STDAPI_(INT   ) PegGetDesktopDeviceCaps(INT);
STDAPI_(VOID  ) PegGetSystemInfo     (LPSYSTEM_INFO);
STDAPI_(DWORD ) PegSHCreateShortcut  (LPWSTR, LPWSTR);
STDAPI_(BOOL  ) PegSHGetShortcutTarget(LPWSTR, LPWSTR, INT);
STDAPI_(BOOL  ) PegCheckPassword     (LPWSTR);
STDAPI          RapiFreeBuffer       (LPVOID);
STDAPI_(BOOL  ) PegGetFileTime       (HANDLE, LPFILETIME, LPFILETIME, LPFILETIME);
STDAPI_(BOOL  ) PegSetFileTime       (HANDLE, LPFILETIME, LPFILETIME, LPFILETIME);
STDAPI_(BOOL  ) PegGetVersionEx      (LPPEGOSVERSIONINFO);
STDAPI_(HWND  ) PegGetWindow         (HWND, UINT);
STDAPI_(LONG  ) PegGetWindowLong     (HWND, int);
STDAPI_(int   ) PegGetWindowText     (HWND, LPWSTR, int);
STDAPI_(int   ) PegGetClassName      (HWND, LPWSTR, int);
STDAPI_(VOID  ) PegGlobalMemoryStatus(LPMEMORYSTATUS);
STDAPI_(BOOL  ) PegGetSystemPowerStatusEx(PSYSTEM_POWER_STATUS_EX, BOOL);

#endif // #ifndef PEG


#endif // #ifndef RAPI_H
