
commandlinedir = ../../optionparser/include
CXX = g++ -std=c++17 -Wall -Wextra
CFLAGS = -Isrc -Os -I$(commandlinedir)
LFLAGS = -g3 -ggdb

##########################
RM = rm -f

##########################
# ---------------------- #
##########################

exename          = bits.exe
ent_urlsrc       = https://www.w3.org/TR/html5/entities.json
ent_jsonsrc      = etc/entities.json
pseudo_jsonsrc   = etc/alphabets.json
generated_ent    = src/generated/entities.h
generated_pseudo = src/generated/pseudoalphabets.h
headers          = src/private.h
sources_lib      = src/util.cpp $(wildcard src/procs/*.cpp)
sources_exe      = src/main.cpp
objects_lib      = $(sources_lib:.cpp=.o)
objects_exe      = $(sources_exe:.cpp=.o)
objects          = $(objects_lib) $(objects_exe)
sources          = $(sources_lib) $(sources_exe)

##########################

all: $(generated_ent) $(generated_pseudo) $(exename)

$(exename): $(sources) $(headers) $(objects)
	$(CXX) $(objects) -o $(exename) $(LFLAGS)

$(generated_ent): $(ent_jsonsrc)
	ruby gen_entities.rb < $(ent_jsonsrc) > $(generated_ent)

$(generated_pseudo): $(pseudo_jsonsrc)
	ruby gen_pseudo.rb < $(pseudo_jsonsrc) > $(generated_pseudo)

$(ent_jsonsrc):
	wget $(ent_urlsrc) -O $(ent_jsonsrc)

help:
	@echo "available targets:"
	@echo
	@echo "  all (or no arguments)          normal build. default."
	@echo "  clean                          remove object files, as well as other temporary files."
	@echo "  distclean                      like 'clean', but also remove executables"
	@echo "  springclean                    like 'distclean', but also remove non-default files."
	@echo "  rebuild                        invoke 'rebuild', then invoke 'all'."
	@echo "  springrebuild                  invoke 'springclean', then invoke 'all'."
	@echo

clean:
	$(RM) $(objects)

distclean: clean
	$(RM) $(exename)

springclean: distclean
	$(RM) $(ent_jsonsrc)
	$(RM) $(generated_ent)
	$(RM) $(generated_pseudo)

rebuild: distclean all
springrebuild: springclean all

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CFLAGS)

