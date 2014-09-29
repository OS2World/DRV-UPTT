/************************************************************************\
**               OS/2(r) Physical Device Driver Libraries               **
\************************************************************************/

//Write.c
//
// Write code
// Executes in respose to DosWrite(Hello$)

#include <devhelp.h>
#include <devdebug.h>
#include <devtype.h>
#include <devrp.h>
#include <conio.h>
#include <i86.h>


WORD16 StratWrite(RP FAR* _rp)
  {
  // RPIOCtl FAR* rp = (RPIOCtl FAR*)_rp;
  RPRWV FAR* rp = (RPRWV FAR*)_rp;
  int nb; 
/*
struct        RPRWV           : public RP
  {
  BYTE        Media;          // Media descriptor
  PHYSICAL    Transfer;       // Transfer physical address
  WORD16      Count;          // Number of bytes (C) or sectors (B)
  WORD32      Start;          // Starting sector number (B)
  HFILE       FileID;         // System file number
  };
*/
  nb = rp->Count;
//  rp->Count = 0; // in case we fail

    rp->Count = nb;       
    return (RPDONE);

}

