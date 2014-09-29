/************************************************************************\
**               OS/2(r) Physical Device Driver Libraries               **
\************************************************************************/
/* 
DISKRET= 0   PCL730
       = 1   PCL724
       = 2   PCL724 Версия ЛАТЕК

*/
#define  DISKRET 2

int configuration= DISKRET;

// Strategy.c
//
// Device driver strategy entry point and dispatch table

#include <devhelp.h>
#include <devtype.h>
#include <devrp.h>
#include <conio.h>
#include "buffer.h"
#include "PCL832.h"

// Declare strategy entry points that are located in other source files
//
// StratInit  - Initialization entry point [Init.c]
// StratIOCtl - Audio IOCtl interface [IOCtl.c]

WORD16 StratInit(RP FAR* _rp);
WORD16 StratIOCtl(RP FAR* _rp);
WORD16 StratRemove(RP FAR* _rp);
WORD16 StratShutdown(RP FAR* _rp);
WORD16 StratError(RP FAR* _rp);
WORD16 StratOpen(RP FAR* _rp);
WORD16 StratClose(RP FAR* _rp);
WORD16 StratRead(RP FAR* _rp);
WORD16 StratWrite(RP FAR* _rp);

// Strategy dispatch table
//
// This table is used by the strategy routine to dispatch strategy requests

typedef WORD16 (*RPHandler)(RP FAR* rp);
RPHandler StratDispatch[] =
  {
  StratInit,                  // 00 (BC): Initialization
  StratError,                 // 01 (B ): Media check
  StratError,                 // 02 (B ): Build BIOS parameter block
  StratError,                 // 03 (  ): Unused
  StratRead,                 // 04 (BC): Read
  StratError,                 // 05 ( C): Nondestructive read with no wait
  StratError,                 // 06 ( C): Input status
  StratError,                 // 07 ( C): Input flush
  StratWrite,                 // 08 (BC): Write
  StratError,                 // 09 (BC): Write verify
  StratError,                 // 0A ( C): Output status
  StratError,                 // 0B ( C): Output flush
  StratError,                 // 0C (  ): Unused
  StratOpen,                 // 0D (BC): Open
  StratClose,                 // 0E (BC): Close
  StratError,                 // 0F (B ): Removable media check
  StratIOCtl,                 // 10 (BC): IO Control
  StratError,                 // 11 (B ): Reset media
  StratError,                 // 12 (B ): Get logical unit
  StratError,                 // 13 (B ): Set logical unit
  StratRemove,                // 14 ( C): Deinstall character device driver
  StratError,                 // 15 (  ): Unused
  StratError,                 // 16 (B ): Count partitionable fixed disks
  StratError,                 // 17 (B ): Get logical unit mapping of fixed disk
  StratError,                 // 18 (  ): Unused
  StratError,                 // 19 (  ): Unused
  StratError,                 // 1A (  ): Unused
  StratError,                 // 1B (  ): Unused
  StratShutdown,              // 1C (BC): Notify start or end of system shutdown
  StratError,                 // 1D (B ): Get driver capabilities
  StratError,                 // 1E (  ): Unused
  StratError                  // 1F (BC): Notify end of initialization
  };



// Strategy entry point
//
// The strategy entry point must be declared according to the STRATEGY
// calling convention, which fetches arguments from the correct registers.

// For now, we'll declare it as extern "C", so that the name is exported
// without being mangled.  That way, it can be accessed by the header
// data structure in header.asm.  (See that file for more info.)

extern "C" VOID  InitPCL730(void);
extern "C" VOID  InitPCL724(void);

extern "C" VOID Strategy(RP FAR* rp);


#pragma aux (STRATEGY) Strategy;

VOID Strategy(RP FAR* rp)
  {
  if (rp->Command < sizeof(StratDispatch)/sizeof(StratDispatch[0]))
    rp->Status = StratDispatch[rp->Command](rp);
  else
    rp->Status = RPDONE | RPERR_COMMAND;

  return;
  }


//extern HCONTEXT Hook1;

extern "C" VOID int_hand(void);
#pragma aux (STRATEGY) int_hand;

extern "C" VOID int_ErrHand(void);
#pragma aux (STRATEGY) int_ErrHand;

extern "C" VOID int_Err730Hand(void);
#pragma aux (STRATEGY) int_Err730Hand;

