#/************************************************************************\
#**               OS/2(r) Physical Device Driver Libraries               **
#   Device Library Sample Driver Make File
#===================================================================


#===================================================================
#
#   Setup the environment properly
#
#===================================================================
INC     = .\dev;.\iw



#===================================================================
#
#   Auto-dependency information
#
#===================================================================
.ERASE
.SUFFIXES:
.SUFFIXES: .lst .obj .c .asm

#CFLAGS  = -dDEBUG=0 -bt=os2 -d1 -e60 -5 -i$(INC) -omerlin -s -wx -xd -ze -zdp -zff -zgf -zl -zq -zu
CFLAGS  = -bt=os2 -d1 -e60 -5 -i$(INC) -omerlin -s -wx -xd -ze -zdp -zff -zgf -zl -zq -zu
CC      = WPP $(CFLAGS)

AFLAGS  = -q
ASM     = WASM $(AFLAGS)

DFLAGS  = -l -s
DIS     = WDISASM $(DFLAGS)

LFLAGS  = system os2 dll option quiet, map, align=512, eliminate
LINK    = WLINK $(LFLAGS)

BFLAGS  = -s -t -c -b -q -n
LIB     = WLIB $(BFLAGS)

.obj.lst:
        $(DIS) $*

.c.obj: .AUTODEPEND
        $(CC) $*.c

.asm.obj: .AUTODEPEND
        $(ASM) $*.asm



#===================================================================
#
#   List of source files
#
#===================================================================
FILE0   = dev\device.lib dev\devmisc.lib
FILE1   = header.obj init.obj ioctl.obj strategy.obj remove.obj shutdown.obj error.obj open.obj close.obj read.obj write.obj
FILE2   = 
FILE3   =
FILE4   =
FILE5   =
FILE6   =
FILE7   =
FILE8   =
FILE9   =
FILES   = $(FILE0) $(FILE1) $(FILE2) $(FILE3) $(FILE4) $(FILE5) $(FILE6) $(FILE7) $(FILE8) $(FILE9)
TARGET  = PCL832



#===================================================================
#
#   Specific dependencies
#
#===================================================================
all: $(TARGET).sys

$(TARGET).lnk: makefile
    @echo name $(TARGET).sys > $(TARGET).lnk
    @for %f in ($(FILES)) do @echo file %f >> $(TARGET).lnk

$(TARGET).sys: $(TARGET).lnk $(FILES)
    $(LINK) @$(TARGET).lnk
