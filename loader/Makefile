include ../defaults.mk

all: init.prg initquiet.prg stripped.prg strippedtest.prg

clean:
	$(CLEANCMD)

%.prg: %.asm
	$(KICKASS) -showmem -symbolfile -libdir $(INCDIR) -bytedumpfile $(basename $@).lst -o $(basename $@).tmp $<
	$(EXOMIZER) $(EXOMIZERSFXOPTS) -o $@ $(basename $@).tmp

init.prg: init.asm
	$(KICKASS) $(KICKASSOPTS) -define io_prtstatus -define io_detect_video -define io_detect_memory_size -define io_detect_sound -define io_detect_emulator -o init.tmp init.asm
	$(EXOMIZER) $(EXOMIZERSFXOPTS) -o $@ init.tmp

initquiet.prg: init.asm
	$(KICKASS) $(KICKASSOPTS) -o $(basename $@).tmp init.asm
	$(EXOMIZER) $(EXOMIZERSFXOPTS) -o $@ $(basename $@).tmp

hwdetect64.prg: hwdetect64.asm
	$(KICKASS) $(KICKASSOPTS) -define prtstatus -o hwdetect64.tmp hwdetect64.asm
	$(EXOMIZER) sfx basic -t 64 -n -o $@ hwdetect64.tmp

stripped.prg: stripped.asm
	$(KICKASS) -showmem -symbolfile -libdir $(INCDIR) -bytedumpfile $(basename $@).lst -o $@ $<

strippedtest.prg: strippedtest.asm stripped.asm
	$(KICKASS) -showmem -symbolfile -libdir $(INCDIR) -bytedumpfile $(basename $@).lst -o $@ $<
