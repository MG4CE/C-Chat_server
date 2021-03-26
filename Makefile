#https://stackoverflow.com/questions/30573481/how-to-write-a-makefile-with-separate-source-and-header-directories

CC := gcc

BIN_NAME := server

BIN_DIR := bin
OBJ_DIR := obj
SRC_DIR := src

BIN_TARGET := $(BIN_DIR)/$(BIN_NAME)

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CPPFLAGS	:= -Iinclude -MMD -MP
CFLAGS 		:= -Wall
LDFLAGS 	:= -Llib
LDLIBS 		:= -lpthread

.PHONY: all clean

all: $(BIN_TARGET)

$(BIN_TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

-include $(OBJ:.o=.d)
