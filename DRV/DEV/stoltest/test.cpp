#define  INCL_DOS
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "inoutp.h"
//#include "portio.h"
#include "PCL832.hpp"
#include "STOL_I.hpp"
#define UP    72
#define DOWN  80
#define LEFT  75
#define RIGHT 77

/*+---------------------------------+*/
/*| EXternal function prototypes.   |*/
/*+---------------------------------+*/


/*+---------------------------------+*/
/*| Internal function prototypes.   |*/
/*+---------------------------------+*/
int TEST_PCL730(void);
int TEST_PCL832(void);
int TESTa_PCL832(int axis);
int TEST_PCL832_DRV(void);
int TEST_interpolPCL832_DRV(void);

void _Optlink goodbye(void)
{
    /* This function is called at normal program termination                   */
    printf("\ngoodbye from LS_IO\n");
}


void main()
{   int rc;
    static  int X=0,Y=0,DX=1024,DY=1024,STEP=1;
     char str[80];
    unsigned int M1,M2;
    printf("Тест устройств ЛТК\n(с)Коцюба Е.В.\n");


    OutPort(0x70,0x17);
    M1 = InPort(0x71);
    OutPort(0x70,0x18);
    M2 = InPort(0x71);
    printf("CMOS - Ext. Mem = %u\n",M1 + (M2<<8));

    for(int j=0; j<5; j++)
    {
        unsigned char A = InPort(0x61);
        A|=3;
        OutPort(0x61,A);
        DosSleep(100);
        A&=(unsigned char)(-4);
        OutPort(0x61,A);
        DosSleep(100);
    }

M0: printf("\n");

   printf("1 - Тест PCL730\n");
   printf("2 - Тест PCL832\n");
//   printf("3 - Тест L54/Датчики уpовня\n");
//   printf("4 - Тест шагового пpивода\n");
   printf("0,Q - Выход\n"); fflush(stdout);
   rc = _getch();
   switch(rc)
   {
      case '1' : TEST_PCL730();
          break;

      case '2' : TEST_PCL832();
          break;
      case 'q' :
      case 'Q' :
      case '0' :
         exit(0);
          break;
  }
    goto M0;
}

int GetNbufPCL832_DRV(void);
int Get1NbufPCL832_DRV(void);
int Get2NbufPCL832_DRV(void);

int TEST_PCL832(void)
{  int rc, i;
static int rdata[32],d32=0;
static int base=0x240;
   char str[80];

M:
  for(i=0;i<16;i++)
  {  rdata[i]= InPortW(base+i*2);
  }
   printf("\n0(%x %x) 1(%x %x) 2(%x %x) \n",rdata[0],rdata[1], rdata[4],rdata[5], rdata[8],rdata[9]);

   printf("\n");
   printf("1 - Reset\n");
   printf("2 - Записать\n");
   printf("3 - Базовый адрес %x\n",base);
   printf("4 - Тест движения 0\n");
   printf("5 - Тест движения 1\n");
   printf("6 - Тест движения 2\n");
   printf("7 - Тест драйвера\n");
   printf("8 - Тест интеpполятоpа драйвера\n");
   printf("9 - Сколько в буфере\n");
   printf("a - test буфера\n");
   printf("b - test IOPL буфера\n");
   printf("0,Q - Выход\n"); fflush(stdout);
   rc = _getch();

   switch(rc)
   {
      case '1' :
             OutPort(base+0x1a,0);
          break;
      case '3' :
       printf("addr="); fflush(stdout);
       gets(str);
       sscanf(str,"%x",&base);
          break;
      case '4' :
          TESTa_PCL832(0+1);
          break;
      case '5' :
          TESTa_PCL832(1+1);
          break;
      case '6' :
          TESTa_PCL832(2+1);
          break;

      case '7' :
          TEST_PCL832_DRV();
          break;
      case '8' :
          TEST_interpolPCL832_DRV();
          break;

      case '9' :
          GetNbufPCL832_DRV();
          break;
      case 'a' :
          Get1NbufPCL832_DRV();
          break;
      case 'b' :
          Get2NbufPCL832_DRV();
          break;

      case 'q' :
      case 'Q' :
      case '0' :
         return 0;
          break;
   }
  goto M;
   return 0;
}

HFILE TEST_PCL832_DRV_Open(void);
int TEST_PCL832_DRV_IOCTL(HFILE  driver_handle);
int TEST_PCL832_WRITE(HFILE  driver_handle);

int InterpolateDetail(void);

