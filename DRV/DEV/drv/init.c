/************************************************************************\
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
#include "PCL832.h"

 char far * drv_strstr(char far *buf, char *find);
extern "C" VOID  InitPCL730(void);
extern "C" VOID  InitPCL724(void);

extern "C" VOID int_hand(void);
#pragma aux (STRATEGY) int_hand;

extern "C" VOID int_ErrHand(void);
#pragma aux (STRATEGY) int_ErrHand;

extern "C" VOID int_Err730Hand(void);
#pragma aux (STRATEGY) int_Err730Hand;

extern int ReservedIRQ;
extern int ErrorIRQ;
extern int Error730IRQ;

extern int base832; /* =0x240; */
extern int base730; /* =0x300; */

extern volatile  struct PCL832_state  PCL832state;
extern int configuration;
// Constant debugging strings (only defined when DEBUG is defined)


#define            MSG_DEBUG              0
#define            MSG_CS                 0
#define            MSG_DS                 0


// Sign on and installation messages

static const char WELCOME[] =
  "\r\n"
  "PCL832 Driver v1.02 Installed.\r\n"
  "(C) ShaturaSoft 1999-2000.\r\n";

const int WELCOME_LENGTH = sizeof(WELCOME) - 1;


// Initialize device driver

WORD16 StratInit(RP FAR* _rp)
  {

  RPInit FAR* rp = (RPInit FAR*)_rp;
  int result=0,i,j;
static  char istr0[]="\r\nПpеpывания:";
static  char istr1[]="\r\nCan't register interrupt";
static  char istr2[]="\r\nАдрес:";
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
  cdbg << ALL << endl;
  cdbg << MSG_DEBUG << endl;
  cdbg << MSG_CS << hex << setw(4) << OffsetFinalCS
       << setw() << dec << 'h' << endl;
  cdbg << MSG_DS << hex << setw(4) << OffsetFinalDS
       << setw() << dec << 'h' << endl;
  cdbg << endl;
  // Print a sign on message to the console.
  DosPutMessage(1, WELCOME_LENGTH, WELCOME);
  st[0]='0'+configuration;
  st[1]=0;
  DosPutMessage(1, 1, st);
  DosPutMessage(1, 10, " вариант\r\n");
 
  if( rp->In.Args)
  { 

      for(i=0;rp->In.Args[i] && i < 80;i++);

      DosPutMessage(1, 12, "Аpгументы:\r\n");
      DosPutMessage(1, i, rp->In.Args);
/* ищем /IRQ: */
      s =  drv_strstr( rp->In.Args,"/IRQ:");
      if(s)
      {  
         j = 0; i = 0;
         s += 5;
         if(s[0] >= '0')
         {  if(s[0] <= '9') i = ((int)s[0])-(int)'0';
         } 
         if(s[1] >= '0')
         {  if(s[1] <= '9') j = ((int)s[1])-(int)'0';
            i = i*10+j;
         } 
ReservedIRQ = i;
      }

/* ищем /ERIRQ: */
      s =  drv_strstr( rp->In.Args,"/ERIRQ:");
      if(s)
      {  j = 0; i = 0;
         s += 7;
         if(s[0] >= '0')
         {  if(s[0] <= '9') i = ((int)s[0])-(int)'0';
         } 
         if(s[1] >= '0')
         {  if(s[1] <= '9') j = ((int)s[1])-(int)'0';
            i = i*10+j;
         } 
ErrorIRQ = i;
      }

/* ищем /IOIRQ: */
      s =  drv_strstr( rp->In.Args,"/IOIRQ:");
      if(s)
      {  j = 0; i = 0;
         s += 7;
         if(s[0] >= '0')
         {  if(s[0] <= '9') i = ((int)s[0])-(int)'0';
         } 
         if(s[1] >= '0')
         {  if(s[1] <= '9') j = ((int)s[1])-(int)'0';
            i = i*10+j;
         } 
Error730IRQ = i;
      }

/* ищем /IO:  PCL832 */
      s =  drv_strstr( rp->In.Args,"/IO:");
      if(s)
      { 
         j = 0; i = 0;
         s += 4;
         if(s[0] >= '0')
         {  if(s[0] <= '9') i = ((int)s[0])-(int)'0';
            else if(s[0] >= 'A'&& s[0] <= 'F') i = 10 + ((int)s[0])-(int)'A';
         } 
         if(s[1] >= '0')
         {  if(s[1] <= '9') j = ((int)s[1])-(int)'0';
            else if(s[1] >= 'A'&& s[1] <= 'F') j = 10 + ((int)s[1])-(int)'A';
            i = i*16+j;
         } 
         if(s[2] >= '0')
         {  if(s[2] <= '9') j = ((int)s[2])-(int)'0';
            else if(s[2] >= 'A'&& s[2] <= 'F') j = 10 + ((int)s[2])-(int)'A';
            i = i*16+j;
         } 
base832 = i;

      }

/* ищем адpес base730 */
      s =  drv_strstr( rp->In.Args,"/IO1:");
      if(s)
      { 
         j = 0; i = 0;
         s += 5;
         if(s[0] >= '0')
         {  if(s[0] <= '9') i = ((int)s[0])-(int)'0';
            else if(s[0] >= 'A'&& s[0] <= 'F') i = 10 + ((int)s[0])-(int)'A';
         } 
         if(s[1] >= '0')
         {  if(s[1] <= '9') j = ((int)s[1])-(int)'0';
            else if(s[1] >= 'A'&& s[1] <= 'F') j = 10 + ((int)s[1])-(int)'A';
            i = i*16+j;
         } 
         if(s[2] >= '0')
         {  if(s[2] <= '9') j = ((int)s[2])-(int)'0';
            else if(s[2] >= 'A'&& s[2] <= 'F') j = 10 + ((int)s[2])-(int)'A';
            i = i*16+j;
         } 
base730 = i;

      }
  }
   /*  endofif( rp->In.Args) */

/* Выводим сообщение о используемых IRQ */
         DosPutMessage(1, 13, istr0);
         st[2]=0; st[1]=0;
         st[0] = (char)(ReservedIRQ/10 +(int)'0');
         st[1] = (char)(ReservedIRQ%10 +(int)'0');
         DosPutMessage(1, 2, st);
         DosPutMessage(1, 2, ", ");

         st[2]=0; st[1]=0;
         st[0] = (char)(ErrorIRQ/10 +(int)'0');
         st[1] = (char)(ErrorIRQ%10 +(int)'0');

         DosPutMessage(1, 2, st);
         DosPutMessage(1, 2, ", ");

         st[2]=0; st[1]=0;
         st[0] = (char)(Error730IRQ/10 +(int)'0');
         st[1] = (char)(Error730IRQ%10 +(int)'0');
         DosPutMessage(1, 2, st);

/* Выводим сообщение о используемом адресе */
         DosPutMessage(1, sizeof(istr2), istr2);
         st[3]=0; st[2]=0; st[1]=0;
         i = base832>>8;
         st[0] = (char)(i +(int)'0');
         if(i >= 10) st[0] = (char)(i +(int)'A');

         i = (base832>>4)&0xf;
         st[1] = (char)(i +(int)'0');
         if(i >= 10) st[1] = (char)(i +(int)'A');

         i = base832&0xf;
         st[2] = (char)(i +(int)'0');
         if(i >= 10) st[2] = (char)(i +(int)'A');

         DosPutMessage(1, 3, st);
         DosPutMessage(1, 2, ", ");

         st[3]=0; st[2]=0; st[1]=0;
         i = base730>>8;
         st[0] = (char)(i +(int)'0');
         if(i >= 10) st[0] = (char)(i +(int)'A');

         i = (base730>>4)&0xf;
         st[1] = (char)(i +(int)'0');
         if(i >= 10) st[1] = (char)(i +(int)'A');

         i = base730&0xf;
         st[2] = (char)(i +(int)'0');
         if(i >= 10) st[2] = (char)(i +(int)'A');

         DosPutMessage(1, 3, st);

//  reset of PCL832
         outp(base832+0x1a,0);
     PCL832state.sts = 0;
//инициализируем платы PCL730 или PCL724
     if(configuration == 0)
              InitPCL730();
      else
              InitPCL724();


// Резервируем прерывание
      if(DevIRQSet( ErrorIRQ,int_ErrHand,0))
      {
         DosPutMessage(1, sizeof(istr1), istr1);
         DosPutMessage(1,9 , " ErrorIRQ");
      }

      if(DevIRQSet( Error730IRQ,int_Err730Hand,0))
      {
         DosPutMessage(1, sizeof(istr1), istr1);
         DosPutMessage(1,12 , " Error730IRQ");
      }
	
      if(DevIRQSet( ReservedIRQ,int_hand,0))
      {
         DosPutMessage(1, sizeof(istr1), istr1);
         DosPutMessage(1,12 , " ReservedIRQ");
      }

  //user code
  return RPDONE;
}

char far * drv_strstr(char far *buf, char *find)
{  int i,j,is;
   for(i=0;buf[i] && i<80;i++)
   {  if(buf[i] == find[0])
      {  is=1; 
         for(j=0;find[j] && j<80;j++)
         {  if(!buf[i+j]) { is = 0; break; }
            if(buf[i+j] != find[j]) {is = 0; break; }
         }
         if(is) return &buf[i];
      }
   } 
   return 0;
}

