CC=gcc
CFLAGS=-I.
D_BIN=../bin
D_INC=../include
DEPS = $(D_INC)/user_manager.h $(D_INC)/socket_server.h $(D_INC)/request_handler.h
OBJ = $(D_BIN)/user_manager.o $(D_BIN)/socket_server.o $(D_BIN)/request_handler.o
MAIN=main.c

%.o: %.c $(DEPS)
	$(CC) -c -o $(D_BIN)/$@ $<

server: $(OBJ)
	$(CC) -O0 -Wall -c $(MAIN) -o $(D_BIN)/$@ $^ $(CFLAG)

release: $(OBJ)
	$(CC) -O2 -Wall -c $(MAIN) -o $(D_BIN)/$@ $^ $(CFLAG)

clean:
	rm -r $(D_BIN)/*