int TEST_interpolPCL832_DRV(void)
{
   int rc;
   ULONG ulWrote;
extern class PCL832interpBuffer PCL832Buf;
   HFILE  driver_handle  = 0L;

   PCL832_reset();      /* reset PCL-832 */
   PCL832_set_dda_cycle_time(100);    /* set DDA cycle time = 24ms */
   PCL832_set_gain(1, 1);            /* set axis 1 gain = 1 */
   PCL832_set_gain(2, 1);            /* set axis 2 gain = 1 */
   PCL832_set_gain(3, 1);            /* set axis 3 gain = 1 */
   InterpolateDetail();

    driver_handle = TEST_PCL832_DRV_Open();

    rc = DosWrite (driver_handle,                /* File handle */
                   (PVOID) PCL832Buf.pBuf,         /* String to be written */
                   PCL832Buf.n*sizeof(struct PCL832interp),      /* Size of string to be written */
                   &ulWrote);                   /* Bytes actually written */

   printf("DosWrite return: %i, ulWrote = %i",rc,ulWrote);
   PCL832_enable_dda();         /* enable PCL-832 dda pulse generator */


   DosClose(driver_handle);
   return 0;

}


void  DDA_interrupt_routine(int axis);
int DDA_irq, OV_irq, int_mask, iddaflag = 0;
int overflow_status = 0;
int dda_pulse = 0;


int TESTa_PCL832(int axis)
{
   int i,j,raz;
   int error, status;
   int dda_pulse_data[100];

   dda_pulse_data[ 0] =  0;
   dda_pulse_data[ 1] =  10; dda_pulse_data[ 2] =  20;
   dda_pulse_data[ 3] =  40; dda_pulse_data[ 4] =  60;
   dda_pulse_data[ 5] =  80; dda_pulse_data[ 6] =  90;
   dda_pulse_data[ 7] = 100; dda_pulse_data[ 8] = 100;
   dda_pulse_data[ 9] = 100; dda_pulse_data[10] = 100;
   dda_pulse_data[11] =  90; dda_pulse_data[12] =  80;
   dda_pulse_data[13] =  60; dda_pulse_data[14] =  40;
   dda_pulse_data[15] =  20; dda_pulse_data[16] =  10;
   dda_pulse_data[17] =   0;

   PCL832_reset();      /* reset PCL-832 */
   PCL832_set_dda_cycle_time(100);    /* set DDA cycle time = 24ms */
   PCL832_set_gain(1, 1);            /* set axis 1 gain = 1 */
   PCL832_set_gain(2, 1);            /* set axis 2 gain = 1 */
   PCL832_set_gain(3, 1);            /* set axis 3 gain = 1 */

   PCL832_enable_dda();         /* enable PCL-832 dda pulse generator */
   DosSleep(100);

   /* move 1st axis 1000 pulses in CW direction */

for(raz=0;raz<2;raz++)
{
   i = 0; dda_pulse = 0; iddaflag = 0;
   printf("*****************************\n");
   for(j=0;j<17;j++)
   {  DDA_interrupt_routine(axis);
      DosSleep(1);

    do
    {
      PCL832_get_errcnt(axis, &error);
      printf("The error counter value of axis 1 = %05d.\n",error);
      if ( overflow_status == 1 )
      {
          printf("The error counter is overflow and PCL-832 has been reset.\n");
          printf("please press any key to end program. \n");
      }
    } while(abs(error) > 2);

      if ( iddaflag == 1 && i < 18 )
           {
              dda_pulse = dda_pulse_data[i];
              if(!raz) dda_pulse = -dda_pulse;
              iddaflag = 0;
              if ( i < 17 ) i++;
              if ( i == 17 )
                {
                   printf("please press any key to end program. \n");
                }
           }
      }
   }
   return 0;
}

void  DDA_interrupt_routine(int axis)
{
//  disable();

  if ( iddaflag == 0 )
    {
       if ( dda_pulse >= 0 )
         {  PCL832_set_dda_pulse(axis, 0, dda_pulse); }
        else
         {
            dda_pulse *= -1;
            PCL832_set_dda_pulse(axis, 1, dda_pulse);
         }

       dda_pulse = 0;
    }
  iddaflag = 1;

//  outportb(0x20,0x20);
//  if ( DDA_irq > 7 )
//    { outportb(0xa0,0x20); }
//  enable();

//   return 0;
}



