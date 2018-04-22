CFLAGS := -O0 -Og -Wall -g3 -ggdb -Werror -MD

OBJS := \
	checkblk.o \
	checkdir.o \
	checkinode.o \
	fscheck.o \
	mkfstools.o \
	printerror.o \
	readsuperblock.o \

fscheck: ${OBJS} debug.h error.h

-include *.d

clean:
	rm -f TAGS *.d *.o fscheck

tags:
	etags *.c *.h ../*.c ../*.h
