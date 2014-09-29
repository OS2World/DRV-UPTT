/************************************************************************\
**               OS/2(r) Physical Device Driver Libraries               **
\************************************************************************/
//remove.c
//
// remove code
// Support deinstallation of the drivers

#include <devhelp.h>
#include <devdebug.h>
#include <devtype.h>
#include <devrp.h>



WORD16 StratRemove(RP FAR* _rp)
 {
   RPIOCtl FAR* rp = (RPIOCtl FAR*)_rp;
  // This command should shutdown any hardware devices that were setup
  // during the initialization stage.  Also, this command should free
  // any resources that the driver owns.

  return RPDONE;// | RPERR_COMMAND;
 };