int TEST_PCL730(void)
{  int rc, i;
static int data[4],d32=0;
static int base=0x300;
   char str[80];

TEST1:
   printf("\n%3x %3x %3x %3x",data[0], data[1],data[2],data[3]);
   printf("\n");
   printf("1 - Прочитать\n");
   printf("2 - Записать\n");
   printf("3 - Базовый адрес %x\n",base);
   printf("0,Q - Выход\n"); fflush(stdout);
   rc = _getch();

   switch(rc)
   {
      case '1' :
         for(i=0;i<4;i++)
         {  data[i]= InPort(base+i);
         }
          break;
      case '2' :
       printf("32bit hex="); fflush(stdout);
       gets(str);
       sscanf(str,"%x",&d32);
         for(i=0;i<4;i++)
         {   data[i] = d32&0xff;
             OutPort(base+i,data[i]);
             d32 = d32 >> 8;
         }
          break;
      case '3' :
       printf("addr="); fflush(stdout);
       gets(str);
       sscanf(str,"%x",&base);
          break;
      case 'q' :
      case 'Q' :
      case '0' :
         return 0;
          break;
   }
   goto TEST1;
}


int TEST_PCL832_DRV(void)
{
   HFILE  driver_handle  = 0L;
   driver_handle = TEST_PCL832_DRV_Open();

   TEST_PCL832_DRV_IOCTL(driver_handle);
   TEST_PCL832_WRITE(driver_handle);
   DosClose(driver_handle);
   return 0;
}

int TEST_PCL832_WRITE(HFILE  driver_handle)
{  int rc;
   ULONG ulWrote;
   int uData[16], *uDp;
   uDp = uData;
   printf("Write: %p\n",uDp);

    rc = DosWrite (driver_handle,                /* File handle */
                   (PVOID) uData,         /* String to be written */
                   sizeof (uData),        /* Size of string to be written */
                   &ulWrote);                   /* Bytes actually written */

   printf("DosWrite return: %i, ulWrote = %i",rc,ulWrote);
   return 0;
}

HFILE TEST_PCL832_DRV_Open(void)
{
   HFILE  driver_handle  = 0L;     /* Handle for file being manipulated */
   ULONG  ActionTaken    = 0;      /* Action taken by DosOpen */
   ULONG  ulBytesRead    = 0;      /* Number of bytes read by DosRead */
    ULONG  ulWrote        = 0;      /* Number of bytes written by DosWrite */
    ULONG  ulLocal        = 0;      /* File pointer position after DosSetFilePtr */
  int rc=-1;
  int sound=0;
rc = DosOpen((PSZ) "PCL832$",
                      &driver_handle,
                      &ActionTaken,
                      0,
                      0,
                      FILE_OPEN,
                      OPEN_SHARE_DENYNONE
                        | OPEN_ACCESS_READWRITE,
                      NULL);
if(rc)
         {
         printf("\nDosOpen failed, error = %d",rc);
         if(rc == 110)printf("(ERROR_OPEN_FAILED)");
         fflush(stdout);
         getchar();
         DosExit(EXIT_PROCESS,0);
         }

  printf("rc=%i,driver_handle=%i,ActionTaken=%i\n",
          rc,driver_handle,ActionTaken);
  return driver_handle;

}

#define IOCTLCAT  0x91

