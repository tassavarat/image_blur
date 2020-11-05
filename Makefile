CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -pthread

SRC = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRC))
BLUR = $(patsubst %.c, %.o, $(wildcard *.c))

.PHONY: clean

all: $(BLUR)
	$(CC) $(CFLAGS) $(BLUR) -o img_blur

clean:
	rm $(OBJS)
