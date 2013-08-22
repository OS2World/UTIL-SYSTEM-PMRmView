#
# PMRmView - PM frontend for RMVIEW.
#
# Makefile for Watcom C/C++
#
# ver 1.0, 06 Dec 1999
#
# Public domain by:
#   Jari Laaksonen
#   Arkkitehdinkatu 30 A 2
#   FIN-33720 Tampere
#   FINLAND
#
#   email: jari.j.laaksonen@nokia.com
#

CFLAGS = -w4 -zq -bt=os2 -e25
LINK   = wlink SYS os2v2 pm op st=0x8000 d all op m op q op symf op c op maxe=25
OBJS   = PMRmView.obj FileUtil.obj
LOBJS  = PMRmView.obj,FileUtil.obj
PROJ   = PMRmView

WCC    = wcc386
WPP    = wpp386

!ifdef DEBUG
CFLAGS = $(CFLAGS) -d2 -od -D_DEBUG
!else
CFLAGS = $(CFLAGS) -otmlra -s -DNDEBUG
!endif

$(PROJ).exe: $(OBJS) $(PROJ).res
  $(LINK) NAME $(PROJ) FIL $(LOBJS)
  RC $(RFLAGS) $(PROJ).res $(PROJ).exe

.c.obj:
  $(WCC) $(CFLAGS) $<

$(PROJ).RES: $(PROJ).RC resource.h
  RC -r $*.RC

PMRmView.obj: PMRmView.c resource.h

FileUtil.obj: FileUtil.c

clean:
  del $(OBJS) $(PROJ).sym
