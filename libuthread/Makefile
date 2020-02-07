# Target library
lib := libuthread.a
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror

all: $(lib)

$(lib): queue.o uthread.o
	ar rcs $(lib) queue.o uthread.o

%.o: %.c 
	$(CC) $(CFLAGS) -c -o $@ $< 

queue.o: queue.c queue.h

uthread.o: uthread.c uthread.h 

preempt.o: preempt.c preempt.h

clean:
	rm -f $(lib) %.o 