	#import "stripped.asm"
	
	* = $5555
	
	dec $ff19
	lda #0
	sta iolib.io_loadflag
	ldx #'b'
	ldy #'1'
	jsr iolib.load
	lda #$65
	bcc !+
	lda #$32
!:	sta $ff19
	jmp *
