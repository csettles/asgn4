CFLAGS = -Wall -ansi -pedantic -g

all: mem.o mytar.o dir_tree.o
	gcc $(CFLAGS) -o mytar $^

mytar.o: mytar.c
	gcc $(CFLAGS) -c $<

mem.o: mem.c
	gcc $(CFLAGS) -c $<

dir_tree.o: dir_tree.c
	gcc $(CFLAGS) -c $<

clean:
	ls -al *.o
	rm *.o
