	#importonce
.macro	driveload(init, dirtrack, dirsect, blk4trk, blk4sec, blk4job, blk4buf, retrycnt, hdrid1, hdrid2, drv0id1, drv0id2, readbyte, writebyte, ledport, ledvalue, serialdata, drive_serial, ledinverted) {
	jsr init
mainloop:
	jsr readfnam
	lda blk4trk
	sta temp1
	ora blk4sec
	bne !+
	lda #RTS
	sta senderrorend
	jsr senderror
	jmp ($fffc)
!:	lda blk4sec
	sta temp2
	ldy #dirsect
dirloop:
	ldx dirtrack
	stx blk4trk
	sty blk4sec
	jsr readblock
	ldy #2
nxtdirent:
	lda blk4buf,y
	and #$83		// type = PRG?
	cmp #$82
	bne nomatch
	lda blk4buf+3,y		// filename first char match?
	cmp temp1
	bne nomatch
	lda blk4buf+4,y		// filename second char match?
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
	.if (drive_serial) {
	lda #%00000000
	sta serialdata
	}
	ldx #$fe
	jsr writebyte
	ldx #$fe
	jsr writebyte
	ldx #$ac
	jsr writebyte
	ldx #$f7
	jsr writebyte
senderrorend:
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
	.if (drive_serial) {
	lda #%00000000
	sta serialdata
	}
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
	.if (drive_serial) {
	lda #%00001000
	sta serialdata
	}
	jmp doload1
eof:
	ldx #$ac
	jsr writebyte
	ldx #$ff
	jsr writebyte
	jmp mainloop
readfnam:
	.if (drive_serial) {
	lda #%00001000
	sta serialdata
	}
	lda ledport		// turn led off
	.if (!ledinverted) {
	and #~ledvalue
	} else {
	ora #ledvalue
	}
	sta ledport
	cli
	.if (drive_serial) {
	lda #%00000001
!:	bit serialdata
	beq !-
	}
	sei
	.if (drive_serial) {
	lda #%00000000
	sta serialdata
	}
	jsr readbyte
	pha
	jsr readbyte
	sta blk4trk
	jsr readbyte
	sta blk4sec
	.if (drive_serial) {
	lda #%00001000
	sta serialdata
	}
	lda ledport		// turn led on
	.if (!ledinverted) {
	ora #ledvalue
	} else {
	and #~ledvalue
	}
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
	bmi !-			// wait for block to be read
	beq readok
	cmp #1			// success
	beq readok
	dec retrycnt
	ldy retrycnt
	bmi readerror
	cpy #2			// header block not found
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
	sei
	jmp senderror
readok:	sei
	rts
temp1:	.byte	0
temp2:	.byte	0
}
