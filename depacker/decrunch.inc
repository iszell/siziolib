	.namespace iolib {
	.namespace iolibdecrunch {
	#import "ted.inc"
	#import "iolib_def.inc"
	.pseudopc decrunch
{
	jsr load
	bcs !+
	jsr dodecrunch
	clc
!:	rts

getcmem:
	php
	stx xtemp
	sty ytemp
	ldy #0
	lda (io_loadptr),y
	inc io_loadptr
	bne !+
	inc io_loadptr+1
!:
.label	xtemp	= * + 1
	ldx #0
.label	ytemp	= * + 1
	ldy #0
	plp
	rts
	#import	"exodecrunch.inc"
dodecrunch:
	exodecrunch(getcmem)
}
}
}
