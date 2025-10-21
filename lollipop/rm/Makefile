TARGET=	lollipop-rm
OBJS=	./main.o

CC=	gcc
CFLAGS=	-O2

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)
