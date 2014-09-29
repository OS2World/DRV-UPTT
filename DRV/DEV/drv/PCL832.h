/* PCL832.h */

#define         CW       0
#define         CCW      1


#define IOCTLCAT  0x91
/* �㭪樨 IOCTL */
#define    IOCLT_GETNUMPOINTS_BUFFER 0x03 /* ����p�頥� �᫮ �祪 �  ���p�*/
#define    IOCLT_GETSTS              0x06 /* GetSts(void) */
#define    IOCLT_SETSTS              0x07 /* SetMode(void) */
#define    IOCLT_GETADDRPCL832       0x08 /* ��p���� ������ ��p�� PCL832 */
#define    IOCLT_GETADDRPCL730       0x09 /* ��p���� ������ ��p�� PCL730 */
#define    IOCLT_GETZEROSTS          0x0a /* GetZeroSts(void) (struct Setup_Zero)*/
#define    IOCLT_SETDDA_CYCLETIME    0x0b /*  */
#define    IOCLT_INIT                0x0c /* �p�� */


/* cmd
   1 - ��p�室 � p���� ��⮬���᪮�� �p�������
   2 - ��p�室 � p���� p�筮��   �p�������
   3 - ���������� ��p�室 � p���� ��⠭���
   4 - ��⠭����� ��p����p� p�筮�� X
   5 - ��⠭����� ��p����p� p�筮�� Y
   6 - ��⠭����� 0 (set home)
*/
struct PCL832control
{  short int cmd;     /* ������� */ 
   short int arg[3];  /* �p�㬥���*/
};

#define  PCL832_MOVE   0x01
#define  PCL832_AUTO   0x02
#define  PCL832_MOVE_X 0x04
#define  PCL832_DIR_X  0x08
#define  PCL832_MOVE_Y 0x10
#define  PCL832_DIR_Y  0x20
#define  PCL832_FINDZERO_X 0x40
#define  PCL832_FINDZERO_Y 0x80

#define  PCL832_ZEROX_SET     0x01
#define  PCL832_ZEROY_SET     0x02
#define  PCL832_ERR_OVER      0x04
#define  PCL832_ERR_DIM       0x08


/* ���� ���楢����  PCL730 */
#define  PCL730_OVERLOAD     0x0020
#define  PCL730_OUTWORKZONE  0x0001

#define  PCL730_ZERO_Y       0x8000
#define  PCL730_ZEROZONE_Y   0x4000
#define  PCL730_ZERO_X1      0x2000
#define  PCL730_ZERO_X0      0x1000
#define  PCL730_ZEROZONE_X   0x0800

#define  PCL730_YERR_MINUS      0x0010
#define  PCL730_YERR_PLUS       0x0008
#define  PCL730_XERR_MINUS      0x0004
#define  PCL730_XERR_PLUS       0x0002
#define  PCL730_ERR_DIM         0x0001

#define  PCL730_ZATROR_READY    0x0080 // ��⢮p ��⮢

#define  PCL730_OUT_WATCH       0x8000 // ������ "�������"
#define  PCL730_OUT_WATER       0x2000 // ������ "����"
#define  PCL724mode             0x8B

/*
   01 - IRQ ��室 �� �������  
   02 - ��室 �� ࠡ���� ���� �+
   04 - ��室 �� ࠡ���� ���� �-
   08 - ��室 �� ࠡ���� ���� Y+
   10 - ��室 �� ࠡ���� ���� Y- ~30��
   20 - 㯮� (�ॢ�襭�� ⮪� ?) + 䨪��� �訡�� + �ॡ���� ������ "����"
 0800 - ���� ���  X
 1000 - ���� �- (Y=max)
 2000 - ���� �- (Y=0)
 4000 - ���� ���  Y  ~56 �� - ��⮬ ~10�� ��ઠ
 8000 - ���  Y  ~3��
*/

/* ��������� ��� ���⠢����� ��� �� � � ���p������ ��p���� */
struct Setup_Zero
{
  short int stskon1; /* ���ﭨ� ���楢��� 1 �p���⠫ - ��/��� 1/0 */
  short int stskon2; /* ���ﭨ� ���楢��� 2 �p���⠫ - ��/��� 1/0 */
  long int x1;       /* ��������� ���楢��� 1 �� X */
  long int x2;       /* ��������� ���楢��� 2 �� X */
};

struct PCL832_state
{  short int sts;  // ���ﭨ�
/* 0x0/&0x1 - �⮨�/��������, 
   &0x02 - p�筮�/��⮬���᪮�  0/1
   &0x04 - �������� X          0/1 
   &0x08 - ���p.�������� X+/-  0/1 
   &0x10 - �������� Y          0/1 
   &0x20 - ���p.�������� Y+/-  0/1
*/   
/* ���p����⠫�� ���稪 ��� p�筮�� �p�������, �᫨ 0 - ��稭����� �p������� */
   short int handT0; 
   short int v;   /* ⥪��� ᪮p���� p�筮�� */ 
   short int v0;  /* ᪮p���� p�筮��   */ 
   short int a0;  /* �᪮p����  p�筮�� */
   short int ZeroErr; 
/* ���� �� ��⠭�����-������ - ��/��� 1/0... 
   PCL832_ZEROX_SET PCL832_ZEROY_SET PCL832_ERR_OVER PCL832_ERR_DIM 
*/ 
   long int x,y;
   short int vx,vy;   // ⥪�騥 ᪮��� �� x,y ��� ��⮬��
   short int vx0,vy0; // ᪮p��� p�筮�� �� x,y 
   short int dx0,dx1,dy; 
   short int NDDAint; // ���稪 DDA ���뢠���
   short int kon; // ���ﭨ� ���楢���� � �p��. �室��� p�����p��
   unsigned short int kadr; // ����騩 ����� ����
   short int lbuf;           // ����� ����
   short int reserved;  
 // add 09.04.2000
   struct Setup_Zero zerostate; // ��������� ��� ���⠢����� ��� �� � � ���p������ ��p���� 
};


/***************************************************/
class STOL_PCL832
{
 public:
    HFILE  handle; /* 奭�� ��� p����� � �p����p�� */
    struct PCL832_state data;

    STOL_PCL832(void)
    {  handle = 0L;
    }
  int Open(void);
  int Close(void);
  int GetSts(void);
  int SetMode(int nand,int handT0, int v0, int a0);
  int Stop(void);
  int SetHome(void);
  int GetPos(int *X,int *Y);
};