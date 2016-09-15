
CC = clang -Wall -Wextra -Weffc++

all: process_entities
	$(CC) -g3 -ggdb src/main.c src/procs/*.c -Isrc -o bits

# nothing to do here, yet
process_entities: entities.json
	ruby gen.rb > src/entities_generated.h

# not a big deal yet if wget fails
entities.json:
	wget https://www.w3.org/TR/html5/entities.json || true

