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

	jsr	iolib.detect
	lda	iolib.drivedetect.io_drivetyp
	bne	!+
	rts
!:	jsr	iolib.init
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
	lda	$5bfe
	sta	ted.background
	lda	$5bff
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

.namespace iolib {
	#define	prtstatus
	#define need_video_detect
	#define need_memory_detect
	#define need_sound_detect
	#define need_loader
	#define need_exodecrunch
}
#import "../core/iolib.inc"
