OUTDIR = .out
cc = gcc
CFLAGS = -g
SFLAGS = -fPIC -shared
LDFLAGS = -ldl

CMD ?=
VAR = LD_PRELOAD

SRC = sandbox.c
LIB_SRC = sandboxso.c

OBJ = sandbox.o
LIB_OBJ = sandboxso.o

EXE = sandbox
LIB = sandbox.so

all:
	$(cc) $(CFLAGS) $(SFLAGS) -o $(LIB) $(LIB_SRC) $(LDFLAGS)
	$(cc) $(CFLAGS) -o $(EXE) $(SRC) $(LDFLAGS)

.PHONY: all format clean run debug

format:
	clang-format -i *.c

debug:
	gdb ./$(EXE) $(CMD)

clean:
	rm -f ./$(EXE) ./$(LIB)
