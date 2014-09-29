/* uptt.h */
/* GlobalInfoSeg (PSEL) Address of the global information segment structure, as defined below: */

typedef struct _GINFOSEG 
{
    ULONG   time;		//	time in seconds since 1/1/1970.
    ULONG   msecs;		//	milliseconds
    UCHAR   hour;		//	Current hour
    UCHAR   minutes;		//	Current minute
    UCHAR   seconds;		//	Current second
    UCHAR   hundredths;		//	Current hundredth of a second
    USHORT  timezone;		//	minutes from UTC. If FFFFH, TimeZone is undefined. 
    USHORT  cusecTimerInterval;	//	timter interval, .0001 secs
    UCHAR   day;		//	day of month
    UCHAR   month;		//	month, 1-12
    USHORT  year;		//	year
    UCHAR   weekday;		//	day of week, 0=Sunday, 1=Monday...
    UCHAR   uchMajorVersion;	//	major version number
    UCHAR   uchMinorVersion;	//	minor version number
    UCHAR   chRevisionLetter;	//	rev level
    UCHAR   sgCurrent;		//	current foreground full-screen session.
    UCHAR   sgMax;		//	max number of full-screen sessions
    UCHAR   cHugeShift;		//	shift count for huge elements
    UCHAR   fProtectModeOnly;	//	protect mode only
    USHORT  pidForeground;	//	pid of last process in foreground
    UCHAR   fDynamicSched;	//	dynamic variation flag: 0/1 Absolute/Enabled. 
    UCHAR   csecMaxWait;	//	max wait in seconds
    USHORT  cmsecMinSlice;	//	min timeslice in milliseconds
    USHORT  cmsecMaxSlice;	//	max timeslice in milliseconds
    USHORT  bootdrive;		//	boot drive (0=a, 1=b...)
    UCHAR   amecRAS[32];	//	system trace major code flag bits
    UCHAR   csgWindowableVioMax;//	max number of VIO sessions
    UCHAR   csgPMMax;		//	max number of PM sessions
} GINFOSEG;
typedef GINFOSEG far *PGINFOSEG;

struct InfoSegGDT {

/* Time (offset 0x00) */

unsigned long   SIS_BigTime;    /* Time from 1-1-1970 in seconds */
unsigned long   SIS_MsCount;    /* Freerunning milliseconds counter */
unsigned char   SIS_HrsTime;    /* Hours */
unsigned char   SIS_MinTime;    /* Minutes */
unsigned char   SIS_SecTime;    /* Seconds */
unsigned char   SIS_HunTime;    /* Hundredths of seconds */
unsigned short  SIS_TimeZone;   /* Timezone in min from GMT (Set to EST) */
unsigned short  SIS_ClkIntrvl;  /* Timer interval (units=0.0001 secs) */

/* Date (offset 0x10) */

unsigned char   SIS_DayDate;    /* Day-of-month (1-31) */
unsigned char   SIS_MonDate;    /* Month (1-12) */
unsigned short  SIS_YrsDate;    /* Year (>= 1980) */
unsigned char   SIS_DOWDate;    /* Day-of-week (1-1-80 = Tues = 3) */

/* Version (offset 0x15) */

unsigned char   SIS_VerMajor;   /* Major version number */
unsigned char   SIS_VerMinor;   /* Minor version number */
unsigned char   SIS_RevLettr;   /* Revision letter */

/* System Status (offset 0x18) */

/* XLATOFF */
#ifdef  OLDVER
unsigned char   CurScrnGrp;     /* Fgnd screen group # */
#else
/* XLATON */
unsigned char   SIS_CurScrnGrp; /* Fgnd screen group # */
/* XLATOFF */
#endif
/* XLATON */
unsigned char   SIS_MaxScrnGrp; /* Maximum number of screen groups */
unsigned char   SIS_HugeShfCnt; /* Shift count for huge segments */
unsigned char   SIS_ProtMdOnly; /* Protect-mode-only indicator */
unsigned short  SIS_FgndPID;    /* Foreground process ID */

/* Scheduler Parms (offset 0x1E) */

unsigned char   SIS_Dynamic;    /* Dynamic variation flag (1=enabled) */
unsigned char   SIS_MaxWait;    /* Maxwait (seconds) */
unsigned short  SIS_MinSlice;   /* Minimum timeslice (milliseconds) */
unsigned short  SIS_MaxSlice;   /* Maximum timeslice (milliseconds) */

/* Boot Drive (offset 0x24) */

unsigned short  SIS_BootDrv;    /* Drive from which system was booted */

/* RAS Major Event Code Table (offset 0x26) */

unsigned char   SIS_mec_table[32]; /* Table of RAS Major Event Codes (MECs) */

/* Additional Session Data (offset 0x46) */

unsigned char   SIS_MaxVioWinSG;  /* Max. no. of VIO windowable SG's */
unsigned char   SIS_MaxPresMgrSG; /* Max. no. of Presentation Manager SG's */

/* Error logging Information (offset 0x48) */

unsigned short  SIS_SysLog;     /* Error Logging Status */

/* Additional RAS Information (offset 0x4A) */

unsigned short  SIS_MMIOBase;   /* Memory mapped I/O selector */
unsigned long   SIS_MMIOAddr;   /* Memory mapped I/O address  */

/* Additional 2.0 Data (offset 0x50) */

unsigned char   SIS_MaxVDMs;      /* Max. no. of Virtual DOS machines */
unsigned char   SIS_Reserved;
};

#define SIS_LEN         sizeof(struct InfoSegGDT)

