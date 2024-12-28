	#import	"plus4_basic_header.inc"
	#import	"plus4_io_map.inc"
	#import	"ted.inc"
	#import	"plus4_kernal_table.inc"

	.encoding	"petscii_mixed"

	jsr	primm
	.text	"IOLibV3 Exomizer test by Siz"
	.byte	13
	.text	"(c) 2016.04.02"
	.byte	14, 13, 0

	jsr	iolib.detect.detect
	jsr	iolib.loader.init
	bcc !+
	rts
!:
	jsr	primm
	.byte	13
	.text	"Testing loader: "
	.byte	0
	sei
	lda	#<irq
	ldx	#>irq
	sta	$fffe
	stx	$ffff
	lda	#%00000010
	sta	ted.irqmask
	lda	#220
	sta	ted.rasterirqline
	sta	ted.ramen

	cli
	ldx	#'e'
	ldy	#'x'
	jsr	iolib.decrunch
//	php
//	ldx #0
//	ldy #0
//	jsr iolib.load
//	plp
	sta	ted.romen
	bcs	error
	jsr	primm
	.text	"done"
	.byte	13, 0
	lda	#$3b
	sta	ted.ctrl1
	lda	#$18
	sta	ted.ctrl2
	lda	#$18
	sta	ted.bitmapctrl
	lda	#$58
	sta	ted.screenaddr
	lda	#$31
	sta	ted.background
	lda	#$00
	sta	ted.color1
	lda	#0
	sta	$ff19
	jmp	*
	
error:	jsr	primm
	.text	"load error"
	.byte	13, 0
	rts
	
irq:	inc	ted.border
	pha
	lda	ted.irqsource
	sta	ted.irqsource
	pla
	dec	ted.border
	rti

	#define io_prtstatus
// Detect video standard. Not really useful except for printing status message
	#define io_detect_video
// Detect memory size and expansion type
	#define io_detect_memory_size
// Detect available sound expansions including SID type and address
	#define io_detect_sound
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
