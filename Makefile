# Resulting executable
EXE := build/websockeye_salmon

# Make it easy to scoop up new source files
OBJS = $(patsubst ./src/%.c,./build/%.o,$(wildcard ./src/*.c))
OBJS += $(patsubst ./src/%.cpp,./build/%.opp,$(wildcard ./src/*.cpp))

# Header locations
INCLUDE = -I./include
ifdef CASTLE_INCLUDE
	INCLUDE += \
		-I/scratch/components-cache/Release/bose-stable/2.4.1-76+g96380a1/opensource-libwebsockets-qc8017_32/include
endif

# Extra libraries
OTHER_LIBS = \
	-lwebsockets
ifdef CASTLE_LIB
	OTHER_LIBS += \
		-L/scratch/components-cache/Release/bose-stable/2.4.1-76+g96380a1/opensource-libwebsockets-qc8017_32/lib
endif

# More compile flags
COMMON_FLAGS += -Wreturn-type #-Wall -Werror
CXX_FLAGS += -std=gnu++11

# YOU BIG PHONY
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

