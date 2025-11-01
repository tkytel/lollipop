COMMANDS=	in ls out rm

.PHONY: all $(COMMANDS)
all: $(COMMANDS)

$(COMMANDS):
	$(MAKE) -C $@

.PHONY: clean
clean:
	for c in $(COMMANDS); do $(MAKE) -C $$c $@; done
