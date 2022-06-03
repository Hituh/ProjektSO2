.SILENT:
output: main.o functions.o list.o
	gcc main.o functions.o list.o -g -pthread -o Most
	rm *.o

main.o: main.c
	gcc -c main.c

functions.o: functions.c functions.h
	gcc -c functions.c

lista.o: list.c list.h 
	gcc -c list.c

clean: 
	rm -f *.o Mosts

reload:
	rm -f *.o Most
	make