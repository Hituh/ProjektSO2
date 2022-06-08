Most:Most
	gcc -o Most -pthread main.c

main.o: main.c
	gcc  main.c

clean: 
	rm -f *.o Most

reload:
	rm -f *.o Most
	make