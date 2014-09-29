/************************************************************************\
**               OS/2(r) Physical Device Driver Libraries               **
**                         for Watcom C/C++ 10                          **
\************************************************************************/
//open.c
//
// open code
// Executes in respose to DosOpen(Hello$)
//
// History:
//
// May 13, 96  Alger Pike    Initial version


#include <devhelp.h>
#include <devdebug.h>
#include <devtype.h>
#include <devrp.h>
#include <conio.h>
#include "buffer.h"



extern int base832; /* =0x240; */
extern volatile int ibuf, lbuf; /* начало буфеpа, длина */
extern volatile struct PCL832_state  PCL832state;

WORD16 StratOpen(RP FAR* _rp)
  {
   RPIOCtl FAR* rp = (RPIOCtl FAR*)_rp;
  // This command should shutdown any hardware devices that were setup
  // during the initialization stage.  Also, this command should free
  // any resources that the driver owns.

//  reset of PCL832
     InitPCL832(1);
  return RPDONE;
  }
