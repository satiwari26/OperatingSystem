CC = g++
CFLAGS = -g -Wall -std=c++11
LDFLAGS =

# Source files
LAB4_SRCS = tinyFS.cpp libDisk.cpp
# Header files
LAB4_HDRS = libDisk.h tinyFS.h

# Object files
LAB4_OBJS = $(LAB4_SRCS:.cpp=.o)

all: tinyFS

tinyFS: $(LAB4_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(LAB4_OBJS)

test: $(LAB4_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(LAB4_OBJS) test.cpp

# Compilation rule for cpp files
%.o: %.cpp $(LAB4_HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f tinyFS $(LAB4_OBJS)

clean-test:
	rm -f test tinyFS $(LAB4_OBJS)
