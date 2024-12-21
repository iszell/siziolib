	#import	"kernal_table.inc"
	#import	"c64_c128_io_map.inc"
	#import	"fm.inc"

	.encoding	"petscii_mixed"

.label	ptr	= 4

	BasicUpstart2(start)
start:	jsr	primm
	.text	"Hardware detect using IOLib by Siz"
	.byte	14, 13, 0

	jsr	iolib.detect.detect
	rts

primm:	{
	pla
	sta	ptr
	pla
	sta	ptr+1
	ldy	#0
!:	inc	ptr
	bne	!+
	inc	ptr+1
!:	lda	(ptr),y
	beq	end
	jsr	chrout
	jmp	!--
end:	lda	ptr+1
	pha
	lda	ptr
	pha
	rts
}

hexbout: {
	pha
	lsr
	lsr
	lsr
	lsr
	jsr print
	pla
	and #$0f
print:
	cmp #10
	bcc !+
	adc #6
!:	adc #'0'
	jmp chrout
}

	#define io_prtstatus
	#define io_detect_video
	#define io_detect_sound
	#define io_detect_drive
	#define io_memory_size
	#define io_detect_emulator
//	#define io_halt_on_vice
	#define io_detect_cpu_port

	#import "../detect/detect.inc"
