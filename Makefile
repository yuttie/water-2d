.SUFFIXES:
.SUFFIXES: .c .cpp .o

# Macro Definition
DEBUG    = -g -pg
CC       = gcc
CFLAGS   = -O2 -Wall -std=gnu99 `sdl-config --cflags` $(DEBUG)
CXX      = g++
CXXFLAGS = -O2 -Wall `sdl-config --cflags` $(DEBUG)
LD       = gcc
LDFLAGS  = $(DEBUG)
INCLUDES = -I.
LIBS     = `sdl-config --libs` -lSDL_gfx
RM       = rm -f
PROF     = gprof
TARGET   = water
OBJS     = main.o imgscale.o

# Rules
all: Makefile $(TARGET)
run: $(TARGET)
	./$(TARGET)
profile:
	$(PROF) $(TARGET) gmon.out | less
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
clean:
	$(RM) $(TARGET) $(OBJS) gmon.out *~
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<

imgscale.o: imgscale.c imgscale.h
main.o:     main.c imgscale.h

