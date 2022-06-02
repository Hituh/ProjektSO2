Projekt: main.c functions.o list.o
	gcc main.c functions.o list.o -g -pthread -o MainOut
	rm *.o

clean: rm 