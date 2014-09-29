#ifndef ___INOUTPORT___
#define ___INOUTPORT___

#ifdef __cplusplus
extern "C" {
#endif

    unsigned char  _Pascal  InPort(unsigned short int port);
    void  _Pascal  OutPort(unsigned short int port, unsigned char data);


   unsigned short int  _Pascal InPortW(unsigned short int port);
   void  _Pascal OutPortW(unsigned short int port, unsigned short int data);

#ifdef __cplusplus
};
#endif

#endif

