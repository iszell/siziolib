	.print	". exomizer decruncher"
	.namespace iolib {
	.namespace iolibdecrunch {
	.pseudopc decrunch {

	tya
	pha
	txa
	pha
	jsr io_hardsync
	jsr io_writebyte
	pla
	jsr io_writebyte
	pla
	jsr io_writebyte
	jsr io_sync
	jsr getcmem	// Ignore first two bytes of file
	jsr getcmem
	jsr dodecrunch
	jsr getcmem	// read EOF marker
	clc
	rts

getcmem:
	php
	stx xtemp
	sty ytemp
!:	jsr io_readbyte
	cmp #$ac
	bne !+
	jsr io_readbyte
	cmp #$ac
	beq !+
	cmp #$ff
	beq eof
	cmp #$f7
	beq lderror
	jsr io_sync
	jmp !-
!:
.label	xtemp	= * + 1
	ldx #0
.label	ytemp	= * + 1
	ldy #0
	plp
	rts
lderror:
	pla
	pla
	pla
	pla
	pla
	sec
	.byte $24	// bit $xx
eof:	pla
	clc
	rts
	#import	"exodecrunch.inc"
dodecrunch:
	exodecrunch(getcmem)
}
}
}
