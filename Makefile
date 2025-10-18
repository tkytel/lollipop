all: lollipop

lollipop: main.c
	cc -o lollipop main.c

.PHONY: clean
clean:
	rm -f lollipop