int ReservedIRQ=5;
int ErrorIRQ=7;
int Error730IRQ=10;
int base832=0x240;
int base730 = 0x300;

int SetVelocity(short int v, short int v0, short int a0);

volatile struct PCL832_state  PCL832state = {0, 0,0,0,0,0, 0,0, 0,0,0,0};
volatile struct Setup_Zero SetupZero = {0,0, 0,0};


int PCL832_set_dda_pulse(void);
int PCL832_Handset_dda_pulse(void);
int PCL832_FindZeroX(void);
int PCL832_FindZeroY(void);


extern  struct PCL832interp  buf[PCL832BUF_SIZE];
extern volatile int ibuf, lbuf; /* начало буфеpа, длина */
extern volatile int IsInterrupt,IsWrite;
volatile int SetupDDAcycleTime;

/* Прерывание по переполнению буфера ошибки  */
VOID int_ErrHand(void)
{
//Tell OS we can reset interrupt

    PCL832state.ZeroErr |= PCL832_ERR_OVER;
    PCL832state.sts |= 0xf00;
    PCL832state.handT0 = 0;
     DevEOI(ReservedIRQ);
}

/* Прерывание по  ошибке  - выход за габариты, пеpегpузка, и дp. */
VOID int_Err730Hand(void)
{
//Tell OS we can reset interrupt
    if(!(PCL832state.ZeroErr & PCL832_ERR_DIM))
    {      PCL832state.handT0 = 0;
           PCL832state.ZeroErr |= PCL832_ERR_DIM;
         if(PCL832state.v > 100)
         {   if(PCL832state.a0 < 20) PCL832state.a0 = 20;
         } else if(PCL832state.v > 20) {
             if(PCL832state.a0 < 10) PCL832state.a0 = 10;
         } else if(PCL832state.v > 0) {
             if(PCL832state.a0 < 2) PCL832state.a0 = 2;
         } else if(PCL832state.v > -10) {
             if(PCL832state.a0 < 2) PCL832state.a0 = 2;
         } else if(PCL832state.v > -20) {
             if(PCL832state.a0 < 10) PCL832state.a0 = 10;
         } else  {
             if(PCL832state.a0 < 20) PCL832state.a0 = 20;
         }
    }

    PCL832state.ZeroErr |= PCL832_ERR_DIM;

    PCL832state.sts |= 0xf000;
     DevEOI(ReservedIRQ);
}

VOID int_hand(void)
{
   IsInterrupt = 1;
   PCL832state.NDDAint++;

//Tell OS we can reset interrupt
     DevEOI(ReservedIRQ);

   if(PCL832state.sts & PCL832_AUTO)
                  PCL832_set_dda_pulse();
   else if( PCL832state.sts & PCL832_FINDZERO_X)
                  PCL832_FindZeroX();

   else if( PCL832state.sts & PCL832_FINDZERO_Y)
                  PCL832_FindZeroY();

   else
                  PCL832_Handset_dda_pulse();

//Call our hook which will trigger back to app
//           DevCtxArm(Hook1, (WORD32) 0);

    if(SetupDDAcycleTime)
    {     outpw(base832+0x04,SetupDDAcycleTime);
          SetupDDAcycleTime = 0;
    }
//?? //Tell OS we can reset interrupt
//     DevEOI(ReservedIRQ);
   IsInterrupt = 0;

}
/***********************************************/
/*  поиск нуля и устpанение пеpекоса           */
/***********************************************/


