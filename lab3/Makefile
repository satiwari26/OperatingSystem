CC = g++
CFLAGS = -g -Wall -std=c++11
LDFLAGS =

# Source files
LAB3_SRCS = memSim.cpp

all: memSim

memSim: $(LAB3_SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(LAB3_SRCS)

clean:
	rm -f memSim
