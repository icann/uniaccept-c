#
#

CFLAGS	= -g -Wall -Werror
RM	= /bin/rm
RMFLAGS	= -f

chktld: chktld.o

chktld.o: chktld.c

clean:
	${RM} ${RMFLAGS} *.o

distclean: clean
	${RM} ${RMFLAGS} chktld *~ \#* .gdb_history


