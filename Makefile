#make for pthread_pool lib
OBJS=pthread_pool.c
CFLAGS=-fPIC -shared
all:
	gcc $(CFLAGS) -o libhot.so $(OBJS)


clean:
	-rm *.o 
	-rm *.so
