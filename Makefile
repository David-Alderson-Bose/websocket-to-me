# Resulting executable
EXE := build/websockeye_salmon

# Make it easy to scoop up new source files
OBJS = $(patsubst ./src/%.c,./build/%.o,$(wildcard ./src/*.c))
OBJS += $(patsubst ./src/%.cpp,./build/%.opp,$(wildcard ./src/*.cpp))

# Header locations
INCLUDE = -I./include


COMMON_FLAGS += -Wreturn-type #-Wall -Werror
CXX_FLAGS += -std=gnu++11

# Libraries to link against the normal way
OTHER_LIBS := \
	-lwebsockets 

.PHONY: all clean echo
all: $(EXE)

# Link executable
$(EXE): $(OBJS)
	@echo 'Linking source file(s) $(OBJS) together into $@...'
	@$(CXX) $(OBJS) $(COMMON_FLAGS) ${OTHER_LIBS} -o "$@" 
	@echo "Built $@"

# Build cpp
build/%.opp: src/%.cpp 
	@echo 'Building source file $<...'
	@$(CXX) $(COMMON_FLAGS) $(CXX_FLAGS) $(INCLUDE) -c -o "$@" "$<" 
	@echo 'Built $@'
	@echo

# Build c
%.o: %.c 
	@echo 'Building source file $<...'
	@$(CC) $(COMMON_FLAGS) ($CFLAGS) $(INCLUDE) -c -o "$@" "$<"
	@echo 'Built $@'
	@echo

clean:
	@rm -rf build/*
	@echo "Squeeky clean!"

