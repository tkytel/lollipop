TARGET=	lollipop

PREFIX=	/usr/local
SPOOL=	/var/spool

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	$(RM) $(TARGET)

.PHONY: install
install: $(TARGET)
	install $(TARGET) $(PREFIX)/sbin/.
