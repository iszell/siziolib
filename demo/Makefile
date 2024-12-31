include ../defaults.mk

all: demo.prg iolibv3test.prg iolibv3exotest.prg bitmapexodata.prg exotestdata.prg testdata.prg

clean:
	$(CLEANCMD)

%.prg: %.asm
	$(KICKASS) -showmem -symbolfile -libdir $(INCDIR) -bytedumpfile $(basename $@).lst -o $(basename $@).tmp $<
	$(EXOMIZER) $(EXOMIZERSFXOPTS) -o $@ $(basename $@).tmp

demo.prg: demo.asm
	$(KICKASS) -showmem -symbolfile -libdir $(INCDIR) -bytedumpfile $(basename $@).lst -o $(basename $@).tmp $<
	$(EXOMIZER) sfx basic -t 16 -n -s "lda 174 pha" -f "pla sta 174" -o $@ $(basename $@).tmp

bitmapexodata.prg: bitmap1.bin
	$(EXOMIZER) $(EXOMIZERMEMOPTS) -o $@ $<

exotestdata.prg: dotctitle.p4i
	$(EXOMIZER) $(EXOMIZERMEMOPTS) -o $@ $<

testdata.prg: testdata.asm
	$(KICKASS) -showmem -symbolfile -libdir $(INCDIR) -bytedumpfile $(basename $@).lst -o $@ $<
