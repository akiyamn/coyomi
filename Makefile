.POSIX:
CC := gcc
CFLAGS := -Wall -g
LDFLAGS := -lm -lncursesw
EXE := coyomi

.PHONY: all clean

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(EXE) $(OBJS) a.out