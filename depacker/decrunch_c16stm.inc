	.print	". exomizer decruncher for C16STM"
	.namespace iolib {
	.namespace iolibdecrunch {
	.pseudopc decrunch {

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
dodecrunch2:
	exodecrunch(getcmem2)
}
}
}
