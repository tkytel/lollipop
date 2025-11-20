TARGET=	lollipop-out
OBJS=	./main.o ../../common/match.o

CC=	gcc
CFLAGS=	-I../../include -O2

PREFIX=	/usr/local
SPOOL=	/var/spool

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)

.PHONY: install
install: $(TARGET)
	install $(TARGET) $(PREFIX)/sbin/.
	mkdir -p $(SPOOL)/lollipop/waiting
	mkdir -p $(SPOOL)/lollipop/sending
	mkdir -p $(SPOOL)/lollipop/sent
	mkdir -p $(SPOOL)/lollipop/removed
