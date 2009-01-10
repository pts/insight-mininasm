## Makefile for building mkhelp.com
## Copyright (c) 2007 - 2009, Oleg O. Chukaev <oleg.chukaev@mail.ru>
##
## Used programs:
##   GNU Make 3.79.1
##   Open Watcom C 1.5

CC = wcl
CFLAGS = -zq -mt -0 -bcl=com -wx

all:	mkhelp.com

mkhelp.com:	mkhelp.c
	$(CC) $(CFLAGS) mkhelp.c

clean:
	del mkhelp.obj
	del mkhelp.com