int Get1NbufPCL832_DRV(void)
{  int rc;
HFILE  driver_handle;

unsigned iReturn;
unsigned long cbreturn;
unsigned short Params;
unsigned long cbParam;

   driver_handle = TEST_PCL832_DRV_Open();

     iReturn=20;
     Params=0;
     cbParam = sizeof(Params);
     rc = DosDevIOCtl(driver_handle, IOCTLCAT, 0x04,(void *) &Params,sizeof(Params),&cbParam,
                        (void *) &iReturn,sizeof(iReturn),&cbreturn);
        printf ("You sent the driver,  %u .\n", Params);
        printf ("Returned from the driver,  %x ", iReturn);
     Params=1;
     rc = DosDevIOCtl(driver_handle, IOCTLCAT, 0x04,(void *) &Params,sizeof(Params),&cbParam,
                        (void *) &iReturn,sizeof(iReturn),&cbreturn);
        printf (",  %x \n", iReturn);

     Params=2;

     rc = DosDevIOCtl(driver_handle, IOCTLCAT, 0x04,(void *) &Params,sizeof(Params),&cbParam,
                        (void *) &iReturn,sizeof(iReturn),&cbreturn);
        printf ("Returned from the driver,  %x ", iReturn);
     Params=3;

     rc = DosDevIOCtl(driver_handle, IOCTLCAT, 0x04,(void *) &Params,sizeof(Params),&cbParam,
                        (void *) &iReturn,sizeof(iReturn),&cbreturn);
        printf (",  %x \n", iReturn);
     Params=4;

     rc = DosDevIOCtl(driver_handle, IOCTLCAT, 0x04,(void *) &Params,sizeof(Params),&cbParam,
                        (void *) &iReturn,sizeof(iReturn),&cbreturn);
        printf ("Returned from the driver,  %x ", iReturn);
     Params=5;

     rc = DosDevIOCtl(driver_handle, IOCTLCAT, 0x04,(void *) &Params,sizeof(Params),&cbParam,
                        (void *) &iReturn,sizeof(iReturn),&cbreturn);
        printf (",  %x ,\n", iReturn);
     Params=6;

     rc = DosDevIOCtl(driver_handle, IOCTLCAT, 0x04,(void *) &Params,sizeof(Params),&cbParam,
                        (void *) &iReturn,sizeof(iReturn),&cbreturn);
        printf ("Returned from the driver,  %x ", iReturn);

     Params=7;
     rc = DosDevIOCtl(driver_handle, IOCTLCAT, 0x04,(void *) &Params,sizeof(Params),&cbParam,
                        (void *) &iReturn,sizeof(iReturn),&cbreturn);
        printf (",  %x \n", iReturn);

     Params=8;
     rc = DosDevIOCtl(driver_handle, IOCTLCAT, 0x04,(void *) &Params,sizeof(Params),&cbParam,
                        (void *) &iReturn,sizeof(iReturn),&cbreturn);
        printf ("Returned from the driver,  %x ", iReturn);

     Params=9;
     rc = DosDevIOCtl(driver_handle, IOCTLCAT, 0x04,(void *) &Params,sizeof(Params),&cbParam,
                        (void *) &iReturn,sizeof(iReturn),&cbreturn);
        printf (",  %x \n", iReturn);

   DosClose(driver_handle);

   return  iReturn;
}

int Get2NbufPCL832_DRV(void)
{  int rc;
HFILE  driver_handle;

unsigned iReturn;
unsigned long cbreturn;
unsigned short Params[32];
unsigned long cbParam;
    short int s;
   int i;
   for(s=i=0;i<6;i++)
   { Params[i] = i;
     s += Params[i];
   }
   driver_handle = TEST_PCL832_DRV_Open();

     iReturn=20;
     cbParam = sizeof(Params);
     rc = DosDevIOCtl(driver_handle, IOCTLCAT, 0x05,(void *) &Params,sizeof(Params),&cbParam,
                        (void *) &iReturn,sizeof(iReturn),&cbreturn);
        printf ("You sent the driver,  %u .\n", s);
        printf ("Returned from the driver,  %u ", iReturn);


   DosClose(driver_handle);

   return  iReturn;
}

int GetNbufPCL832_DRV(void)
{  int rc;

HFILE  driver_handle;

unsigned short iReturn;
unsigned long cbreturn;
unsigned short Params;
unsigned long cbParam;

   driver_handle = TEST_PCL832_DRV_Open();

     iReturn=20;
     Params=500;
     cbParam = sizeof(Params);
     rc = DosDevIOCtl(driver_handle, IOCTLCAT, 0x03,(void *) &Params,sizeof(Params),&cbParam,
                        (void *) &iReturn,sizeof(iReturn),&cbreturn);
        printf ("You sent the driver,  %u .\n", Params);
        printf ("Returned from the driver,  %d ,  cbParam =%i\n", iReturn, cbParam);
   DosClose(driver_handle);

   return  iReturn;
}


int TEST_PCL832_DRV_IOCTL(HFILE  driver_handle)
{  int rc;

unsigned short iReturn;
unsigned long cbreturn;
unsigned short Params;
unsigned long cbParam;

     iReturn=20;
     Params=500;
     cbParam = sizeof(Params);
     rc = DosDevIOCtl(driver_handle, IOCTLCAT, 0x01,(void *) &Params,sizeof(Params),&cbParam,
                        (void *) &iReturn,sizeof(iReturn),&cbreturn);
        printf ("You sent the driver,  %u .\n", Params);
        printf ("Returned from the driver,  %d ,  cbParam =%i\n", iReturn, cbParam);

    rc = DosDevIOCtl(driver_handle, IOCTLCAT, 0x02,(void *) &Params,sizeof(Params),&cbParam,
                        (void *) &iReturn,sizeof(iReturn),&cbreturn);
        printf ("Returned from the driver,  %d ,  cbParam =%i\n", iReturn, cbParam);
   return 0;
}
