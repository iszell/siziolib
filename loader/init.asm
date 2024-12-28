	#import	"plus4_basic_header.inc"
	#import	"plus4_kernal_table.inc"
	#import	"ted.inc"

	.encoding	"petscii_mixed"
	
	jsr	iolib.detect.detect
	lda	iolib.detect.drive_model
	bne	!+
	jsr primm
	.byte 13
	.text "no supported drive found. exiting."
	.byte 13, 0
	rts
!:	jsr	iolib.loader.init

	sei
	sta	ted.ramen
	ldx	#23
!:	lda	bootcode,x
	sta	$fe8,x
	dex
	bpl	!-
	ldx	#'i'
	ldy	#'i'
	jmp	$fe8

bootcode: .pseudopc $fe8 {
	jsr	iolib.decrunch
	sta	ted.romen
	cli
	bcs	error
	jmp	$100d
	
error:
	lda	#$32
	sta	ted.border
	rts
}

//	#define io_prtstatus
// Detect video standard. Not really useful except for printing status message
//	#define io_detect_video
// Detect memory size and expansion type
//	#define io_detect_memory_size
// Detect available sound expansions including SID type and address
//	#define io_detect_sound
	#define io_detect_drive
// Detect VICE xplus4 and optionally halt when detected
//	#define io_detect_emulator
//	#define io_halt_on_vice
// Detect CPU to get rid of 6502to7501 adapters
	#define io_detect_cpu_port
	#define io_halt_on_6502

// Include loader code (drive detection and loader)
	#define need_loader
// Include exomizer on-the-fly decruncher
	#define need_exodecrunch

	#import "../core/iolib.inc"
