/******************************************/
/* 横���᪨� ���p                      */
/******************************************/
#define PCL832BUF_SIZE 4096
//#define PCL832BUF_SIZE 512

struct PCL832interp
{  short int dx[3],dt;
//   long int id;
};

/* �������� � ���p n-����⮢ */
int AddBufer( struct PCL832interp  FAR * pd, int n);

int DelBufer(void);/* 㤠���� ���� ����� �� ���p�*/
int GetBuferLength(void);
int PCL832_set_dda_pulse(void);
void InitPCL832(int mode);
