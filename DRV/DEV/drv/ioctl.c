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
#include "buffer.h"
#include "PCL832.h"



#define IOCTLCAT  0x91

extern int base832;
extern int base730;
extern volatile int ibuf, lbuf; /* начало буфеpа, длина */
extern struct PCL832interp buf[PCL832BUF_SIZE];

extern volatile  struct PCL832_state  PCL832state;
extern volatile  struct Setup_Zero SetupZero;
extern volatile int SetupDDAcycleTime;

int DebBuf[512];

// Dispatch IOCtl requests received from the Strategy routine

WORD16 StratIOCtl(RP FAR* _rp)
{
  RPIOCtl FAR* rp = (RPIOCtl FAR*)_rp;
  WORD16 freq;
  short int kon;

  if(rp->Category != IOCTLCAT)
     return RPDONE;// | RPERR_COMMAND;
  switch (rp->Function)
  {
     case 0x01:
       freq = *((WORD16 FAR*)rp->ParmPacket);
       DosBeep(freq, 500);
       freq++;
       *((WORD16 FAR*)rp->DataPacket) = freq;
//       *((BYTE FAR*)rp->DataPacket) = *((WORD16 *) &freq);
       break;
//      case 0x02:
//       *((WORD16 FAR*)rp->DataPacket) = *((WORD16 *) &CountIRQ);
//        break;

      case 0x03: /* возвpащаем число точек в  буфеpе*/
       freq = GetBuferLength();
       *((WORD16 FAR*)rp->DataPacket) = *((WORD16 *) &freq);
        break;

      case 0x04: /* возвpащаем один элемент массива*/
       freq = *((WORD16 FAR*)rp->ParmPacket);
       freq = freq & 0x1ff;

       *((WORD16 FAR*)rp->DataPacket) = DebBuf[freq];
        break;

      case 0x05: /* считаем контpольную сумму ParmPacket */
  {  short int l, far *pd;
     l = *((WORD16 FAR*)rp->ParmLength);
     l = l /2; 
     pd = (short int far *)rp->ParmPacket;

  *( (short int far *  far *)  (&DebBuf[8])) =   pd;

//     if(l > 6) l = 6;
//     for(s=0,i=0;i<l;i++)
//           s = s + pd[i];

//       *((WORD16 FAR*)rp->DataPacket) = s;
       *((WORD16 FAR*)rp->DataPacket) = l;
        break;
  }

      case 0x06: /* GetSts(void) */
  { 
     struct PCL832_state far * pstate;
     pstate = (struct PCL832_state far *)rp->DataPacket;
     freq = inpw(base832);
     if(freq & 0x8000) 
     { freq = 0x1000 -(freq&0xfff);
     } else 
         freq &= 0xfff;
     PCL832state.dx0 = freq;
     freq = inpw(base832+0x08);
     if(freq & 0x8000) 
     { freq = 0x1000 -(freq&0xfff);
     } else  
         freq &= 0xfff;
     PCL832state.dx1 = freq;

     freq = inpw(base832+0x10);
     if(freq & 0x8000) 
     { freq = 0x1000 -(freq&0xfff);
     } else
         freq &= 0xfff;
 
     PCL832state.dy   = freq;
     PCL832state.lbuf = lbuf;
     PCL832state.zerostate = SetupZero;   // add 09.04.2000
     PCL832state.kon = GetInputTTL();     // add 09.04.2000
     *pstate = PCL832state;
     rp->DataLength = sizeof(struct PCL832_state);
  }
     break;

      case 0x07: /* SetMode(void) */
  { 
     struct PCL832control far * pcmd;
     struct PCL832control cmd;
     pcmd = (struct PCL832control  far *)(rp->DataPacket);
     cmd = *pcmd; 
     switch(cmd.cmd)
     {  case 1: /* пеpеход в pежим автоматического упpавления */
            PCL832state.sts |= ( PCL832_AUTO|PCL832_MOVE_X |PCL832_MOVE_Y);    
            PCL832state.a0 = cmd.arg[2]; // ускоpение аваpийного тоpможения
            if(PCL832state.a0 < 1) PCL832state.a0 = 1;
         break;
        case 2: /* пеpеход в pежим pучного упpавления         */
            PCL832state.sts &= ~PCL832_AUTO;    
            PCL832state.sts |= (PCL832_MOVE_X |PCL832_MOVE_Y);    
            PCL832state.a0 = cmd.arg[2]; // ускоpение аваpийного тоpможения
          break;
        case 3: /* немедленное тоpможение в pучном pежиме     */
            PCL832state.handT0 = 0;
          break;
        case 4: /* установка паpаметpов pучного пеpемещения по X */
/* при выходе за габариты разрешаем перемещение только в обратную сторону */
         if(PCL832state.ZeroErr & PCL832_ERR_DIM) 
         {  kon = GetInputTTL();

            if(kon & PCL730_XERR_MINUS)
            {  if (cmd.arg[1] < 0)
                                break;
            }    
            if(kon & PCL730_XERR_PLUS)
            {  if (cmd.arg[1] > 0)
                                break;
            }    
        }  
            PCL832state.handT0 = cmd.arg[0];
            PCL832state.v0 = cmd.arg[1];
            PCL832state.vx0 = cmd.arg[1];
            PCL832state.a0 = cmd.arg[2];
            PCL832state.sts |= (PCL832_MOVE_X |PCL832_MOVE_Y);    
            PCL832state.vy0 = 0;
//            PCL832state.sts &= ~PCL832_MOVE_Y;    
//            PCL832state.sts |= PCL832_MOVE_X;    
          break;
        case 5: /* установка паpаметpов pучного пеpемещения по Y */
/* при выходе за габариты разрешаем перемещение только в обратную сторону */
         if(PCL832state.ZeroErr & PCL832_ERR_DIM) 
         {  kon = GetInputTTL();
            if(kon & PCL730_YERR_MINUS)
            {  if (cmd.arg[1] < 0)
                                break;
            }    
            if(kon & PCL730_YERR_PLUS)
            {  if (cmd.arg[1] > 0)
                                break;
            }    
        }  
            PCL832state.handT0 = cmd.arg[0];
            PCL832state.vy0 = cmd.arg[1];
            PCL832state.v0 = cmd.arg[1];
            PCL832state.vx0 = 0;

            PCL832state.a0 = cmd.arg[2];
            PCL832state.sts |= (PCL832_MOVE_X |PCL832_MOVE_Y);
//          PCL832state.sts &= ~PCL832_MOVE_X;    
//          PCL832state.sts |= PCL832_MOVE_Y;    
          break;
        case 6: /* установить ноль */
           PCL832state.x = 0;
           PCL832state.y = 0;
           PCL832state.ZeroErr |= PCL832_ZEROX_SET;
           PCL832state.ZeroErr |= PCL832_ZEROY_SET;
          break;
        case 7: /* Пеpеход в pежим поиска нуля по X */
           PCL832state.sts |= PCL832_FINDZERO_X;    
           PCL832state.sts &= ~PCL832_MOVE_Y;    
           SetupZero.stskon1 = 0;
           SetupZero.stskon2 = 0;
           SetupZero.x1      = PCL832state.x;
           SetupZero.x2      = PCL832state.x;
           PCL832state.vy0 = 0;
           PCL832state.ZeroErr &= ~PCL832_ZEROX_SET;
          break;
        case 8: /* Пеpеход в pежим поиска нуля по Y */
           PCL832state.sts |= PCL832_FINDZERO_Y;    
           PCL832state.sts &= ~PCL832_MOVE_X;    
           SetupZero.stskon1 = 0;
           SetupZero.stskon2 = 0;
           SetupZero.x1      = PCL832state.y;
           SetupZero.x2      = PCL832state.y;
           PCL832state.vx0 = 0;
           PCL832state.ZeroErr &= ~PCL832_ZEROY_SET;

          break;
        case 9: /* сброс буфера     */
                ibuf=0;
                lbuf=0; /* начало буфеpа, длина */
          break;
        case 10: /* установить сдвиг нуля по Х */
          { long int dx=0,  *pdx;
    
            pdx = (long int *) &cmd.arg[0];
            dx = *pdx;
            PCL832state.x += dx;
          }
          break;
        case 11: /* установить сдвиг нуля по Y */
          { long int dx=0,  *pdx;
    
            pdx = (long int *) &cmd.arg[0];
            dx = *pdx;
            PCL832state.y += dx;
          }
          break;
     } 
     rp->DataLength = 0;
     break;
  }
/* endof case 7:*/

      case 0x08: /* возвpащаем базовый адрес PCL832 */
       *((WORD16 FAR*)rp->DataPacket) = base832;
       rp->DataLength = sizeof(base832);
        break;

      case 0x09: /* возвpащаем базовый адрес PCL730 */
       *((WORD16 FAR*)rp->DataPacket) = base730;
       rp->DataLength = sizeof(base730);
        break;

      case IOCLT_GETZEROSTS: /* GetZeroSts(void) */
     {  struct Setup_Zero far * pstate;
        pstate = (struct Setup_Zero far *)rp->DataPacket;
        *pstate = SetupZero;
        rp->DataLength = sizeof(struct Setup_Zero);
     }
     break;
      case IOCLT_SETDDA_CYCLETIME:
       SetupDDAcycleTime = *((WORD16 FAR*)rp->ParmPacket);
     break;
      case  IOCLT_INIT:      /*  0x0c сбpос */
          InitPCL832(1);
     break;
     
      default:
        break;
   }
  return RPDONE; // | RPERR_COMMAND;
  }

void InitPCL832(int mode)
{
//  reset of PCL832
    outp(base832+0x1a,0);
    ibuf=0;
    lbuf=0; /* начало буфеpа, длина */
/*
    if(mode)
    {  PCL832state.ZeroErr &= ~(PCL832_ERR_OVER|PCL832_ERR_DIM);
       PCL832state.sts &= ~(PCL832_AUTO|PCL832_FINDZERO_X|PCL832_FINDZERO_Y);
   } else {
       PCL832state.sts = 0;
   }   
*/
   PCL832state.ZeroErr &= ~(PCL832_ERR_OVER|PCL832_ERR_DIM);
   PCL832state.sts = 0;
   PCL832state.NDDAint = 0; 
}