int PCL832_FindZeroX(void)
{
static short  int dda_pulse,v,kon, kon1, kon2;

   v = PCL832state.vx;
   if(PCL832state.ZeroErr & PCL832_ERR_OVER)
   {   PCL832state.handT0 = 0;
       PCL832state.sts &= ~PCL832_FINDZERO_X;
       return 0;
   }
   kon = GetInputTTL();

   PCL832state.kon = kon;
  if(kon & (PCL730_OVERLOAD|PCL730_OUTWORKZONE))
  {    PCL832state.handT0 = 0;
       PCL832state.sts &= ~PCL832_FINDZERO_X;
       return 0;
  }

  kon1 = 0;
  if(kon & PCL730_ZERO_X0) kon1 = 1;
  kon2 = 0;
  if(kon & PCL730_ZERO_X1) kon2 = 1;
  if(kon1) SetupZero.stskon1 = 1;
  if(kon2) SetupZero.stskon2 = 1;
  if(SetupZero.stskon1 && SetupZero.stskon2)
  {     PCL832state.handT0 = 0;
        SetupZero.stskon1=2;
        SetupZero.stskon2=2;
        PCL832state.x=0;
        PCL832state.vx0 = 0;
        PCL832state.sts &= ~PCL832_FINDZERO_X;
        PCL832state.sts &= (~PCL832_MOVE);
        PCL832state.ZeroErr |= PCL832_ZEROX_SET;
        return 0;
  }

   if(PCL832state.handT0 >0 )
   {   PCL832state.vx = SetVelocity(PCL832state.vx, PCL832state.vx0, PCL832state.a0);
//      PCL832state.handT0--;
   } else {
     if(PCL832state.vx == 0)
     {
        PCL832state.sts &= (~PCL832_MOVE);
        return 0;
     } else if(PCL832state.vx > 0)
     {   PCL832state.vx -= PCL832state.a0;  //тоpможение
         if(PCL832state.vx  < 0) PCL832state.vx =0;
     } else {
         PCL832state.vx += PCL832state.a0;  //тоpможение
         if(PCL832state.vx  > 0) PCL832state.vx =0;
     }
   }

  PCL832state.sts |= PCL832_MOVE;
  dda_pulse = PCL832state.vx;
  if(dda_pulse < 0)
        dda_pulse = (-dda_pulse) | 0x8000 ;
  PCL832state.x +=  PCL832state.vx;
       if(!SetupZero.stskon1)
       {    SetupZero.x1 += PCL832state.vx;
            outpw(base832+0x00,dda_pulse);
       }
       if(!SetupZero.stskon2)
       {    SetupZero.x2 += PCL832state.vx;
            outpw(base832+0x08,dda_pulse);
       }
       PCL832state.sts &= ~PCL832_DIR_X;
       if(PCL832state.vx < 0)
                 PCL832state.sts |= PCL832_DIR_X;

    outp(base832+0x18,0); //разрешаем цикл
   return 0;
}

int PCL832_FindZeroY(void)
{
static short  int dda_pulse,kon;

   kon = GetInputTTL();

   PCL832state.kon = kon;
   if(PCL832state.ZeroErr & PCL832_ERR_OVER)
   {   PCL832state.handT0 = 0;
       PCL832state.sts &= ~PCL832_FINDZERO_Y;
       return 0;
   }

  if(kon & (PCL730_OVERLOAD|PCL730_OUTWORKZONE))
  {    PCL832state.handT0 = 0;
       PCL832state.sts &= ~PCL832_FINDZERO_Y;
        SetupZero.stskon1=0x10;
        SetupZero.stskon2= kon;
       return 0;
  }
  if(kon & PCL730_ZERO_Y)
  {     PCL832state.handT0 = 0;
        SetupZero.stskon1=2;
        SetupZero.stskon2= kon;
        SetupZero.x2 = GetInputTTL();
        PCL832state.y=0;
        PCL832state.sts &= ~PCL832_FINDZERO_Y;
        PCL832state.sts &= (~PCL832_MOVE);
        PCL832state.ZeroErr |= PCL832_ZEROY_SET;
        PCL832state.vy0 = 0;
       return 0;
  }

   if(PCL832state.handT0 >0 )
   {   PCL832state.vy =
            SetVelocity(PCL832state.vy, PCL832state.vy0, PCL832state.a0);
   } else {
     if(PCL832state.vy == 0)
     {  SetupZero.stskon1=0x20;
        PCL832state.sts &= (~PCL832_MOVE);
        SetupZero.stskon2= kon;
        return 0;
     } else if(PCL832state.vy > 0)
     {   PCL832state.vy -= PCL832state.a0;  //тоpможение
         if(PCL832state.vy  < 0) PCL832state.vy =0;
     } else {
         PCL832state.vy += PCL832state.a0;  //тоpможение
         if(PCL832state.vy  > 0) PCL832state.vy =0;
     }
   }

  PCL832state.sts |= PCL832_MOVE;
  dda_pulse = PCL832state.vy;
  if(dda_pulse < 0)
        dda_pulse = (-dda_pulse) | 0x8000 ;

   PCL832state.y += PCL832state.vy;
   SetupZero.x1 += PCL832state.vy;
   outpw(base832+0x10,dda_pulse);
   PCL832state.sts &= ~PCL832_DIR_Y;
   if(PCL832state.vy < 0)
                 PCL832state.sts |= PCL832_DIR_Y;

    outp(base832+0x18,0); //разрешаем цикл
   return 0;
}



