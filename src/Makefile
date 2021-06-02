CFLAGS=-Wall -Wextra -pedantic

all: slab

debug: CFLAGS += -DDEBUG -g
debug: slab

slab:
	gcc $(CFLAGS) -c slab.c hash.c

test: slab
test: CFLAGS += -DDEBUG -g
test:
	gcc $(CFLAGS) test.c -o slab_test slab.o hash.o
	./slab_test
