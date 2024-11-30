	#import	"plus4_basic_header.inc"
	#import	"plus4_io_map.inc"
	#import	"ted.inc"
	#import	"plus4_kernal_table.inc"

	.encoding	"petscii_mixed"

.label	charset	=	$4000
.label	bmp	=	$2000

	jsr	primm
	.text	"IOLibV3 demo by Siz (c) 2017.05.10"
	.byte	14, 13, 0
	
	jsr	iolib.detect
	lda	iolib.drivedetect.io_drivetyp
	bne	!+
	rts
!:	jsr	iolib.init
	bcc !+
	rts
!:
	lda	#0
!:	sta	pio_keyboard
	sta	ted.keyblatch
	ldx	ted.keyblatch
	inx
	bne	!-

	jsr	primm
	.text	"Press any key to start..."
	.byte	0

	lda	#0
!:	sta	pio_keyboard
	sta	ted.keyblatch
	ldx	ted.keyblatch
	inx
	beq	!-

	ldx	#0
!:
	.for	(var i=0; i<4; i=i+1) {
	lda	$d400+i*$100,x
	sta	charset+i*$100,x
	}
	inx
	bne	!-

	lda	#0
	sta	loopcnt
	sta	loopcnt+1
	sta	loopcnt+2
	sta	loopcnt+3
	sta	loopcnt+4
	sta	jiffy

	sei
	sta	ted.ramen
	lda	#<irq
	ldx	#>irq
	sta	$fffe
	stx	$ffff
	lda	#2
	sta	ted.rasterirqline
	cli
	
	ldx	#0
	lda	#$70
!:	.for	(var i=0; i<4; i=i+1) {
	sta	$1800+$100*i,x
	}
	inx
	bne	!-
	lda	#$01
!:	.for	(var i=0; i<4; i=i+1) {
	sta	$1c00+$100*i,x
	}
	inx
	bne	!-
	
	lda	#$3b
	sta	ted.ctrl1
	lda	#$18
	sta	ted.screenaddr
	lda	#$08
	sta	ted.bitmapctrl
	
loadloop:
	jsr	rstframe
	ldx	#'b'
	ldy	#'1'
	jsr	iolib.load
	lda #$32
	bcs quit
	inc	framerun
	
	jsr	clr
	
	jsr	incloop
	
	lda	#'R'
	ldx	#'a'
	ldy	#'w'
	sta	modestr
	stx	modestr+1
	sty	modestr+2
	jsr	prtstr

	jsr	rstframe
	ldx	#'e'
	ldy	#'1'
	jsr	iolib.decrunch
	lda #$55
	bcs quit
	inc	framerun
	jsr	clr
	lda	#'E'
	ldx	#'x'
	ldy	#'o'
	sta	modestr
	stx	modestr+1
	sty	modestr+2
	jsr	prtstr
	jmp	loadloop

quit:
	sei
	sta ted.border
	jmp *
	
clr:	ldx	#0
	lda	#$00
!:	.for	(var i=0; i<32; i=i+1) {
	sta	$2000+$100*i,x
	}
	inx
	bne	!-
	rts

irq:	pha
	txa
	pha
	tya
	pha

.label	bordercolor = *+1
	lda	#$71
	ldx	#160
!:	cpx	ted.rastercolumn
	bcs	!-
	sta	ted.border

.label	irqline	= *+1
	lda	#203
	sta	ted.rasterirqline
	
	lda	bordercolor
	eor	#$50
	sta	bordercolor
	
	lda	irqline
	eor	#(203 ^ 2)
	sta	irqline
	
	bpl	!+
	jsr	incframe
	jsr	inctimer
!:
	
	lda	ted.irqsource
	sta	ted.irqsource
	
	pla
	tay
	pla
	tax
	pla
	rti

rstframe:
	ldx	#0
	stx	timecnt
	stx	timecnt+1
	stx	timecnt+2
	stx	timecnt+3
	stx	timecnt+4
	inx
	stx	framerun
