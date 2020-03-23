CC =gcc
CFLAGS =-g -Wall

all: hw1

%o: %c
	$(CC) -c $(CFLAGS) $<

.PHONY: all hw1 run debug clean

hw1: hw1.o
	$(CC) -o $@ $^

run:
	./hw1

run_by_root:
	sudo ./hw1

debug: 
	gdb ./hw1

clean:
	$(RM) hw1 *.o
