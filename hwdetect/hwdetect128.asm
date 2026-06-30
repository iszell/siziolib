	#import	"kernal_table.inc"
	#import	"c64_c128_io_map.inc"
	#import	"fm.inc"

	.encoding	"petscii_mixed"

.label	ptr	= 4

	.print "C128 BASIC header"
	* = $1C01 "BASIC header"

{
	.word basend
	.word 2026
	.byte $9e	// BASIC SYS token
	.text "7181"
	.byte 0
basend:	.word 0
}
	* = $1c0d "Code"

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

// Print IOLib status messages
	#define io_prtstatus
// Detect video standard. Not really useful except for printing status message
//	#define io_detect_video
// Detect memory size and expansion type
//	#define io_detect_memory_size
// Detect available sound expansions including SID type and address
	#define io_detect_sound
// Detect if ACIA is present
//	#define io_detect_acia
// Detect user port type for parallel drive interface
//	#define io_detect_userport
// Detect all drives and select last used drive for loader (#8 if device number is less than 8)
	#define io_detect_drive
// Detect VICE xplus4 and optionally halt when detected
//	#define io_detect_emulator
//	#define io_halt_on_vice
// Detect CPU to get rid of 6510to7501 adapters for serial loaders
//	#define io_detect_cpu_port
// Halt CPU when 6510 and serial drive detected
//	#define io_halt_on_6510
// Include loader code (drive detection and loader)
//	#define need_loader
// Include exomizer on-the-fly decruncher
//	#define need_exodecrunch

	#import "../detect/detect.inc"
