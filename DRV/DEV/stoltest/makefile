# makefile
# Created by IBM WorkFrame/2 MakeMake at 17:06:31 on 24 Sept 1999
#
# The actions included in this make file are:
#  Compile::C++ Compiler
#  Link::Linker

.SUFFIXES: .cpp .obj 

.all: \
    D:\EVGEN\STOLTEST\stoltest.exe

.cpp.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Ti /Fo"%|dpfF.obj" /C %s

D:\EVGEN\STOLTEST\stoltest.exe: \
    D:\EVGEN\STOLTEST\PCL832.obj \
    D:\EVGEN\STOLTEST\test.obj \
    {$(LIB)}stoltest.def
    @echo " Link::Linker "
    icc.exe @<<
     /B" /de"
     /FeD:\EVGEN\STOLTEST\stoltest.exe 
     stoltest.def
     D:\EVGEN\STOLTEST\PCL832.obj
     D:\EVGEN\STOLTEST\test.obj
<<

D:\EVGEN\STOLTEST\PCL832.obj: \
    D:\EVGEN\STOLTEST\PCL832.cpp \
    {D:\EVGEN\STOLTEST;$(INCLUDE);}inoutp.h \
    {D:\EVGEN\STOLTEST;$(INCLUDE);}PCL832.hpp

D:\EVGEN\STOLTEST\test.obj: \
    D:\EVGEN\STOLTEST\test.cpp \
    {D:\EVGEN\STOLTEST;$(INCLUDE);}inoutp.h \
    {D:\EVGEN\STOLTEST;$(INCLUDE);}PCL832.hpp
