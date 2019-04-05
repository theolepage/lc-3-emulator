CC = gcc
CFLAGS = -Wall -Wextra -std=c99

BIN = LC-3-emulator
SRC = main.c

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(SRC) $(CFLAGS) -o $(BIN)