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
/* shutdown.c */
#include <conio.h>

#include <devhelp.h>
#include <devdebug.h>
#include <devtype.h>
#include <devrp.h>


//extern "C" VOID TimerHandler(void);
//#pragma aux (STRATEGY) TimerHandler;

WORD16 StratShutdown(RP FAR* _rp)
{
   RPIOCtl FAR* rp = (RPIOCtl FAR*)_rp;
  // If any work needs to be done when the system is shutdown, it
  // should be performed here.
  // This command should shutdown any hardware devices that were setup
  // during the initialization stage.  Also, this command should free
  // any resources that the driver owns.


//    if(TimerHandler)
//         DevTimerClear( TimerHandler);


  return RPDONE;//| RPERR_COMMAND;
}
