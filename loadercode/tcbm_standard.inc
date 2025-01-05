	.print	". plus/4 TCBM standard protocol loader"
	.namespace iolib {
	.namespace plus4tcbm_standard {
	#import "ted.inc"

	.pseudopc io_base
{

.label	tia	= tcbm_9
.label	padta	= tia
.label	pbdta	= tia+1
.label	pcdta	= tia+2
.label	padir	= tia+3
.label	pbdir	= tia+4
.label	pcdir	= tia+5

.label	ptr	= iolib.io_loadptr

	jmp	startload
	rts

startload:
	stx filename
	sty filename+1
	
	ldx io_tcbmoffs

	jsr open
	lda pbdta,x	// STATUS
	and #3
	beq !+
	sec
	bcs err
!:	jsr talk
	lda #$60			// SECOND 0
	jsr secondary

	jsr getbyte
#if need_loadflag
	bit iolib.io_loadflag
	bpl !+
	lda ptr
!:
#endif
	sta ptr
	pha
	jsr getbyte
#if need_loadflag
	bit iolib.io_loadflag
	bpl !+
	lda ptr+1
!:
#endif
	sta ptr+1
	pha
	ldy #0
loadloop:
	jsr getbyte
	sta (ptr),Y
	lda eofmarker
	and #3
	bne eof
	iny
	bne loadloop
	inc ptr+1
	bne loadloop
eof:
	jsr untalk
	jsr listen
	lda #$E0			// CLOSE 0
	jsr secondary
	jsr unlisten
	clc
loaderror:
	pla
	sta ptr+1
	pla
	sta ptr
err:
	ldx filename
	ldy filename+1
	rts

open:
	lda #$F0					// OPEN
	pha
	jsr	listen
	pla
	jsr	secondary
	lda	filename
	jsr	sendbyte
	lda	filename+1
	jsr	sendbyte
	lda	#'*'
	jsr	sendbyte
	// fall through to unlisten
unlisten:
	lda #$3F					// UNLISTEN
	.byte BIT_ABS
untalk:
	lda	#$5F					// UNTALK
	.byte BIT_ABS
listen:
	lda	#$20					// LISTEN
	.byte BIT_ABS
talk:
	lda #$40					// TALK
	pha
	lda #$81    				// this is a command byte
	bne send
secondary:						// send secondary addr ($60 == SECOND after LISTEN and after TALK)
	pha
	lda #$82    				// this is a second byte
	bne send
sendbyte:
	pha
	lda #$83					// this is a data byte
send:							// send using standard Kernal protocol
	sta padta,x
!:	lda pcdta,x
	bmi !-
	pla
	pha
	jmp common

getbyte:
	lda #$84
	sta padta,x
!:	lda pcdta,x
	bmi !-
	lda #0
	sta padir,x		// input
	sta pcdta,x		// clear ACK
!:	lda pcdta,x
	bpl !-
	lda pbdta,x		// STATUS
	sta eofmarker
	lda padta,x
	pha
	lsr pcdta,x		// set ACK
!:	lda pcdta,x
	bmi !-
	lda #$ff
	sta padir,x		// output
	lda #0
common:
	sta padta,x
	lsr pcdta,x
!:	lda pcdta,x
	bpl !-
	lda #0			// needed by TCBM_Send
	sta padta,x
	lsr pcdta,x
	pla
	rts

filename:
	.byte 0, 0
eofmarker:
	.byte 0

.if(*>io_base + $100) {
	.error "Build too long: $" + toHexString(*-io_base-$100, 4)
} else {
	.print ". . space remaining: $" + toHexString($100-(* - io_base), 4)
}

}
}
}
