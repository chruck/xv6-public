CFLAGS := -Og -Wall -ggdb -Werror -MD

OBJS := fscheck.o printerror.o checkinode.o checkdir.o checkblk.o readsuperblock.o

fscheck: ${OBJS} debug.h error.h

-include *.d

clean:
	rm -f TAGS *.d *.o fscheck

tags:
	etags *.c *.h ../*.c ../*.h
