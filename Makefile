#CFLAGS = -g -I. -Wall
CFLAGS = -O -I. -Wall
INC = misc.h tcp.h
SRC = collect.c misc.c tcp.c
TAR = $(SRC) $(INC) Makefile
COLLECT = collect.o misc.c tcp.o

collect: $(COLLECT)
	${CC} ${CFLAGS} ${COLLECT} -lm -o collect

collect.tar: ${TAR}
	tar cvf collect.tar ${TAR}

depend: $(SRC)
	makedepend -Y. $(SRC)
