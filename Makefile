LOLLIPOPD=	lollipopd
LOLLIPOP=	lollipop

.PHONY: all $(LOLLIPOPD) $(LOLLIPOP)
all: $(LOLLIPOPD) $(LOLLIPOP)

$(LOLLIPOPD) $(LOLLIPOP):
	$(MAKE) -C $@

.PHONY: clean
clean:
	$(MAKE) -C $(LOLLIPOPD) $@
	$(MAKE) -C $(LOLLIPOP) $@
