# CROSS_COMPILE = arm-none-linux-gnueabi-
CROSS_COMPILE = arm-linux-gnueabi-

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld

CFLAGS = -g -Wall -O2 -nostdlib -fno-builtin -march=armv4t -marm -I$(shell pwd)/include

objs := div64.o ctype.o printf.o string.o vsprintf.o

mylibc.a: $(objs)
	${LD} -r -o $@ $^
	
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o:%.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.a *.o		
	
