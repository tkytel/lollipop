all: lollipopd

lollipopd: main.c ulid.c ulid.h
	cc -o lollipopd main.c ulid.c

.PHONY: clean
clean:
	rm -f lollipopd
