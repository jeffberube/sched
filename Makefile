CC = gcc
OBJ = sched.o ui.o pnode.o proc.o
FLAGS = -Wall -g -std=c99 -o
LIB = -lncurses

sched: $(OBJ)
	$(CC) $(OBJ) $(LIB) $(FLAGS) $@

.c.o:
	$(CC) -c $<

clean:
	rm *.o sched
