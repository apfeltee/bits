
commandlinedir = ../optionparser/include
CXX     = g++ -std=c++17 -Wall -Wextra
CFLAGS  = -Isrc -I$(commandlinedir)
CFLAGS += -O3
LFLAGS += -O3
#CFLAGS += -g3 -ggdb
#LFLAGS  = -g3 -ggdb

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
generated_src    = $(generated_ent) $(generated_pseudo)
objects_lib      = $(sources_lib:.cpp=.o)
objects_exe      = $(sources_exe:.cpp=.o)
objects          = $(objects_lib) $(objects_exe)
sources          = $(sources_lib) $(sources_exe)

##########################

.PHONY: all

all: $(exename)

#$(exename): $(sources) $(headers) $(objects) $(generated_src)
$(exename): $(headers) $(generated_src) $(objects)
	$(CXX) $(objects) -o $(exename) $(LFLAGS)

#$(sources): $(generated_src) $(headers)

#$(objects): $(sources) 

$(generated_ent): $(ent_jsonsrc)
	ruby gen_entities.rb < $(ent_jsonsrc) > $(generated_ent)

$(generated_pseudo): $(pseudo_jsonsrc)
	ruby gen_pseudo.rb < $(pseudo_jsonsrc) > $(generated_pseudo)

$(ent_jsonsrc):
#	wget $(ent_urlsrc) -O $(ent_jsonsrc)

%.o: %.cpp $(headers)
	$(CXX) -c $< -o $@ $(CFLAGS)

src/procs/url.cpp: $(headers)
src/procs/case.cpp: $(headers)
src/procs/dump.cpp: $(headers)
src/procs/html.cpp: $(headers)
src/procs/trim.cpp: $(headers)
src/procs/count.cpp: $(headers)
src/procs/rot13.cpp: $(headers)
src/procs/subst.cpp: $(headers)
src/procs/base64.cpp: $(headers)
src/procs/sponge.cpp: $(headers)
src/procs/pseudo.cpp: $(headers)
src/procs/entropy.cpp: $(headers)

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

# best not to delete entities.json - it never really changes
# and it will break the build when offline!
springclean: distclean
#	$(RM) $(ent_jsonsrc)
	$(RM) $(generated_ent)
	$(RM) $(generated_pseudo)

rebuild: distclean all
springrebuild: springclean all


