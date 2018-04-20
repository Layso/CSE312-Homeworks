leysö: clean allcpp



execName = sim8080

args = PrintNumbers.com 3

extraFilesToClear = exe.mem




allc: compilec linkc run

allcansi: compilecansi linkc run

allcpp: compilecpp linkcpp run




compilec:
	gcc -c *.c

compilecansi:
	gcc -c *.c -ansi -Wall -pedantic -error

compilecpp:
	g++ -c *.cpp




linkc:
	gcc -o $(execName) *.o

linkcpp:
	g++ -o $(execName) *.o




run:
	./$(execName) $(args)




clean: clear
	rm -f *.o $(execName) $(extraFilesToClear)

clear:
	clear

