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
#include <devhelp.h>
#include <devdebug.h>
#include <devtype.h>
#include <devrp.h>
#include <conio.h>


WORD16 StratOpen(RP FAR* _rp)
{
   RPIOCtl FAR* rp = (RPIOCtl FAR*)_rp;
  // This command should shutdown any hardware devices that were setup
  // during the initialization stage.  Also, this command should free
  // any resources that the driver owns.
     
  return RPDONE;
}
