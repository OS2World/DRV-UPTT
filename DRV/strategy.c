/* strategy.c */
/************************************************************************\
**               OS/2(r) Physical Device Driver Libraries               **
\************************************************************************/


// Strategy.c
//
// Device driver strategy entry point and dispatch table

#include <devhelp.h>
#include <devtype.h>
#include <devrp.h>
#include <conio.h>
#include <i86.h>
#include "uptt.h"

// Declare strategy entry points that are located in other source files
//
// StratInit  - Initialization entry point [Init.c]
// StratIOCtl - Audio IOCtl interface [IOCtl.c]

WORD16 StratInit(RP FAR* _rp);
WORD16 StratIOCtl(RP FAR* _rp);
WORD16 StratRemove(RP FAR* _rp);
WORD16 StratShutdown(RP FAR* _rp);
WORD16 StratError(RP FAR* _rp);
WORD16 StratOpen(RP FAR* _rp);
WORD16 StratClose(RP FAR* _rp);
WORD16 StratRead(RP FAR* _rp);
WORD16 StratWrite(RP FAR* _rp);

// Strategy dispatch table
//
// This table is used by the strategy routine to dispatch strategy requests

typedef WORD16 (*RPHandler)(RP FAR* rp);
RPHandler StratDispatch[] =
  {
  StratInit,                  // 00 (BC): Initialization
  StratError,                 // 01 (B ): Media check
  StratError,                 // 02 (B ): Build BIOS parameter block
  StratError,                 // 03 (  ): Unused
  StratRead,                 // 04 (BC): Read
  StratError,                 // 05 ( C): Nondestructive read with no wait
  StratError,                 // 06 ( C): Input status
  StratError,                 // 07 ( C): Input flush
  StratWrite,                 // 08 (BC): Write
  StratError,                 // 09 (BC): Write verify
  StratError,                 // 0A ( C): Output status
  StratError,                 // 0B ( C): Output flush
  StratError,                 // 0C (  ): Unused
  StratOpen,                 // 0D (BC): Open
  StratClose,                 // 0E (BC): Close
  StratError,                 // 0F (B ): Removable media check
  StratIOCtl,                 // 10 (BC): IO Control
  StratError,                 // 11 (B ): Reset media
  StratError,                 // 12 (B ): Get logical unit
  StratError,                 // 13 (B ): Set logical unit
  StratRemove,                // 14 ( C): Deinstall character device driver
  StratError,                 // 15 (  ): Unused
  StratError,                 // 16 (B ): Count partitionable fixed disks
  StratError,                 // 17 (B ): Get logical unit mapping of fixed disk
  StratError,                 // 18 (  ): Unused
  StratError,                 // 19 (  ): Unused
  StratError,                 // 1A (  ): Unused
  StratError,                 // 1B (  ): Unused
  StratShutdown,              // 1C (BC): Notify start or end of system shutdown
  StratError,                 // 1D (B ): Get driver capabilities
  StratError,                 // 1E (  ): Unused
  StratError                  // 1F (BC): Notify end of initialization
  };



// Strategy entry point
//
// The strategy entry point must be declared according to the STRATEGY
// calling convention, which fetches arguments from the correct registers.

// For now, we'll declare it as extern "C", so that the name is exported
// without being mangled.  That way, it can be accessed by the header
// data structure in header.asm.  (See that file for more info.)


extern "C" VOID Strategy(RP FAR* rp);


#pragma aux (STRATEGY) Strategy;

VOID Strategy(RP FAR* rp)
  {
  if (rp->Command < sizeof(StratDispatch)/sizeof(StratDispatch[0]))
    rp->Status = StratDispatch[rp->Command](rp);
  else
    rp->Status = RPDONE | RPERR_COMMAND;

  return;
}


