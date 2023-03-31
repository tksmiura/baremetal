

CC ?= gcc
CFLAGS ?= -Wall -O0 -g
LDFLAGS ?=

.PHONY: test test_report clean

.c.o:
	$(CC) $(CFLAGS) -c $<

test: runner target
	./test.sh

runner: runner.o
	$(CC) $(LDFLAGS) -o $@ $<

target: guest.o
	$(CC) $(LDFLAGS) -o $@ $<

clean:
	rm -f *.o runner target
