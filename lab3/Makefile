CC = g++
CFLAGS = -g -Wall
LDFLAGS =

# Source files
LAB3_SRCS = memSim.cpp

all: memSim

memSim: $(LAB3_SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(LAB3_SRCS)

clean:
	rm -f memSim
