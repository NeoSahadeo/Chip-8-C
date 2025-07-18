CC = clang
CFLAGS = -g

SRCS = ./src/main.c ./src/cpu.c ./src/screen.c ./src/loadrom.c
TARGET = main

all: build-all

build-all: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) -lSDL3

dev: build-all
	./$(TARGET)
