CFLAGS = -Wall -ansi -pedantic -g

all: mem.o mytar.o
	gcc $(CFLAGS) -o mytar $^

mytar.o: mytar.c
	gcc $(CFLAGS) -c $<

mem.o: mem.c
	gcc $(CFLAGS) -c $<

clean:
	ls -al *.o
	rm *.o
