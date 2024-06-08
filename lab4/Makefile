CC = g++
CFLAGS = -g -Wall -std=c++11
LDFLAGS =

# Source files
LAB4_SRCS = libTinyFS.cpp libDisk.cpp
# Header files
LAB4_HDRS = libDisk.h libTinyFS.h errtfs.h macrostfs.h

# Object files
LAB4_OBJS = $(LAB4_SRCS:.cpp=.o)

all: test

tinyFS: $(LAB4_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(LAB4_OBJS)

test: $(LAB4_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o tinyFsDemo $(LAB4_OBJS) tinyFsDemo.cpp

# Compilation rule for cpp files
%.o: %.cpp $(LAB4_HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# clean:
# 	rm -f tinyFS $(LAB4_OBJS)

clean:
	rm -f tinyFsDemo tinyFS $(LAB4_OBJS)
