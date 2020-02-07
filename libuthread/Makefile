# Target library
lib := libuthread.a
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror

all: $(lib)

$(lib): queue.o 
	ar rcs $(lib) queue.o 


queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c -o $@ $< 

clean:
	rm -f $(lib) %.o 
