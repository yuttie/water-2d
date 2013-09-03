.SUFFIXES:
.SUFFIXES: .c .cpp .o
.PHONY: all clean profile run rebuild

# Macro Definition
DEBUG    = -g -pg
CC       = gcc
CFLAGS   = -Wall -std=c99 `pkg-config --cflags sdl SDL_gfx` $(DEBUG)
CXX      = g++
CXXFLAGS = -Wall `pkg-config --cflags sdl SDL_gfx` $(DEBUG)
LD       = gcc
LDFLAGS  = $(DEBUG)
INCLUDES = -I.
LIBS     = `pkg-config --libs sdl SDL_gfx`
RM       = rm -f
PROF     = gprof
TARGET   = water2d
SRCS     = main.c imgscale.c cpuidutil.c step_fpu.c step_sse.c option_parser.c
OBJS     = $(SRCS:%.c=%.o)

# Rules
all: $(TARGET)
clean:
	$(RM) $(TARGET) $(OBJS) gmon.out tags *~
rebuild: clean $(TARGET)
profile:
	$(PROF) $(TARGET) gmon.out | less
run: $(TARGET)
	./$(TARGET)
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
step_sse.o: step_sse.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $(OPT) -c $<
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPT) -c $<

# Dependency(Generated by 'gcc -MM *.c')
calcfunc.o: calcfunc.c main.h calcfunc.h
cpuidutil.o: cpuidutil.c cpuidutil.h
imgscale.o: imgscale.c imgscale.h
main.o: main.c imgscale.h cpuidutil.h step_fpu.h step_sse.h \
  option_parser.h main.h
option_parser.o: option_parser.c main.h option_parser.h
step_fpu.o: step_fpu.c main.h step_fpu.h
step_sse.o: step_sse.c main.h step_sse.h


