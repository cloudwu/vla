LUA_INC=-I/usr/local/include
LUA_LIB=-L/usr/local/bin -llua54

CFLAGS = -g -Wall

vlatest.dll : test.c vla.c
	gcc --shared -o $@ $(CFLAGS) $^ $(LUA_INC) $(LUA_LIB)

clean :
	rm -f vlatest.dll

