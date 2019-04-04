CC	= gcc
OUT	= lprog
SRC	= chain.c editor.c init.c lp.c matrix.c or.c session.c
OBJ	= $(SRC:.c=.o)
HDR	= lp.h matrix.h or.h
LIB	= -lcurses -lcur

$(OUT):	$(OBJ)
	$(CC) -o $(OUT) $(OBJ) $(LIB)

.c.o:
	$(CC) -c $(<)

lp.o:	lp.c lp.h or.h
init.o:	init.c lp.h or.h
session.o: session.c lp.h
editor.o: editor.c lp.h or.h
or.o:	or.c or.h
matrix.o: matrix.c matrix.h
chain.o: chain.c lp.h
