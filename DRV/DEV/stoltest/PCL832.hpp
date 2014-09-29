/* PCL832.hpp */


#define         CW       0
#define         CCW      1


void PCL832_enable_dda(void);
void PCL832_reset(void);
int PCL832_set_dda_cycle_time(int ddatime);
int PCL832_set_dda_pulse(int axis, int dir, int dda_pulse);
int PCL832_set_gain(int axis, int gain);
int PCL832_get_errcnt(int axis, int *errcnt);
