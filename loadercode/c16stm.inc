	.print	". plus/4 C16STM loader"
	.namespace iolib {
	.namespace plus4c16stm {
	
	#import "iolib_def.inc"
	#import "ted.inc"
	.pseudopc io_base
{

#if TEST_C16STM
.print "!!! TEST BUILD !!!"
#endif

	jmp	startload
hardsync:
sync:
	rts

startload:
	stx	filename
	sty	filename+1

	lda	#$52		// fopen
	jsr	sendrecievebyte
	ldx #0
!:	lda filename,x	// send filename
	jsr sendrecievebyte
	inx
	cpx #7
	bne !-

	jsr sendrecievebyte
	cmp #$7f
	bne !+
// signal load error
loaderror:
	sec
	rts
!:
	sta channel		// logical file channel
	
	jsr sendrecievebyte
	sta length		// file size
	jsr sendrecievebyte
	sta length+1
	
	jsr sendandreceive

	jsr sendrecievebyte
	cmp #$80
	bne loaderror
!:

	lda #$63		// read word from file: load address
	jsr sendandreceive
#if need_loadflag
	bit iolib.io_loadflag
	bmi !+
#endif
	sta iolib.io_loadptr
#if	need_exodecrunch
	pha
#endif
!:
	jsr sendrecievebyte
#if need_loadflag
	bit iolib.io_loadflag
	bmi !+
#endif
	sta iolib.io_loadptr+1
#if	need_exodecrunch
	pha
#endif
!:
	jsr declength
	jsr declength

	lda #$65		// read block
	jsr sendrecievebyte
	lda length
	jsr sendrecievebyte
	lda length+1
	jsr sendrecievebyte

	ldy #0
!:	jsr sendrecievebyte
	sta (iolib.io_loadptr),y
	iny
	bne !+
	inc io_loadptr+1
!:	jsr declength
	bne !--

	jsr sendrecievebyte

	lda #$54		// fclose
	jsr sendrecievebyte
.label channel=*+1
	lda #0
	jsr sendandreceive
	
#if	need_exodecrunch
	pla
	sta iolib.io_loadptr+1
	pla
	sta iolib.io_loadptr
#endif

	ldx filename
	ldy filename+1
	clc
	rts

sendandreceive:
	jsr sendrecievebyte
sendrecievebyte:
#if TEST_C16STM
	lda #$80
	rts
#else
	sta c16stm.data
!:
	lda c16stm.status
	bmi !-
	lda c16stm.data
	rts
#endif

declength:
	lda length+1
	bne !+
	dec length+1
!:	dec length
	rts

length:
	.word	0
filename:
	.text	"00.prg"
filenameptr:
	.byte	0

.if(*>io_base + $100) {
	.error "Build too long: $" + toHexString(*-io_base-$100, 4)
} else {
	.print ". . space remaining: $" + toHexString($100-(* - io_base), 4)
}
}
}
}

/*
Transfer protocol:
- write byte to send/trigger recieve to data register
- wait for status register bit 7 to became 0
- read response byte from data register
Load protocol (ROM):
- fopen
- - $52
- - filename
- - $00
- read response: $7f: file open error, otherwise channel(?)
- read file length low
- read file length high
- read 2 dummy bytes
- read fopen status: $80 - OK
- read file with one of these:
- - $63: read word
- - $64: read byte
- - $65 <length-low> <length high>: read block with length then dummy read
- fclose:
- - $54
- - channel#
*/
