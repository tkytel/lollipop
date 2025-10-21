LOLLIPOPD=	lollipopd

.PHONY: all $(LOLLIPOPD)
all: $(LOLLIPOPD)

$(LOLLIPOPD):
	$(MAKE) -C $@
