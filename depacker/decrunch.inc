	.namespace iolib {
	.namespace iolibdecrunch {
	#import "ted.inc"
	#import "iolib_def.inc"
	.pseudopc decrunch
{
{
	stx xtemp
	sty ytemp
	jsr load
	bcs !+
	jsr dodecrunch
	clc
!:	ldx xtemp: #0
	ldy ytemp: #0
	rts
}

getcmem: {
	php
	stx xtemp
	sty ytemp
	ldy #0
	lda (io_loadptr),y
	inc io_loadptr
	bne !+
	inc io_loadptr+1
!:	ldx xtemp: #0
	ldy ytemp: #0
	plp
	rts
}
	#import	"exodecrunch.inc"
dodecrunch:
	exodecrunch(getcmem)
}
}
}
