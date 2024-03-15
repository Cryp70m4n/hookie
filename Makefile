CC=gcc
CFLAGS=-Wall -Wextra -g
TARGETS=sodump scinject
SRCDIR_SODUMP=src/sodump
SRCDIR_SCINJECT=src/scinject
BINDIR=/usr/bin

SRCS_SODUMP=$(wildcard $(SRCDIR_SODUMP)/*.c)
SRCS_SCINJECT=$(wildcard $(SRCDIR_SCINJECT)/*.c)
OBJS_SODUMP=$(SRCS_SODUMP:.c=.o)
OBJS_SCINJECT=$(SRCS_SCINJECT:.c=.o)

.PHONY: all clean install

all: $(TARGETS)

sodump: $(OBJS_SODUMP)
	$(CC) $(CFLAGS) $^ -o $@

scinject: $(OBJS_SCINJECT)
	$(CC) $(CFLAGS) $^ -o $@

install: $(TARGETS)
	cp sodump $(BINDIR)
	cp scinject $(BINDIR)

clean:
	rm -f $(OBJS_SODUMP) $(OBJS_SCINJECT) $(TARGETS)
