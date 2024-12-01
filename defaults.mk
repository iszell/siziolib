BINDIR=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))bin
INCDIR=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))include

C1541			= c1541.bat
CC1541			= $(BINDIR)/cc1541
BITNAX			= $(BINDIR)/bitnax
BITNAXOPTS		= --bitfire --plus4
EXOMIZER		= $(BINDIR)/exomizer
EXOMIZERSFXOPTS	= sfx basic -t 4 -n -s "lda 174 pha" -f "pla sta 174"
EXOMIZERMEMOPTS	= mem -f -c
#KICKASS			= java -cp $(BINDIR)/KickAss.jar:$(BINDIR)/kickass-cruncher-plugins-2.0.jar kickass.KickAssembler
KICKASS			= java -jar $(BINDIR)/KickAss.jar
KICKASSOPTS		+= -showmem -symbolfile -libdir $(INCDIR) -bytedumpfile $(basename $@).lst $(KICKASSLINKING) $(KICKASSDEBUG)

CLEANCMD		= $(RM) *.d{0..99} *.lst *.lz *.prg *.sym *.tmp ByteDump.txt
