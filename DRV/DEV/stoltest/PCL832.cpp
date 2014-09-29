/* PCL832.cpp */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "inoutp.h"
#include "PCL832.hpp"

static int base832=0x240;

/* reset PCL-832 */
void PCL832_reset(void)
{
        OutPort(base832+0x1a,0);
}

void PCL832_enable_dda(void)
{
        OutPort(base832+0x18,0);
}

int PCL832_set_dda_cycle_time(int ddatime)
{
  unsigned short int temp;

        if ( ddatime < 0 || ddatime > 2000 )
          return -1;

    temp = (unsigned short int) (ddatime / 0.511875);
    OutPortW(base832+0x04,temp);
        return 0;
}

int PCL832_set_dda_pulse(int axis, int dir, int ldda_pulse)
{  unsigned short  dda_pulse;

        if ( axis < 1 || axis > 3 )
          return -1;

        if ( dir != CW && dir != CCW )
          return -2;

        if ( ldda_pulse < 0 || ldda_pulse > 4095 )
          return -3;
        dda_pulse =  ldda_pulse;

        if ( dir == CCW )
          dda_pulse |= 0x8000;

        switch (axis)
              {
                case 1: OutPortW(base832+0x00,dda_pulse);
                        break;
                case 2: OutPortW(base832+0x08,dda_pulse);
                        break;
                case 3: OutPortW(base832+0x10,dda_pulse);
                        break;
              }
        return 0;
}

int PCL832_set_gain(int axis, int gain)
{
        if ( axis < 1 || axis > 3 )
          return -1;

        if ( gain < 0 || gain > 63 )
          return -2;

        switch (axis)
              {
                case 1: OutPort(base832+0x02,gain);
                        break;
                case 2: OutPort(base832+0x0a,gain);
                        break;
                case 3: OutPort(base832+0x12,gain);
                        break;
              }
        return 0;

}

int PCL832_get_errcnt(int axis, int *errcnt)
{
        if ( axis < 1 || axis > 3 )
          return -1;

        switch (axis)
              {
                case 1: *errcnt = InPortW(base832+0x00);
                        break;
                case 2: *errcnt = InPortW(base832+0x08);
                        break;
                case 3: *errcnt = InPortW(base832+0x10);
                        break;
              }

        if ( *errcnt & 0x8000 ) { *errcnt |= 0xf000; }
         else { *errcnt &= 0x0fff; }

        return 0;
}
