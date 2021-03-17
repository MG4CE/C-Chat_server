CC=gcc
BIN_P=../bin
CFLAG=-lpthread
BIN_N=server

all: server

server: main.c 
	$(CC) -O0 -o $(BIN_P)/$(BIN_N) main.c $(CFLAG)

release: main.c
	$(CC) -O2 -o $(BIN_P)/$(BIN_N) main.c $(CFLAG)

clean:
	rm $(BIN_P)/$(BIN_N)
