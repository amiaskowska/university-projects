CXX          := clang++
TARGET       := text_moths
STD          := -std=c++23
OPT_FLAGS    := -O2 -Wall -Wextra
WARN_FLAGS   := -Wno-experimental-header-units -Wno-pragma-system-header-outside-header
CXXFLAGS     := $(STD) $(OPT_FLAGS) $(WARN_FLAGS)

# system headers
SYS_HEADERS  := vector cctype string iostream unordered_map regex memory ostream istream stdexcept
SYS_PCHS     := $(addsuffix .pch, $(SYS_HEADERS))

# modules
MODULES      := parser colony text moth
MODULE_OBJS  := $(addsuffix .o, $(MODULES))
MODULE_PCMS  := $(addsuffix .pcm, $(MODULES))

# flags generation
GET_PCM_FLAGS = $(foreach pcm, $(filter %.pcm, $^), -fmodule-file=$(basename $(notdir $(pcm)))=$(pcm))
GET_PCH_FLAGS = $(addprefix -fmodule-file=, $(filter %.pch, $^))
ALL_MOD_FLAGS = $(GET_PCM_FLAGS) $(GET_PCH_FLAGS)

.PHONY: all clean

all: $(TARGET)

$(TARGET): main.o $(MODULE_OBJS)
	$(CXX) $^ -o $@

# compilationss
%.pch:
	$(CXX) $(CXXFLAGS) -xc++-system-header --precompile $* -o $@

%.pcm: %.cppm
	$(CXX) $(CXXFLAGS) --precompile $< -o $@ $(ALL_MOD_FLAGS)

%.o: %.pcm
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(ALL_MOD_FLAGS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(ALL_MOD_FLAGS)

# dependencies
DEPS_MOTH   := vector.pch string.pch cctype.pch ostream.pch
DEPS_TEXT   := moth.pcm vector.pch string.pch memory.pch iostream.pch
DEPS_COLONY := text.pcm moth.pcm unordered_map.pch string.pch
DEPS_PARSER := colony.pcm text.pcm moth.pcm regex.pch istream.pch string.pch stdexcept.pch
DEPS_MAIN   := parser.pcm colony.pcm text.pcm moth.pcm iostream.pch

moth.pcm:   $(DEPS_MOTH)
text.pcm:   $(DEPS_TEXT)
colony.pcm: $(DEPS_COLONY)
parser.pcm: $(DEPS_PARSER)

moth.o:   $(DEPS_MOTH)
text.o:   $(DEPS_TEXT)
colony.o: $(DEPS_COLONY)
parser.o: $(DEPS_PARSER)
main.o:   $(DEPS_MAIN)

clean: 
	rm -f *.o *.pcm *.pch $(TARGET)
