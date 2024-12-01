include ../defaults.mk

all: serial_1bit_1541.prg serial_1bit_1581.prg serial_1bit_gijoe.prg parallel_1541.prg tcbm_1551.prg

clean:
	$(CLEANCMD)

%.prg: %.asm core.inc serial_1bit_core.inc
	$(KICKASS) -showmem -symbolfile -libdir $(INCDIR) -bytedumpfile $(basename $@).lst -o $@ $<
