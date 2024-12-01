SUBDIRS := 	drivecode \
			loadercode \
			depacker \
			loader \
			demo \
			hwdetect

all: $(SUBDIRS) testdisk.d64 testdisk.d71 testdisk.d81 extract

include defaults.mk

.PHONY: $(SUBDIRS) all

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

clean: $(SUBDIRS)
	$(CLEANCMD)

%.prg: %.asm
	$(KICKASS) $(KICKASSOPTS) -o $(basename $@).tmp $<
	$(EXOMIZER) $(EXOMIZERSFXOPTS) -o $@ $(basename $@).tmp

testdisk.d64 testdisk.d71 testdisk.d81: $(SUBDIRS)
	$(RM) $@
	$(CC1541) \
		-n "iolibv3test" \
		-f demo -w demo/demo.prg \
		-f iolibv3test -w demo/iolibv3test.prg \
		-f iolibv3exotest -w demo/iolibv3exotest.prg \
		-f b1 -w demo/bitmap1.bin \
		-f e1 -w demo/bitmapexodata.prg \
		-f testdata -w demo/testdata.prg \
		-f exotestdata -w demo/exotestdata.prg \
		-f "hwdetect plus/4" -w hwdetect/hwdetectplus4.prg \
		-f "hwdetect 64" -w hwdetect/hwdetect64.prg \
		-f "stripped loader" -w loader/strippedtest.prg \
		$@

extract: testdisk.d64
	$(RM) -r sd2iec
	mkdir sd2iec
	$(C1541) -attach testdisk.d64 -cd sd2iec -extract 8 -cd ..
	for i in sd2iec/* ; do mv "$$i" "$$i.prg" ; done
