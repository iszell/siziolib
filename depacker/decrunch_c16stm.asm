	.namespace iolib {
	.namespace iolibdecrunch {
	#import "ted.inc"
	#import "iolib_def.inc"
	*= decrunch
{

// Specialized exomizer decruncher for loaders with no driveside code
	tya
	pha
	txa
	jsr io_writebyte
	pla
	jsr io_writebyte
	bcs !+
	jsr dodecrunch2
	clc
	.byte BIT_ZP
!:	sec
	rts

getcmem2: {
	php
	sty ytemp2
!:	jsr io_readbyte
.label	ytemp2	= * + 1
	ldy #0
	plp
	rts
}
	#import	"exodecrunch.inc"
dodecrunch2:
	exodecrunch(getcmem2)
}
}
}
