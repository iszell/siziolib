SUBDIRS := 	drivecode \
			loadercode

all: $(SUBDIRS) testdisk.d64 testdisk.d71 testdisk.d81 init.prg initquiet.prg stripped.prg extract

include defaults.mk

.PHONY: $(SUBDIRS) all

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

clean: $(SUBDIRS)
	$(CLEANCMD)

%.prg: %.asm *.inc
	$(KICKASS) $(KICKASSOPTS) -o $(basename $@).tmp $<
	$(EXOMIZER) $(EXOMIZERSFXOPTS) -o $@ $(basename $@).tmp

bitmapexodata.prg: bitmap1.bin
	$(EXOMIZER) $(EXOMIZERMEMOPTS) -o $@ $<

demo.prg: demo.asm *.inc
	$(KICKASS) $(KICKASSOPTS) -o $@ demo.asm

hwdetect64.prg: hwdetect64.asm *.inc
	$(KICKASS) $(KICKASSOPTS) -define prtstatus -o hwdetect64.tmp hwdetect64.asm
	$(EXOMIZER) sfx basic -t 64 -n -o $@ hwdetect64.tmp

init.prg: init.asm *.inc
	$(KICKASS) $(KICKASSOPTS) -define prtstatus -o init.tmp init.asm
	$(EXOMIZER) $(EXOMIZERSFXOPTS) -o $@ init.tmp

initquiet.prg: init.asm *.inc
	$(KICKASS) $(KICKASSOPTS) -o initquiet.tmp init.asm
	$(EXOMIZER) $(EXOMIZERSFXOPTS) -o $@ initquiet.tmp

exotestdata.prg: dotctitle.bin
	$(EXOMIZER) $(EXOMIZERMEMOPTS) -o $@ $<

stripped.prg: stripped.asm *.inc
	$(KICKASS) $(KICKASSOPTS) -o $@ stripped.asm

strippedtest.prg: strippedtest.asm stripped.asm *.inc
	$(KICKASS) $(KICKASSOPTS) -o $@ strippedtest.asm

testfile.prg: testfile.asm
	$(KICKASS) $(KICKASSOPTS) -o $@ testfile.asm

testdata.prg: testdata.asm
	$(KICKASS) $(KICKASSOPTS) -o $@ testdata.asm

testdisk.d64 testdisk.d71 testdisk.d81: $(PRGS) exotestdata.prg bitmapexodata.prg
	$(RM) $@
	$(CC1541) \
		-n "iolibv3test" \
		-f demo -w demo.prg \
		-f iolibv3test -w iolibv3test.prg \
		-f iolibv3exotest -w iolibv3exotest.prg \
		-f b1 -w bitmap1.bin \
		-f e1 -w bitmapexodata.prg \
		-f testdata -w testdata.prg \
		-f exotestdata -w exotestdata.prg \
		-f "hwdetect plus/4" -w hwdetectplus4.prg \
		-f "hwdetect 64" -w hwdetect64.prg \
		-f "stripped loader" -w strippedtest.prg \
		$@

extract: testdisk.d64
	$(RM) -r sd2iec
	mkdir sd2iec
	$(C1541) -attach testdisk.d64 -cd sd2iec -extract 8 -cd ..
	for i in sd2iec/* ; do mv "$$i" "$$i.prg" ; done
