TARGET=	lollipopd
OBJS=	./main.o ../common/ulid.o

CC=	gcc
CFLAGS=	-I ../include -O2

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)