/***********************************************/

/* pабота в pучном pежиме */
int PCL832_Handset_dda_pulse(void)
{
static short  int dda_pulse, kon;

   kon = GetInputTTL();
   PCL832state.kon = kon;

   if(PCL832state.handT0 >0 )
   {
/* контpолиpование ошибочных команд по выходу в pабочую зону из залета */
      if((kon & PCL730_XERR_MINUS) && PCL832state.vx0 < 0)
                   PCL832state.vx0 = 0;
      else
         if((kon & (PCL730_ZERO_X0|PCL730_ZERO_X1)) && PCL832state.vx0 < 0)
                   PCL832state.vx0 /= 2;

      if((kon & PCL730_XERR_PLUS) && PCL832state.vx0 > 0)
                   PCL832state.vx0 = 0;

      if((kon & PCL730_ZERO_Y) && PCL832state.vy0 < 0)
                  PCL832state.vy0 /= 2;
      else
       if((kon & PCL730_YERR_MINUS) && PCL832state.vy0 < 0)
                   PCL832state.vy0 = 0;

      if((kon & PCL730_YERR_PLUS) && PCL832state.vy0 > 0)
                   PCL832state.vy0 = 0;

      PCL832state.vx = SetVelocity(PCL832state.vx, PCL832state.vx0, PCL832state.a0);
      PCL832state.vy = SetVelocity(PCL832state.vy, PCL832state.vy0, PCL832state.a0);
      PCL832state.handT0--;
   } else {

     if(PCL832state.vx == 0 && PCL832state.vy == 0 )
     {   PCL832state.sts &= (~PCL832_MOVE);
        return 0;
     }
     if(PCL832state.vx > 0) {
         PCL832state.vx -= PCL832state.a0;  //тоpможение
         if(PCL832state.vx  < 0) PCL832state.vx =0;
     } else {
         PCL832state.vx += PCL832state.a0;  //тоpможение
         if(PCL832state.vx  > 0) PCL832state.vx =0;
     }

     if(PCL832state.vy > 0) {
         PCL832state.vy -= PCL832state.a0;  //тоpможение
         if(PCL832state.vy  < 0) PCL832state.vy =0;
     } else {
         PCL832state.vy += PCL832state.a0;  //тоpможение
         if(PCL832state.vy  > 0) PCL832state.vy =0;
     }

   }

  PCL832state.sts |= PCL832_MOVE;
  dda_pulse = PCL832state.vx;
  if(dda_pulse < 0)
        dda_pulse = (-dda_pulse) | 0x8000 ;
  if(PCL832state.sts &PCL832_MOVE_X)
  {
       PCL832state.x +=  PCL832state.vx;
       outpw(base832+0x00,dda_pulse);
       outpw(base832+0x08,dda_pulse);
       PCL832state.sts &= ~PCL832_DIR_X;
       if(PCL832state.vx < 0)
                 PCL832state.sts |= PCL832_DIR_X;
  }

  dda_pulse = PCL832state.vy;
  if(dda_pulse < 0)
        dda_pulse = (-dda_pulse) | 0x8000 ;

  if(PCL832state.sts &PCL832_MOVE_Y)
  {
       PCL832state.y += PCL832state.vy;
       outpw(base832+0x10,dda_pulse);
       PCL832state.sts &= ~PCL832_DIR_Y;
       if(PCL832state.vy < 0)
                 PCL832state.sts |= PCL832_DIR_Y;
  }

    outp(base832+0x18,0); //разрешаем цикл
   return 0;
}

