/************************************************************************\
**               OS/2(r) Physical Device Driver Libraries               **
**                         for Watcom C/C++ 10                          **
\************************************************************************/


//shytdown.c
//
// shutdown code
// Support shutdown of the drivers
//
// History:
//
// May 13, 96  Alger Pike    Initial version

#include <conio.h>

#include <devhelp.h>
#include <devdebug.h>
#include <devtype.h>
#include <devrp.h>

extern "C" VOID  InitPCL730(void);
extern "C" VOID  InitPCL724(void);


extern int base832; /* =0x240; */
extern int base730; /* =0x300; */
extern int configuration;

WORD16 StratShutdown(RP FAR* _rp)
  {
   RPIOCtl FAR* rp = (RPIOCtl FAR*)_rp;
  // If any work needs to be done when the system is shutdown, it
  // should be performed here.
  // This command should shutdown any hardware devices that were setup
  // during the initialization stage.  Also, this command should free
  // any resources that the driver owns.


//  reset of PCL832
         outp(base832+0x1a,0);

     if(configuration == 0)
              InitPCL730();
      else
              InitPCL724();

  return RPDONE;//| RPERR_COMMAND;
  }
