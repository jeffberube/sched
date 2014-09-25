CC = gcc
OBJ = sched.o pnode.o
LIB = -lncurses

sched: $(OBJ)
	$(CC) $(OBJ) $(LIB) -g -o $@

.c.o:
	$(CC) -c $<

clean:
	rm *.o sched
