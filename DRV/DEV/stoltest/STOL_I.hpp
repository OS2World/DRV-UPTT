/* STOL_I.CPP */

struct PCL832interp
{  short int dx[3],dt;
};

class PCL832interpBuffer
{
 public:
   int n; /* число элементов в буфеpе */
   int l; /* длина буфеpа */
   struct PCL832interp *pBuf;

   PCL832interpBuffer(void)
   {  pBuf = NULL;
      n = l = 0;
   }
   PCL832interpBuffer(int _l)
   {
      n = l = 0;
      pBuf = (struct PCL832interp *) calloc(_l,sizeof(struct PCL832interp));
      if(pBuf) l = _l;
   }
   int AddBuffer(int dl)
   {  if(pBuf == NULL)
      {
         pBuf = (struct PCL832interp *) calloc(dl,sizeof(struct PCL832interp));
         l = dl;
      } else {
         l += dl;
         pBuf = (struct PCL832interp *) realloc((void *)pBuf, l * sizeof(struct PCL832interp));

      }
      return 0;
   }
   int AddPoint(int dx[2], int dt)
   {  if(n+1 >= l) AddBuffer(256);
      pBuf[n].dx[0] = dx[0];
      pBuf[n].dx[1] = dx[0];
      pBuf[n].dx[2] = dx[1];
      pBuf[n].dt    = dt;
      n++;
   }

};

