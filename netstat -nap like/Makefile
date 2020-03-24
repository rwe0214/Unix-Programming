CC =gcc
CFLAGS =-g -Wall

all: netstat_nap_like

%o: %c
	$(CC) -c $(CFLAGS) $<

.PHONY: all netstat_nap_like run debug clean

netstat_nap_like: netstat_nap_like.o
	$(CC) -o $@ $^

run:
	./netstat_nap_like

run_by_root:
	sudo ./netstat_nap_like

debug: 
	gdb ./netstat_nap_like

clean:
	$(RM) netstat_nap_like *.o
