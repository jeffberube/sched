CC = gcc
OBJ = sched.o pnode.o
LIB = -lncurses

sched: $(OBJ)
	$(CC) $(OBJ) $(LIB) -o $@

.c.o:
	$(CC) -c $<
