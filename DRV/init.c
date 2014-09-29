/************************************************************************\
** UPTT.SYS драйвер для манипуляций с миллисекундым счетчиком           **
** (c) eCoSoft 2006 Евгений Коцюба 2006					**
**               OS/2(r) Physical Device Driver Libraries               **
**                         for Watcom C/C++ 10                          **
\************************************************************************/

// Init.c
//
// Device driver initialization
//
// Note that the initialization code is discardable, so normally this
// module would start with the code_seg and data_seg pragmas.  However,
// Watcom C++ 10 currently handles these pragmas incorrectly, so for now
// this code and data will stay resident.

#include <devdebug.h>
#include <devhelp.h>
#include <devtype.h>
#include <devrp.h>
#include <conio.h>



// Sign on and installation messages

static const char WELCOME[] =
  "\r\n"
  "uptt Driver v0.1 ("    __DATE__ ") Installed.\r\n";
static const char WELCOME1[] =
  "(C) eCoSoft 2006\r\n";

const int WELCOME_LENGTH  = sizeof(WELCOME) - 1;
const int WELCOME_LENGTH1 = sizeof(WELCOME1) - 1;
SEL  ReservedSelectors[2]={0,0};/* Reserved GDT selectors */
struct InfoSegGDT FAR      *g_pGlobalInfoSeg = 0;

// Initialize device driver

WORD16 StratInit(RP FAR* _rp)
{

  RPInit FAR* rp = (RPInit FAR*)_rp;
  int result=0,i,j;
static char st[80]; 
   char far *s;
   int nirq=0;

//  HFILE FAR* File;

  // MANDATORY: Initialize the Device Driver libraries.
  DevInit(rp->In.DevHlp);

  // Signal that we've installed successfully by setting the size of
  // our code and data segments.
  rp->Out.FinalCS = OffsetFinalCS;
  rp->Out.FinalDS = OffsetFinalDS;

// Print a sign on message to the debugging terminal.
/*
  cdbg << ALL << endl;
  cdbg << MSG_DEBUG << endl;
  cdbg << MSG_CS << hex << setw(4) << OffsetFinalCS
       << setw() << dec << 'h' << endl;
  cdbg << MSG_DS << hex << setw(4) << OffsetFinalDS
       << setw() << dec << 'h' << endl;
  cdbg << endl;
*/

  // Print a sign on message to the console.
  DosPutMessage(1, WELCOME_LENGTH, WELCOME);
  DosPutMessage(1, WELCOME_LENGTH1, WELCOME1);
/* 
  if( rp->In.Args)
  {   for(i=0;rp->In.Args[i] && i < 80;i++);
      DosPutMessage(1, 12, "Аpгументы:\r\n");
      DosPutMessage(1, i, rp->In.Args);
  }  / *  endofif( rp->In.Args) * /
*/  
  
  DosPutMessage(1, 2, "\r\n");

  
  i = DevAllocGDTSel(ReservedSelectors, 2);
  if(i == 0)   
     DosPutMessage(1, 2, "Ok");
  else    
  {
    st[0] = (char)(i/10 +(int)'0');
    st[1] = (char)(i%10 +(int)'0');
  }
  DosPutMessage(1, 2, "\r\n");


  //user code
  return RPDONE;
}
