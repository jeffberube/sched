CC = gcc
OBJ = sched.o ui.o pnode.o proc.o comm.o
FLAGS = -Wall -std=c99 -g -o
LIB = -lncurses

sched: $(OBJ)
	$(CC) $(OBJ) $(LIB) $(FLAGS) $@

.c.o:
	$(CC) -g -c $<

clean:
	rm *.o sched
