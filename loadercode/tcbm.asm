	.print	". plus/4 TCBM loader"
	.namespace iolib {
	.namespace plus4tcbm {
	#import "plus4_io_map.inc"

	*= io_base
{

.label	tia	= tcbm_9
.label	padta	= tia
.label	pbdta	= tia+1
.label	pcdta	= tia+2
.label	padir	= tia+3
.label	pbdir	= tia+4
.label	pcdir	= tia+5

	jmp	startload
	jmp	readbyte
	jmp	writebyte
	jmp	hardsync
	jmp	sync
	ldx	io_tcbmoffs
	lda pcdta,x
	and #%10111111
	sta pcdta,x
!:	lda pcdta,x
	bmi !-
hardsync:
sync:	rts

readbyte:
	ldx	io_tcbmoffs
!:	lda pcdta,x	//ACK=1?
	bmi !-		//yes
	lda pcdta,x
	ora #%01000000
	sta pcdta,x	//DAV=1
!:	lda pcdta,x	//ACK=1?
	bpl !-		//no
	lda padta,x	//get byte
	pha
	lda pcdta,x
	and #%10111111
	sta pcdta,x	//DAV=0
	pla
	rts

writebyte:
	ldx	io_tcbmoffs
	pha
	lda #$ff
	sta padir,x	//dir=out
!:	lda pcdta,x	//ACK=1?
	bmi !-		//yes
	pla
	sta padta,x	//put byte
	lda pcdta,x
	ora #%01000000
	sta pcdta,x	//DAV=1
!:	lda pcdta,x	//ACK=1?
	bpl !-		//no
	lda pcdta,x
	and #%10111111
	sta pcdta,x	//DAV=0
	inc padir,x	//dir=in
	rts
	#import	"core.inc"

startload:
	plus4load(readbyte, writebyte, sync, hardsync)
}
}
}