/* pабота в автоматическом pежиме */
int PCL832_set_dda_pulse(void)
{
static short  int dda_pulse, kon;

   kon = GetInputTTL();
   PCL832state.kon = kon;
   if(PCL832state.ZeroErr & PCL832_ERR_OVER)
          return 0;
  if(kon & (PCL730_OVERLOAD|PCL730_OUTWORKZONE))
  {    PCL832state.handT0 = 0;
       PCL832state.sts &= ~PCL832_AUTO;
       return 0;
  }

   if(!lbuf)
   {   PCL832state.sts &= (~PCL832_MOVE);
       PCL832state.vy = PCL832state.vx = 0;
       return 1; /* буфер пуст - делать нечего */
   }
   PCL832state.sts |= PCL832_MOVE;

//    temp = (unsigned short int) (ddatime / 0.511875);
//    outpw(base832+0x04,10); // пока сделаем 5 миллисек

//void PCL832_enable_dda(void)
//    outp(base832+0x18,0); //разрешаем цикл


//а теперь спокойненько установим значение для следующих циклов
       dda_pulse = buf[ibuf].dx[0];
       PCL832state.vx = dda_pulse; // текущие скорости по x для автомата
       PCL832state.kadr = buf[ibuf].dx[2];

       PCL832state.x += dda_pulse;
       if(dda_pulse < 0)
             dda_pulse = (-dda_pulse) | 0x8000 ;

       outpw(base832+0x00,dda_pulse);

//       dda_pulse = buf[ibuf].dx[1];
//мля !       if(dda_pulse < 0)
//           dda_pulse = (-dda_pulse) | 0x8000 ;

       outpw(base832+0x08,dda_pulse);

       dda_pulse = buf[ibuf].dx[1];
//       dda_pulse = buf[ibuf].dx[2];
       PCL832state.vy = dda_pulse; // текущие скорости по y для автомата

       PCL832state.y += dda_pulse;
       if(dda_pulse < 0)
             dda_pulse = (-dda_pulse) | 0x8000 ;

//  *( (int far * )  (&DebBuf[2])) =  (int ) dda_pulse;
// dda_pulse  = 20;
// dda_pulse |= 0x8000;
       outpw(base832+0x10,dda_pulse);

//void PCL832_enable_dda(void) ???
    outp(base832+0x18,0); //разрешаем цикл

//декрементируем буфер
   if(buf[ibuf].dt)
   {   buf[ibuf].dt--;
   } else {
     lbuf--;
     ibuf++;
     if(ibuf>=PCL832BUF_SIZE)
                         ibuf = 0;
   }

   return 0;
}
/* по текущей скорости V,заданной v0 и ускорению a0 выдать следующую скорость */
int SetVelocity(short int v, short int v0, short int a0)
{
   if(v0 >= 0)
   {  if(v <= v0)
      {     v += a0;  // pазгон
            if(v > v0) v = v0;
      } else {
            v -= a0;  // торможение
            if(v < v0) v = v0;
      }
   } else {
      if(v >= v0)
      {      v -= a0;  // pазгон
             if(v < v0) v = v0;
      } else {
             v += a0;  // торможение
             if(v > v0) v = v0;
      }
   }
   return v;
}


int GetInputTTL(void)
{  int rc,rc1;
#if DISKRET == 0
     rc = inpw(base730);
#elif  DISKRET == 1

    rc = 0;
     rc1 = inpw(base832+0x02);
     if((rc1 & 0x01))
       rc |= PCL730_ZERO_X0;
     if(!(rc1 & 0x02))
       rc |= PCL730_ZEROZONE_X;

     rc1 = inpw(base832+0x0a);
     if((rc1 & 0x01))
       rc |= PCL730_ZERO_X1;
//     if(rc1 & 0x02)
//       rc |= PCL730_ERR_DIM | PCL730_XERR_PLUS |PCL730_XERR_MINUS 
//                            | PCL730_YERR_PLUS |PCL730_YERR_MINUS;


     rc1 = inpw(base832+0x12);
     if(!(rc1 & 0x02))
       rc |= PCL730_ZEROZONE_Y;
     if((rc1 & 0x01))
       rc |= PCL730_ZERO_Y;
#else

    rc = 0;
     rc1 = inpw(base832+0x02);
     if(!(rc1 & 0x01))
       rc |= PCL730_ZERO_X0;
     if(!(rc1 & 0x02))
       rc |= PCL730_ZEROZONE_X;

     rc1 = inpw(base832+0x0a);
     if(!(rc1 & 0x01))
       rc |= PCL730_ZERO_X1;
     rc1 = inpw(base832+0x12);
     if(!(rc1 & 0x02))
       rc |= PCL730_ZEROZONE_Y;
     if(!(rc1 & 0x01))
       rc |= PCL730_ZERO_Y;

#endif

    return rc;
}


/* Инициализация PCL724 */
void InitPCL724(void)
{ 
/* инициализация    */
   outp(base730+3,PCL724mode);
/* сброс привода */
   outp(base730,0);
}

void InitPCL730(void)
{ 
/* инициализация    */
   outp(base730,0);
}
