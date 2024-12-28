include ../defaults.mk

all: decrunch.prg 
# decrunch_c16stm.prg

clean:
	$(CLEANCMD)

%.prg: %.asm exodecrunch.inc
	$(KICKASS) -showmem -symbolfile -libdir $(INCDIR) -bytedumpfile $(basename $@).lst -o $@ $<
