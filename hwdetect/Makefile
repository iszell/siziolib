include ../defaults.mk

all: hwdetectplus4.prg hwdetect64.prg

clean:
	$(CLEANCMD)

%.prg: %.asm
	$(KICKASS) -showmem -symbolfile -libdir $(INCDIR) -bytedumpfile $(basename $@).lst -o $(basename $@).tmp $<
	$(EXOMIZER) $(EXOMIZERSFXOPTS) -o $@ $(basename $@).tmp

hwdetect64.prg: hwdetect64.asm
	$(KICKASS) $(KICKASSOPTS) -define prtstatus -o hwdetect64.tmp hwdetect64.asm
	$(EXOMIZER) sfx basic -t 64 -n -o $@ hwdetect64.tmp
