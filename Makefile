
entities_url = https://www.w3.org/TR/html5/entities.json

#########################

CC = gcc -Wall -Wextra
CFLAGS = -Isrc
LFLAGS = -g3 -ggdb

##########################
RM = rm -f

##########################
# ---------------------- #
##########################

exename = bits
sources_lib = src/util.c $(wildcard src/procs/*.c)
sources_exe = src/main.c
objects_lib = $(sources_lib:.c=.o)
objects_exe = $(sources_exe:.c=.o)
objects = $(objects_lib) $(objects_exe)
sources = $(sources_lib) $(sources_exe)

##########################

all: process_entities $(objects) $(exename)

$(objects): $(sources)

$(exename): $(objects)
	$(CC) $(objects) -o $(exename) $(LFLAGS)

# nothing to do here, yet
process_entities: entities.json
	ruby gen.rb > src/entities_generated.h

entities.json:
	wget $(entities_url)

clean:
	$(RM) $(objects)

distclean: clean
	$(RM) $(exename)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

