
CC = clang -Wall -Wextra -Weffc++

all:
	$(CC) -g3 -ggdb main.c -o bits
