CFLAGS = -g -Wall

test.exe : test.c vla.c
	gcc -o $@ $(CFLAGS) $^

clean :
	rm -f test.exe

