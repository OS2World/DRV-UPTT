/* STOL_I.CPP */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "STOL_I.HPP"

/* �������� ���p������ ��� PCL832 */

static double a,v; /* 䨧��᪨� �᪮p���� � ᪮p���� */

/******************************************/
/* 横���᪨� ���p                      */
/******************************************/

struct PCL832interp buf[8000];
short int ibuf=0,lbuf=0; /* ��砫� ���p�, ����� */

/* �������� � ���p n-������⮢ */
int AddBufer(struct PCL832interp *pd, int n)
{ int i,j;
  if(lbuf+n > 8000) return 1;
  j = ibuf;
  for(i=0;i<n;i++)
  { buf[j] = pd[i];
    j++;
    if(j>=8000) j = 0;
  }
  lbuf += n;
  ibuf = j;
  return 0;
}
/* 㤠���� ���� ������� �� ���p�*/
int DelBufer(void)
{
   if(lbuf)
   { lbuf--;
     ibuf++;
     if(ibuf>=8000) ibuf = 0;
   }
   return 0;
}


class PCL832interpBuffer PCL832Buf;

int InterpolateDetail(void);
int Interpolateline(double x[2],  double v0, double v, double v1, double a);


int InterpolateDetail(void)
{ int i;
  double x0[2],v0,v,v1,a;
  v0 = 0.;
  v = 10.;
  v1 = 0.;
  a = 10.;
  x0[0] = 0.;
  x0[1] = 10.;
  Interpolateline(x0, v0, v, v1, a);
  printf("��祪:%i",PCL832Buf.n);
  x0[0] = 10.;
  x0[1] = 10.;
  Interpolateline(x0, v0, v, v1, a);
  printf("��祪:%i",PCL832Buf.n);

  return 0;
}

/*
int InterpolatePline( void)
{  int i;
   MovetoStartPoint()
   CalcPoints(a,v,...)
   for(i=0;i<n;i++)
   {   Interpolateline(i);
   }

}
*/
int InterpolatelineA(double x0[2],double x1[2],double v0,double a0, double t0);

/* �������� ���p������ ����� � ��砫�� � 0, ���殬 � x,
   ��砫쭮� ᪮p����� v0, ����筮� v1, �p���p᪮� v, �᪮p����� a
*/
int Interpolateline(double x[2],  double v0, double v, double v1, double a)
{  double l,l0,l1, x00[2], x0[2],k0,k1,k2,t0,t1,t2;
   double S,S0,S1, a0, a1, a2;
   int i;
   if(a <= 0.) return 1;

   S = hypot(x[0],x[1]);
   a0 = a;
   if(v0 > v) a0 = -a;
   t0 = (v-v0)/a0;
   S0 = (v0  + a0 * t0 * 0.5) * t0;

   a1 = a;
   if(v > v1) a1 = -a;
   t1 = (v1-v)/a1;
   S1 = (v  + a1 * t1 * 0.5) * t1;

   if(S > S0 + S1) /* ��p���쭠� ���p������ */
   {  k0 = S0/S;
      for(i=0;i<2;i++) { x00[i] = 0.; x0[i] = x[i] * k0; }
      InterpolatelineA(x00,x0,v0,a0,t0); /* ��砫�� p�����*/
  printf("��祪 p�����:%i  ",PCL832Buf.n);
      k1 = (S-S1)/S;
      for(i=0;i<2;i++) { x00[i] = x0[i]; x0[i] = x[i] * k1; }
      t2 = (S-(S0+S1))/v;
      InterpolatelineA(x00,x0,v,0.,t2);
  printf("��祪  ��p�:%i  ",PCL832Buf.n);

      for(i=0;i<2;i++) { x00[i] = x0[i]; x0[i] = x[i]; }
      InterpolatelineA(x00,x0,v,0.,t1);  /* ����筮� �p������� */
  printf("��祪 �p�������:%i\n",PCL832Buf.n);

     return 0;

   }

   if(v0 == 0. && v1 == 0.) /* �祭� �����쪨� ��p���� */
   {
      k0 = 0.5;
      for(i=0;i<2;i++) { x00[i] = 0.; x0[i] = x[i] * k0; }
      t0 = sqrt(S/a);
      InterpolatelineA(x00,x0,v0,a,t0); /* ��砫�� p�����*/
      v = a*t0;
      for(i=0;i<2;i++) { x00[i] = x0[i]; x0[i] = x[i]; }
      InterpolatelineA(x00,x0,v,-a,t0); /* ����筮� �p������� */
      return 0;
   }
/* �� ����砥��� ��� �� ��㯥��� - p�����᪮p���� �����塞 ᪮p���� � v0 �� v1 */
   for(i=0;i<2;i++) { x00[i] = 0.; x0[i] = x[i]; }
   t2 = 2 * S / (v0 + v1);
   a2 = (v0 + v1) * (v1 - v0)/ (2.* S);
   InterpolatelineA(x00,x0,v0,a0,t2);

   return 0;
}

/* ���p����p������ p�����᪮p������ ��������  */
int InterpolatelineA(double x0[2],double x1[2],double v0,double a0, double t0)
{  double tstep = 0.001;
   double koeff = 100.; /* ��p���� �� �� � 0.01 ��*/
   double S,t,l;
   int i,n,ix[2],ixold[2],idx[2], idxold[2],rep;
   S = hypot(x0[0]-x1[0],x0[1]-x1[1]);
   n = t0/tstep;
   ix[0] = ix[1] = 0;
   idxold[0]=idxold[1]=-1;
   rep=0;
   for(i=1;i<n;i++)
   {  t = tstep * i;
      l = (v0  + a0 * t * 0.5) * t;
      ixold[0] = ix[0];
      ixold[1] = ix[1];
      ix[0] =  (x1[0]-x0[0]) * l /S * koeff;
      ix[1] =  (x1[1]-x0[1]) * l /S * koeff;
      idx[0] = ix[0] - ixold[0];
      idx[1] = ix[1] - ixold[1];
      if(idx[0] == idxold[0] && idx[1] == idxold[1])
      {  rep++;
      } else {
         if(rep)
         {  PCL832Buf.AddPoint(idxold,rep+1);
            rep = 0;
         } else {
            PCL832Buf.AddPoint(idxold,1);
         }
      }
      idxold[0] = idx[0];
      idxold[1] = idx[1];

   }
   if(rep)
   {  PCL832Buf.AddPoint(idxold,rep+1);
      rep = 0;
   } else {
      PCL832Buf.AddPoint(idxold,1);
   }

   return 0;
}


