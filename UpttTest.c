/* upptTest.c */
/* 
UPTT driver demo. 
* Get global information segment structure (gis)  (function 1)
* Set milliseconds filed in gis                   (function 2)
*/
#define  INCL_DOS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#define far
#include "uptt.h"

int UpttOpen(void);
int UpttClose(void);

HFILE handle = NULLHANDLE;
GINFOSEG gis;

int main(int n, char *par[])
{
     UpttOpen();
     printf("Sizeof GINFOSEG=%i\n",sizeof(GINFOSEG));
     UpttClose();
     return 0;
}

#define IOCTLCAT  0x91
int UpttOpen(void)
{  int  ParOut, i;
   unsigned long cbreturn, cbParam;
   ULONG  Params, OldCount, ActionTaken    = 0;      /* Action taken by DosOpen */
  int rc=-1;
rc = (int) DosOpen((PSZ)"UPTT$", &handle,&ActionTaken,0,0, FILE_OPEN,
              OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE|OPEN_FLAGS_FAIL_ON_ERROR, NULL);
  if(rc)
  {  printf("\nDosOpen failed, error = %d",rc);
     if(rc == 110)printf("(ERROR_OPEN_FAILED)");
     fflush(stdout);
  } else {
/* Beep
     Params=1000;
     cbParam  = sizeof(Params);
     cbreturn = sizeof(ParOut);
     rc = (int) DosDevIOCtl(handle, IOCTLCAT, 0x01,
          (void *) &Params,sizeof(Params), &cbParam,
          (void *) &ParOut,sizeof(ParOut),&cbreturn);
*/
for(i=0; i< 40; i++)
{
/* Get gis */     
     Params=0;
     cbParam  = sizeof(Params);
     cbreturn = sizeof(gis);
     rc = (int) DosDevIOCtl(handle, IOCTLCAT, 0x02,
          (void *) &Params,sizeof(Params), &cbParam,
          (void *) &gis,sizeof(gis),&cbreturn);
     OldCount = gis.msecs;
printf("gis.msecs=%u -> ",gis.msecs);
/* Set gis.msecs */     
     Params= gis.msecs+100000000; /* i*100000000; */ 
     cbParam  = sizeof(Params);
     cbreturn = sizeof(gis);
     rc = (int) DosDevIOCtl(handle, IOCTLCAT, 0x03,
          (void *) &Params,sizeof(Params), &cbParam,
          (void *) &gis,sizeof(gis),&cbreturn);

printf("gis.msecs=%u\n",gis.msecs);
   if(OldCount >  gis.msecs)
       printf("Old value:%u, new value:%u, DosSleep(100) forever?\n");
   DosSleep(100);
}
/*

/*
     cbParam = sizeof(Params);
     cbreturn = sizeof(base730);
     rc = (int) DosDevIOCtl(handle, IOCTLCAT, 0x09,
          (void *) &Params,sizeof(Params), &cbParam,
          (void *) &base730,sizeof(base730),&cbreturn);
//     PCL724base = base730;
     GetSts();
     if(data.tick)  use_AirDry = 0x10;
*/
  }

  return rc;
}

int UpttClose(void)
{

   DosClose(handle);
   return 0;
}
