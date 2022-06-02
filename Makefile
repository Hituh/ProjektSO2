.SILENT:
output: main.o functions.o 
	gcc main.o functions.o -o Most

main.o: main.c
	gcc -c main.c

functions.o: functions.c functions.h
	gcc -c functions.c

clean: 
	rm -f *.o Most

reload:
	rm -f *.o Most
	make