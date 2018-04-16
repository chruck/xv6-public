CFLAGS := -Og -Wall -ggdb -Werror -MD

fscheck: fscheck.o printerror.o checkinode.o checkdir.o checkblk.o

clean:
	rm -f TAGS *.o fscheck

tags:
	etags *.c *.h
