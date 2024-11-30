	.namespace iolib {
	.namespace sd2iecgiejoe {
	*= $0500
{

.label	blk4job	= $04
.label	blk4trk	= $0e
.label	blk4sec	= $0f
.label	drv0id1	= $12
.label	drv0id2	= $13
.label	bitbuff	= $14
.label	hdrid1	= $16
.label	hdrid2	= $17
.label	retrycnt	= $8b

// serial bus bits are inverted!
// default value %00000000
//Bit  7   ATN IN
//Bits 6-5 Device address preset switches:
//           00 = #8, 01 = #9, 10 = #10, 11 = #11
//Bit  4   ATN acknowledge OUT
//Bit  3   CLOCK OUT
//Bit  2   CLOCK IN
//Bit  1   DATA OUT
//Bit  0   DATA IN
.label	serialdata = $1800

.label	ledport	= $1c00
.const ledvalue	= %00001000

.label	blk4buf	= $0700

	jsr init
mainloop:
	jsr readfnam
	lda blk4trk
	sta temp1
	lda blk4sec
	sta temp2
	ldy #1
dirloop:
	ldx #18
	stx blk4trk
	sty blk4sec
	jsr readblock
	ldy #2
nxtdirent:
	lda blk4buf,y
	and #$83		// type = PRG?
	cmp #$82
	bne nomatch
	lda blk4buf+3,y	// filename first char match?
	cmp temp1
	bne nomatch
	lda blk4buf+4,y	// filename second char match?
	cmp temp2
	bne nomatch
	jmp doload
nomatch:
	tya
	clc
	adc #32
	tay
	bcc nxtdirent
	ldy blk4buf+1
	bpl dirloop
senderror:
	lda #%00000000
	sta serialdata
	ldx #$fe
	jsr writebyte
	ldx #$fe
	jsr writebyte
	ldx #$ac
	jsr writebyte
	ldx #$f7
	jsr writebyte
	jmp mainloop
doload:	lda blk4buf+1,y
	sta blk4trk
	lda blk4buf+2,y
	sta blk4sec
doload1:
	jsr readblock
	ldy #0
	lda blk4buf
	sta blk4trk
	bne !+
	ldy blk4buf + 1
	iny
!:	sty temp1
	lda blk4buf + 1
	sta blk4sec
	ldy #2
	lda #%00000000
	sta serialdata
doload2:
	ldx blk4buf,y
	cpx #$ac
	bne !+
	jsr writebyte
	ldx #$ac
!:	jsr writebyte
	iny
	cpy temp1
	bne doload2
	lda blk4buf
	beq eof
	ldx #$ac
	jsr writebyte
	ldx #$c3
	jsr writebyte
	lda #%00001000
	sta serialdata
	jmp doload1
eof:	ldx #$ac
	jsr writebyte
	ldx #$ff
	jsr writebyte
	jmp mainloop
readfnam:
	lda #%00001000
	sta serialdata
	lda ledport	// turn led off
	and #255-ledvalue
	sta ledport
	cli
	lda #%00000001
!:	bit serialdata
	beq !-
	sei
	lda #%00000000
	sta serialdata
	jsr readbyte
	pha
	jsr readbyte
	sta blk4trk
	jsr readbyte
	sta blk4sec
	lda #%00001000
	sta serialdata
	lda ledport	// turn led on
	ora #ledvalue
	sta ledport
	pla
	rts
readblock:
	ldy #5
	sty retrycnt
retry:	cli
	lda #$80		// command: read sector
	sta blk4job
!:	lda blk4job
	bmi !-		// wait for block to be read
	cmp #1		// success
	beq readok
	dec retrycnt
	ldy retrycnt
	bmi readerror
	cpy #2		// header block not found
	bne !+
	lda #$c0		// bump head
	sta blk4job
!:	lda hdrid1
	sta drv0id1
	lda hdrid2
	sta drv0id2
!:	lda blk4job
	bmi !-
	bpl retry
readerror:
	pla
	pla
	jmp senderror
readok:	sei
	rts
writebyte:
	stx bitbuff
	lda #%00000100
	jsr write2bit
	jsr write2bit
	jsr write2bit
write2bit:
	lsr bitbuff
	ldx #%00000010
	bcc !+
	ldx #%00000000
!:	bit serialdata
	bne !-
	stx serialdata
	lsr bitbuff
	ldx #%00000010
	bcc !+
	ldx #%00000000
!:	bit serialdata
	beq !-
	stx serialdata
	rts
readbyte:
	ldy #4
	lda #%00000100
!:	bit serialdata
	beq !-
	lda serialdata
	lsr
	ror bitbuff
	lda #%00000100
!:	bit serialdata
	bne !-
	lda serialdata
	lsr
	ror bitbuff
	dey
	bne readbyte+2
	lda bitbuff
	rts
init:	sei
	cld
	ldy #8
initloop:
	lda #%00010000
	sta serialdata
!:	dex
	bne !-
	lda #%00000000
	sta serialdata
!:	dex
	bne !-
	dey
	bne initloop
!:	lda serialdata
	and #%00000101
	bne !-
	lda serialdata
	and #%00000101
	bne !-
	rts
.label	temp1	= *
.label	temp2	= temp1 + 1
}
}
}