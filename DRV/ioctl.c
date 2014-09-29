/************************************************************************\
**               OS/2(r) Physical Device Driver Libraries               **
\************************************************************************/

// IOCtl.c
//
// Process device specific I/O commands


#include <devhelp.h>
#include <devdebug.h>
#include <devtype.h>
#include <devrp.h>
#include <conio.h>
#include <string.h>
#include <i86.h>
#include "uptt.h"


#define IOCTLCAT  0x91

extern volatile int ibuf, lbuf; /* начало буфеpа, длина */

extern struct InfoSegGDT FAR      *g_pGlobalInfoSeg; //d_271953

//GINFOSEG gis;
PGINFOSEG far pgis;
// Dispatch IOCtl requests received from the Strategy routine
typedef VOID FAR *TVOID;

/* Create untyped far pointer from selector and offset */
#define MAKEP(sel, off) 	((PVOID)MAKEULONG(off, sel))
/* Combine l & h to form a 32 bit quantity. */
#define MAKEULONG(l, h) ((ULONG)(((USHORT)(l)) | ((ULONG)((USHORT)(h))) << 16))
#define MAKELONG(l, h)	((LONG)MAKEULONG(l, h))
typedef unsigned short SEL;	/* sel */
typedef SEL FAR *PSEL;
/* Extract selector or offset from far pointer */
#define SELECTOROF(p)       (((PUSHORT)&(p))[1])
#define OFFSETOF(p)         (((PUSHORT)&(p))[0])


WORD16 StratIOCtl(RP FAR* _rp)
{
  RPIOCtl FAR* rp = (RPIOCtl FAR*)_rp;
  WORD16 freq;
  short int kon;
  int i,rc;

  if(rp->Category != IOCTLCAT)
     return RPDONE;// | RPERR_COMMAND;
  switch (rp->Function)
  {
     case 0x01: //Beep
       freq = *((WORD16 FAR*)rp->ParmPacket);
       DosBeep(freq, 500);
       freq++;
       
       *((BYTE FAR*)rp->DataPacket) = *((WORD16 *) &freq);
       break;
      case 0x02: //get gis
//       *((WORD16 FAR*)rp->DataPacket) = *((WORD16 *) &CountIRQ);
        if(rp->DataLength < sizeof(GINFOSEG)) break;
        
   {
       TVOID   ptemp = 0;   
       rc = DevGetDOSVar(1,0,&ptemp);
       
       g_pGlobalInfoSeg = (InfoSegGDT FAR*) (MAKEP(*((PSEL)ptemp), 0));
//       tmp = g_pGlobalInfoSeg->SIS_BigTime;
//       _fmemcpy(rp->DataPacket,&tmp, sizeof(ULONG));
       _fmemcpy(rp->DataPacket,g_pGlobalInfoSeg, sizeof(struct InfoSegGDT));
//       _fmemcpy(rp->DataPacket,&ptemp, sizeof(ULONG));
//       _fmemcpy(rp->DataPacket+sizeof(ULONG),&g_pGlobalInfoSeg, sizeof(ULONG));
//      pgis = 0;
//      rc = DevGetDOSVar(1,0,(PPVOID)&pgis);
//         ptemp = pgis;   
//AllocGDTSelector
// StrikeTroll скорее всего номер пройдет так: с полученного 16:16 адреса делаешь
// Virt2Lin, потом полученный линейный адрес намапливаешь на свой GDT селектор (через
// LinToGDTSelector) с правом записи
// #define GETLINEAR_FLAG_ALLOCPHYS   0x0000000A
// #define GETLINEAR_FLAG_MAPPHYSICAL 0x00000010
// #define GETLINEAR_FLAG_MAPPROCESS  0x00000020
// #define GETLINEAR_FLAG_MAPSHARED   0x00000400
// #define GETLINEAR_FLAG_MAPHMA      0x00002000
   }  
        break;
        
      case 0x03: /* set ms counter */
     { ULONG ms; 
       TVOID   ptemp = 0;   
       LONG  phyBuff;
       LINEAR Linear;
extern SEL ReservedSelectors[2];/* Reserved GDT selectors         */
       struct InfoSegGDT FAR      *g_pGIS;
       
       ms = *((ULONG FAR*)rp->ParmPacket);
       rc = DevGetDOSVar(1,0,&ptemp);
      
       g_pGIS = (InfoSegGDT FAR*) (MAKEP(*((PSEL)ptemp), 0));
       
   rc = DevVirtToLin((USHORT) (SELECTOROF(g_pGIS)),
                          (ULONG) (OFFSETOF(g_pGIS)),
                          &Linear);
   rc = DevLinToGDTSel(ReservedSelectors[0],Linear,sizeof(InfoSegGDT) );
       
       g_pGlobalInfoSeg = (InfoSegGDT FAR*) MAKEULONG(0, ReservedSelectors[0]);
       
            // convert buffer's virtual address to physical address
//     DevVirtToPhys(g_pGlobalInfoSeg, (PULONG)&phyBuff);
       _fmemcpy(&(g_pGlobalInfoSeg->SIS_MsCount),&ms, sizeof(ULONG));
//_disable();     
//_enable();
       _fmemcpy(rp->DataPacket,g_pGlobalInfoSeg, sizeof(struct InfoSegGDT));
//       ms = rc;
//       _fmemcpy(rp->DataPacket+sizeof(ULONG),&ms, sizeof(ULONG));
//       _fmemcpy(rp->DataPacket+sizeof(ULONG),&phyBuff, sizeof(ULONG));
       
       
       
       
     } 
        break;
     
      default:
        break;
   }
  return RPDONE; // | RPERR_COMMAND;
  }

