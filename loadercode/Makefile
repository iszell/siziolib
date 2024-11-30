include ../defaults.mk

all: c16stm.prg parallel.prg serial_1bit.prg tcbm_standard.prg tcbm.prg

clean:
	$(CLEANCMD)

%.prg: %.asm core.inc
	$(KICKASS) -showmem -symbolfile -libdir $(INCDIR) -bytedumpfile $(basename $@).lst -o $@ $<
