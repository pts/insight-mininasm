## Makefile for building Insight 1.24
## Copyright (c) 2006 - 2009 Oleg O. Chukaev <oleg.chukaev@mail.ru>
##
## Used programs:
##   Nasm 0.98.39, 2.02
##   GNU Make 3.79.1
##   mkhelp, uclpack -- in `tools' directory


## Width of screen. Only 80 and 90 columns are supported now.
WIDTH = 80

## Color scheme. data\colors_{k,b,c}.inc for black (native), blue (Norton
## Commander), cyan (Turbo Debugger) colors.
COLOR = data\colors_k.inc

AS = ../nasm
AFLAGS = -t -s -O4 -w+orphan-labels -f bin -DSCR_WIDTH=$(WIDTH) -DCOL_SCH=$(COLOR)

all:	insight.com

tools:
	cd ..\tools
	make
	cd ..\src

insight.com: insight.asm *.inc data\page1.xlp data\page2.xlp makefile	\
	$(COLOR)
	$(AS) $(AFLAGS) -o insight.com insight.asm

data\page1.xlp:	data\page1.hlp makefile $(COLOR)
	..\tools\mkhelp data\page1.hlp data\ins_help.tmp $(WIDTH) $(COLOR)
	..\tools\uclpack -R --10 --nrv2b data\ins_help.tmp data\page1.xlp

data\page2.xlp:	data\page2.hlp makefile $(COLOR)
	..\tools\mkhelp data\page2.hlp data\ins_help.tmp $(WIDTH) $(COLOR)
	..\tools\uclpack -R --10 --nrv2b data\ins_help.tmp data\page2.xlp

clean:
	del insight.com
	del data\page1.xlp
	del data\page2.xlp
	del data\ins_help.tmp

realclean:	clean
	cd ..\tools
	make clean
	cd ..\src

