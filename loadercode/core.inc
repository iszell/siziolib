	#importonce
	#import	"plus4_io_map.inc"
	#import	"iolib_def.inc"

// WARNING! C16STM does not use this!

.macro	plus4load(readbyte, writebyte, sync, hardsync) {
.label	ptr	= iolib.io_loadptr
	stx filename
	sty filename+1
	jsr hardsync
	lda #2
	jsr writebyte
	lda filename
	jsr writebyte
	lda filename+1
	jsr writebyte
	jsr sync

	jsr readbyte
#if need_loadflag
	bit iolib.io_loadflag
	bmi !+
#endif
	sta ptr
!:
	jsr readbyte
#if need_loadflag
	bit iolib.io_loadflag
	bmi !+
#endif
	sta ptr+1
!:
	cmp #$fe
	bne !+
	jsr readbyte
	jsr readbyte
	jmp loaderror
!:	ldy #0
loadloop:
	jsr readbyte
	cmp #$ac
	bne storebyte
	jsr readbyte
	cmp #$ac
	beq storebyte
	cmp #$ff
	beq eof
	cmp #$f7
	beq loaderror
	jsr sync
	jmp loadloop
storebyte:
	sta (ptr),Y
	iny
	bne loadloop
	inc ptr+1
	bne loadloop
eof:
	clc
loaderror:
	ldx filename
	ldy filename+1
	rts

filename:
	.byte 0, 0
}
