CPP_FLAGS = -std=c++17 -Wall -Wextra -Wshadow
DEBUG_FLAGS = -O0 -g -fsanitize=address,undefined
RELEASE_FLASGS = -O3 -flto

INCLUDE_FLAGS = -I./src
LD_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

SRCS = $(shell find src -name '*.cpp' -or -name '*.c')
DEBUG_OBJS = $(SRCS:%=build/debug/%.o)
RELEASE_OBJS = $(SRCS:%=build/release/%.o)

-include $(shell find build -name '*.d')

debug: $(DEBUG_OBJS)
	g++ $(DEBUG_OBJS) -o game $(CPP_FLAGS) $(DEBUG_FLAGS) $(INCLUDE_FLAGS) $(LD_FLAGS)

release: $(RELEASE_OBJS)
	g++ $(RELEASE_OBJS) -o game $(CPP_FLAGS) $(RELEASE_FLAGS) $(INCLUDE_FLAGS) $(LD_FLAGS)

build/debug/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	g++ -c $< -o $@ -MMD $(CPP_FLAGS) $(DEBUG_FLAGS) $(INCLUDE_FLAGS) $(LD_FLAGS)

build/release/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	g++ -c $< -o $@ -MMD $(CPP_FLAGS) $(RELEASE_FLAGS) $(INCLUDE_FLAGS) $(LD_FLAGS)

clean:
	rm -rf build
