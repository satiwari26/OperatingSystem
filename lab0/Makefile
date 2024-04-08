CC = gcc
CFLAGS = -Wall 

all: fork_loop who_runs_first

fork_loop: fork_loop.c
	$(CC) $(CFLAGS) -o $@ $^

who_runs_first: who_runs_first.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f fork_loop who_runs_first