!:	rts

incframe:
	lda	framerun
	beq	!-
	ldy	#4
	inc	timecnt+4
!:	ldx	timecnt,y
	inx
	cpx	#10
	bne	!+
	lda	#0
	sta	timecnt,y
	dey
	bpl	!-
!:	txa
	sta	timecnt,y
!:	rts

inctimer:
	inc	jiffy
	lda	jiffy
	cmp	#50
	bne	!-
	lda	#0
	sta	jiffy
	
	inc	runsec+1
	lda	runsec+1
	cmp	#'9'+1
	bne	!-
	lda	#'0'
	sta	runsec+1
	
	inc	runsec
	lda	runsec
	cmp	#'6'
	bne	!-
	lda	#'0'
	sta	runsec

	inc	runmin+1
	lda	runmin+1
	cmp	#'9'+1
	bne	!-
	lda	#'0'
	sta	runmin+1
	
	inc	runmin
	lda	runmin
	cmp	#'6'
	bne	!-
	lda	#'0'
	sta	runmin

	inc	runhour+1
	lda	runhour+1
	cmp	#'9'+1
	bne	!-
	lda	#'0'
	sta	runhour+1
	
	inc	runhour
	lda	runhour
	cmp	#'9'+1
	bne	!-
	lda	#'0'
	sta	runhour

	rts

incloop:
	ldy	#4
!:	ldx	loopcnt,y
	inx
	cpx	#10
	bne	!+
	lda	#0
	sta	loopcnt,y
	dey
	bpl	!-
!:	txa
	sta	loopcnt,y
	ldx	#4
!:	lda	loopcnt,x
	ora	#'0'
	sta	loopstr,x
	dex
	bpl	!-
	rts

prtstr:
	ldx	#0
	ldy	#0
!:	lda	timecnt,x
	ora	#'0'
	sta	timestr,y
	inx
	iny
	cpy	#3
	bne	!+
	iny
!:	cpx	#5
	bne	!--

	lda	#0
	sta	prtchar.dstaddr
	lda	#(>bmp+24*320)
	sta	prtchar.dstaddr+1
	ldx	#0
!:	lda	string,x
	bmi	!+
	cmp	#$40
	bcc	!++
	and	#$1f
!:	and	#$7f
!:	jsr	prtchar
	inx
	cpx	#32
	bne	!---
	rts

prtchar:
	{
	sta	savea
	stx	savex
	lda	#0
	sta	srcaddr+1
	lda	savea
	asl
	rol	srcaddr+1
	asl
	rol	srcaddr+1
	asl
	rol	srcaddr+1
	sta	srcaddr
	lda	srcaddr+1
	ora	#>charset
	sta	srcaddr+1
	ldx	#7
.label	srcaddr=*+1
!:	lda	charset,x
.label	dstaddr=*+1
	sta	bmp+24*320,x
	dex
	bpl	!-
	lda	#8
	clc
	adc	dstaddr
	sta	dstaddr
	bcc	!+
	inc	dstaddr+1
!:
.label	savex=*+1
	ldx	#0
.label	savea=*+1
	lda	#0
	}
	rts

loopcnt:
	.fill	5, 0
timecnt:
	.fill	5, 0
framerun:
	.byte	0
jiffy:	
	.byte	0

string:
	.text	"L: "
loopstr:
	.text	"01234 T: "
timestr:

	.text	"012.34s "
modestr:
	.text	"Raw"
	.text	" "
runhour:
	.text	"00:"
runmin:
	.text	"00:"
runsec:
	.text	"00"
	.fill	32, ' '

.namespace iolib {
	#define	prtstatus
	#define	bypass_vice
	#define need_cpu_detect
	#define need_video_detect
	#define need_memory_detect
	#define need_sound_detect
	#define need_loader
	#define need_exodecrunch
}
#import "iolib.inc"
