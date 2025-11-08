LOLLIPOPD=	lollipopd
LOLLIPOP=	lollipop

.PHONY: all $(LOLLIPOPD) $(LOLLIPOP)
all: $(LOLLIPOPD) $(LOLLIPOP)

$(LOLLIPOPD) $(LOLLIPOP):
	$(MAKE) -C $@

.PHONY: clean install
install clean:
	$(MAKE) -C $(LOLLIPOPD) $@
	$(MAKE) -C $(LOLLIPOP) $@
