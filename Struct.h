//
//	Struct for Field Definition
//
#define	FLENDMARK	0
#define	FLBYTE		1
#define	FLCHECK		2
#define	FLRADIO		3
#define	FLSHORT		4
#define	FLDATE		5
#define	FLTIME		6
#define	FLDWORD		7
#define	FLSINGLE	8
#define	FLQWORD		9
#define	FLDOUBLE	10
#define	FLCURRENCY	11
#define	FLSTRING	12
#define	FLDROPDOWN	13
#define	FLLISTBOX	14
#define	FLSTATIC	15
#define	FLGROUPBOX	16
#define	FLMULTIEDIT	17
#define	FLREPEAT	18
#define	FLBINARY	19
//
typedef	struct	{
	DWORD	Attr;	// Field Attribute
	DWORD	MaxLen;	// Field Info
	void	*Ptr;	// Field Buffer or Caption Pointer
	DWORD	Left;	// Rect of Caption
	DWORD	Top;	// Rect of Caption
	DWORD	Width;	// Rect of Caption
	DWORD	Hight;	// Rect of Caption
} FIELD;
//
//	Struct for File Work
//
typedef	struct	{
	HWND	WindowHandle;	// Window Handle
	DWORD	StructPtr;	// Seek Ptr to Struct Field
	DWORD	RecordPtr;	// Seek Ptr to Record Body
	DWORD	IndexPtr;	// Seek Ptr to Sort Index Table
	DWORD	StructLen;	// Length of Struct Field
	DWORD	RecordLen;	// Length of Record Body
	DWORD	IndexLen;	// Length of Sort Index Table
	DWORD	FileAge;	// Counter of Rewrite
	char	Password[8];	// File Password
	char	*DataBaseName;	// Data Base Name
	FIELD	*FieldList;	// Pointer to Field Def Array
	DWORD	IndexCount;	// Count of Sort Index Table
	unsigned char 	*IndexList;	// Index List Pointer (Allocate)
	DWORD	SortKeyNum;	// Sort Key Total Number
	DWORD	*SortKeyPos;	// Sort Key Field Pos Table
	unsigned char	*RecordBuf;	// Current Record Buffer Ptr (Allocate)
	DWORD	RecordBufLen;	// Current Record Buffer Length
	DWORD	EndOfFile;		// End of File Flag
	TCHAR	*CurrentDir;// Current Dir
	TCHAR	*CurrentName;// Current File Name
	TCHAR	*Filter;	// File Open Filter
	HANDLE	FileHandle;	// Main File Handle
	DWORD	BufferPos;//	For Memory Pointer
} FILEWORKS;
