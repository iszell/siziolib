	#import	"plus4_basic_header.inc"
	#import	"plus4_io_map.inc"
	#import	"ted.inc"
	#import	"plus4_kernal_table.inc"

	.encoding	"petscii_mixed"

	jsr	primm
	.text	"IOLibV3 test by Siz (c) 2014.06.03"
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
	lda	#0
	sta	counter
	sta	counter+1
	sta	counter+2
	sta	counter+3
	cli
	ldx	#'t'
	ldy	#'e'
	jsr	iolib.load
	php
	ldx #0
	ldy #0
	jsr iolib.load
	lda	#%00000010
	sta	ted.irqmask
	lda	$efff
	sta	ted.romen
	tax
	lda	#0
	jsr	lnprt
	jsr	iolib.prtspc
	plp
	bcs	!+
	jsr	primm
	.text	"no "
	.byte	0
!:	jsr	primm
	.text	"error"
	.byte	13
	.text	"load took "
	.byte	0
	lda	counter
	ldx	counter+1
	jsr	lnprt
	lda	#':'
	jsr	chrout
	lda	counter+2
	ldx	counter+3
	jsr	lnprt
	jsr	primm
	.text	" frames"
	.byte	13, 0
	rts
	
irq:	inc	ted.border
	pha
	lda	ted.irqsource
	sta	ted.irqsource
	inc	counter+3
	bne	!+
	inc	counter+2
	bne	!+
	inc	counter+1
	bne	!+
	inc	counter
!:	pla
	dec	ted.border
	rti

counter:
	.word	0
	.word	0

.namespace iolib {
	#define	prtstatus
	#define need_video_detect
	#define need_memory_detect
	#define need_sound_detect
	#define need_loader
}
#import "../core/iolib.inc"
