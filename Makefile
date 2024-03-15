CC=gcc
CFLAGS=-Wall -Wextra -g
TARGET=sodump
SRCDIR=src/sodump
BINDIR=/usr/bin

SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(SRCS:.c=.o)

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

install: $(TARGET)
	cp $(TARGET) $(BINDIR)

clean:
	rm -f $(OBJS) $(TARGET)
