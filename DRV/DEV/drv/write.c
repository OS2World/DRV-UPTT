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
#include "buffer.h"


struct PCL832interp  buf[PCL832BUF_SIZE];
volatile int ibuf=0,lbuf=0; /* начало буфеpа, длина */
volatile int IsInterrupt=0,IsWrite=0;
extern int DebBuf[512];


WORD16 StratWrite(RP FAR* _rp)
  {
  // RPIOCtl FAR* rp = (RPIOCtl FAR*)_rp;
  RPRWV FAR* rp = (RPRWV FAR*)_rp;
  struct PCL832interp FAR * pd;
  VIRTUAL appl_ptr;
  int nb,i,j;   
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
  rp->Count = 0; // in case we fail


appl_ptr = (VOID FAR *)0x1f1f;
//  *( (int far * )  (&DebBuf[6])) =  (int ) appl_ptr;

   i = DevPhysToVirt(rp->Transfer,nb,(VIRTUAL FAR*) &appl_ptr);

//  *( (PHYSICAL *) (&DebBuf[0])) =  rp->Transfer;
//  *( (int far * far *)  (&DebBuf[2])) =  (int far * far ) appl_ptr;
//  *( (int far * far *)  (&DebBuf[4])) =  (int far *)buf;
// //  DebBuf[8] =  i;

/* добавить в буфеp n-элементов */
   pd = (struct PCL832interp FAR *) appl_ptr; /* *appl_ptr ?? */

  nb = nb /sizeof(struct PCL832interp);
//  if(nb > 64) nb = 64;

    if(lbuf+nb > PCL832BUF_SIZE)
    {
         return (RPDONE | RPBUSY);
    }
_disable();
    j = (ibuf+lbuf)%PCL832BUF_SIZE;
    lbuf += nb;
_enable();

    for(i=0;i<nb;i++)
    {    buf[j] = pd[i]; 
      j++;
      if(j>=PCL832BUF_SIZE) j = 0;
    }
/*
DebBuf[0] =  buf[0].dx[0];
DebBuf[1] =  buf[0].dx[1];
DebBuf[2] =  buf[0].dx[2];
DebBuf[3] =  buf[0].dt;

DebBuf[4] =  buf[1].dx[0];
DebBuf[5] =  buf[1].dx[1];
DebBuf[6] =  buf[1].dx[2];
DebBuf[7] =  buf[1].dt;

DebBuf[8] =  buf[2].dx[0];
DebBuf[9] =  buf[2].dx[1];
DebBuf[10]=  buf[2].dx[2];
DebBuf[11]=  buf[2].dt;
*/
//    ibuf = j;
    rp->Count = nb;       
    return (RPDONE);

}

/*
 case RPREAD:                  // 0x04
 
    rp->s.ReadWrite.count = 0;       // in case we fail
 
    input_char = inp(DIGIO_INPUT);   // get data
 
    if (PhysToVirt( (ULONG) rp->s.ReadWrite.buffer,
       1,0,&appl_ptr))
         return (RPDONE | RPERR | ERROR_GEN_FAILURE);
 
    if (MoveBytes((FARPOINTER)&input_char,appl_ptr,1))
         return (RPDONE | RPERR | ERROR_GEN_FAILURE);
 
    rp->s.ReadWrite.count = 1;       // one byte read
    return (RPDONE);
*/
 

/* добавить в буфеp n-элементов */
int AddBufer( struct PCL832interp FAR * pd, int n)
{ int i,j;
  if(lbuf+n > PCL832BUF_SIZE) return 1;
  j = ibuf;
  for(i=0;i<n;i++)
  { buf[j] = pd[i];
    j++;
    if(j>=PCL832BUF_SIZE) j = 0;
  }
  lbuf += n;
  ibuf = j;
  return 0;
}
int GetBuferLength(void)
{  return lbuf;
}

/* удалить один элемент из буфеpа*/
int DelBufer(void)
{
   if(lbuf)
   { lbuf--;
     ibuf++;
     if(ibuf>=PCL832BUF_SIZE) ibuf = 0;
   }
   return 0;
}


