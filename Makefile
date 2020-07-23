program_NAME := bomberman
program_SRCS := $(wildcard src/*.cpp)
program_OBJS := ${program_SRCS:.cpp=.o}
program_HEADERS := $(wildcard src/*.h)
program_INCLUDE_DIRS := /usr/include/SDL2
program_LIBRARY_DIRS :=
program_LIBRARIES := SDL2 SDL2main SDL2_image SDL2_ttf
program_DEPS := ${program_OBJS:.o=.d}

CXX=g++
CXXFLAGS=-std=c++14 -Wall -pedantic -MMD -MP

CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDLIBS += $(foreach library,$(program_LIBRARIES),-l$(library))

.PHONY: all generate clean compile run doc

all: generate

generate: $(program_OBJS)

compile: $(program_NAME)

$(program_NAME): $(program_OBJS)
	$(LINK.cc) $(program_OBJS) -o $@ $(LDLIBS)

run:
	@ ./$(program_NAME)
	
doc:
	@doxygen Doxyfile

clean:
	@- rm -f $(program_NAME)
	@- rm -f src/*.o
	@- rm -f src/*.d
	@- rm -rf doc

-include $(program_DEPS)
