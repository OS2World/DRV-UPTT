/* readme.txt */
UPTT
UPTime Test driver

UPTT can be used for changing milliseconds  counter in
global information segment structure (gis), those thing   
that wraps every 49 days of uptime and what is get via
 
rc = DosQuerySysInfo(QSV_MS_COUNT, QSV_MS_COUNT, (void *)&ulBuffer, sizeof(unsigned long));

or simply at address    (int *)(((0x0DFF4& 0xfff8)<< 0x0D)+4);

The same thing  go.exe shows as uptime.


To setup driver add to config.sys

device=uptt.sys 

To test diver run UpttTest.exe and look at UpttTest.c
=======================================================
This test shows how DosSleep suxxes at 49 day of uptime.
Полный писец для серверных приложений и риал-тайма.