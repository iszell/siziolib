	#import	"plus4_basic_header.inc"
	#import	"plus4_io_map.inc"
	#import	"ted.inc"
	#import	"plus4_kernal_table.inc"

	.encoding	"petscii_mixed"
	
	jsr	iolib.detect
	lda	iolib.drivedetect.io_drivetyp
	cmp #$ff
	bne	!+
	jsr primm
	.byte 13
	.text "no supported drive found. exiting."
	.byte 13, 0
	rts
!:	jsr	iolib.init

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
	
error:	lda	#$32
	sta	ted.border
	rts
}

.namespace iolib {
/*
// Print IOLib status messages
	#define prtstatus
*/
/*
// Bypass VICE xplus4 detection and quitting when VICE is detected
	#define bypass_vice
*/
// Detect CPU to get rid of 6502to7501 adapters
	#define need_cpu_detect
// Detect video standard. Not really useful except for printing status message
	#define need_video_detect
// Detect memory size and expansion type
	#define need_memory_detect
// Detect available sound expansions including SID type and address
	#define need_sound_detect
// Include loader code (drive detection and loader)
	#define need_loader
// Include exomizer on-the-fly decruncher
	#define need_exodecrunch
}
#import "core/iolib.inc"
