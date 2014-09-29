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

