CFLAGS = -pedantic -Wall -Wextra -g -fsanitize=address
SRC = src/main.c
LDLIBS = -lSDL3

bezier: src/main.c
	cc $(CFLAGS) ${SRC} -o main.o $(LDLIBS)
