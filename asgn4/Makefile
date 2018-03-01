CFLAGS = -Wall -ansi -pedantic -g

all: mem.o mytar.o array_list.o dir_tree.o
	gcc $(CFLAGS) -o mytar $^

mytar: mytar.c mem.c array_list.c dir_tree.c
	
	gcc $(CFLAGS) -c $<
mytar.o: mytar.c 
	gcc $(CFLAGS) -c $<

mem.o: mem.c
	gcc $(CFLAGS) -c $<

array_list.o: array_list.c
	gcc $(CFLAGS) -c $<

dir_tree.o: dir_tree.c
	gcc $(CFLAGS) -c $<	

clean:
	ls -al *.o
	rm *.o
