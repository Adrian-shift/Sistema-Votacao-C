CC=gcc

CFLAGS=-Wall

LIBS=-lnewt -lpthread -lsqlite3

SRC=src/main.c \
    src/ui/dashboard.c \
    src/ui/results.c \
    src/models/server_state.c \
    src/network/server_socket.c \
    src/database/database.c \
    src/protocol/protocol.c

OUT=build/server

all:
	mkdir -p build
	$(CC) $(SRC) $(CFLAGS) $(LIBS) -o $(OUT)

run: all
	./$(OUT)

clean:
	rm -rf build
