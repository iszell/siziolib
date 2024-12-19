include ../defaults.mk

all: sid-detect2.prg

clean:
	$(CLEANCMD)

%.prg: %.asm *.inc
	$(KICKASS) -showmem -symbolfile -libdir $(INCDIR) -bytedumpfile $(basename $@).lst -o $@ $<
