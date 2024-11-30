	.print	". drive detection"
	.namespace iolib {

//#define DEBUG
//#define TEST_C16STM
#undef HAS_BURST
#undef HAS_IEEE

detect_drive:
.namespace drivedetect {
.label	io_ptr		= 2
#if	HAS_TED
.label	currdrive	= $ae
#else
.label	currdrive	= $ba
#endif
.const	lastdrive	= 15
io_det_drive:
#if	HAS_TED
	#import "plus4_io_map.inc"
#if DEBUG
	lda ted.border
	pha
#endif
#endif
	lda #0
	sta nodefaultdrive
	sta c16stmdevice
// default to drive 8 if last used # is less than that
	lda	#8
	cmp	currdrive
	bcc	!+
	inc nodefaultdrive
	sta	currdrive
!:	lda	#0
	sta	io_drivetyp
	sta	io_driveiftyp
	sta	io_drivesernum
	#if	prtstatus
	jsr	primm
	.text	@" Detected devices:\$0d\$00"
	#endif
	jsr	io_detectdrvtp	
	#if	prtstatus
	jsr	primm
	.text	@" Using drive #\$00"
	jsr	io_prtdrivenum
	jsr	prtcr
	#endif
#if	HAS_TED
#if DEBUG
	pla
	sta ted.border
#endif
#endif
	rts
	
io_detectdrvtp:
	lda	currdrive
	pha
// Start drive detection from unit number #4
	ldx	#4
io_detnextdrvtp:
	stx	currdrive
	#if prtstatus
	jsr	primm
	.text	@"  #\$00"
	jsr	io_prtdrivenum
	#endif
	jsr	io_detcurrdrv
	cpy #5 // 1551?
	beq forcestatus
	cpy	#0
	bne	!+
// Detection by ROM read failed. Try FlexSD
	jsr	detflexsd
	bcc	!+
// FlexSD firmware not present. Try status read
forcestatus:
	jsr	io_detbysts
// Detect interface for drive
!:
	#if HAS_TED
// Detect failed. Try C16STM anyway
	lda io_drivetyp
	cmp #$ff
	bne !+
	jsr	io_detc16stm
!:
	#endif
	jsr	io_detdrvif
	#if	prtstatus
// Print detected drive and interface type
	lda	io_drivetyp
	cmp	#$ff
	bne !+
	jmp	io_detdrvnotpresent
!:	lda	#':'
	jsr	chrout
	jsr	prtspc
	lda	io_drivetyp
	asl
	asl
	asl
	asl
	tax
	ldy	#16
!:	lda	io_drivenames,x
	cmp	#' '
	beq	!+
	jsr	chrout
	inx
	dey
	bne	!-
!:	jsr	prtspc
	lda	io_driveiftyp
	asl
	asl
	asl
	asl
	tax
	ldy	#16
!:	lda	io_ifnames,x
	jsr	chrout
	inx
	dey
	bne	!-
// Print init message for unknown drive type
{
	lda io_drivetyp
	bne !+++
#if	HAS_TED
	lda mraddr+1
	jsr hexbout
	lda mraddr
	jsr hexbout
	lda #':'
	jsr chrout
	lda mrdta
	jsr hexbout
#endif
	jsr	prtcr
	ldx	#'u'
	ldy	#'i'
	jsr readstatus
	lda statustxt
	beq io_detdrvnotpresent
	ldx #6
!:	jsr prtspc
	dex
	bpl	!-
	ldx #3
!:	lda statustxt,x
	cmp #','
	beq !+
	jsr chrout
	inx
	bne !-
!:	jsr prtcr
}
	#endif	
// Device number not responding. Try next one
io_detdrvnotpresent:
	#if prtstatus
	lda	io_drivetyp
	cmp	#$ff
	bne !+
#if DEBUG
	jsr prtcr
#else
	#if HAS_TED
	lda #$1b
	jsr chrout
	lda #'d'
	jsr chrout
	#else
	lda #$14
	jsr chrout
	jsr chrout
	jsr chrout
	jsr chrout
	jsr chrout
	#endif
#endif
!:	
	#endif
	pla
	pha
	cmp	currdrive
	bne !+
	// this is the drive we use: save detected interfaces
	lda	io_drivetyp
	pha
	lda	io_driveiftyp
	pha
#if	HAS_TED
.if (iobase > $8000) {
	sei
	sta	ted.ramen
}
	lda	io_tcbmoffs
	pha
.if (iobase > $8000) {
	sta	ted.romen
	cli
}
#endif
!:
	ldx	currdrive
	inx
	cpx	#lastdrive+1
	beq !+
	jmp	io_detnextdrvtp
!:
	#if	prtstatus
	jsr	primm
	.text	@" Devices connected to serial bus: \$00"
	lda	io_drivesernum
	ora	#'0'
	jsr	chrout
	jsr	prtcr
	#endif
	// restore interface and current drive
#if	HAS_TED
	pla
	sta	io_tcbmoffs
#endif
	pla
	cmp	#1
	bne	!+ // not serial
	ldx	io_drivesernum
	dex
	beq	!+ // only one drive on bus
	lda	#2
!:	sta	io_driveiftyp
	pla
	sta	io_drivetyp
	pla
	sta	currdrive
	lda nodefaultdrive
	beq	!+
	lda c16stmdevice
	beq !+
	// No default drive was set and C16STM is detected: use that
	sta currdrive
	lda #13
	sta io_drivetyp
	lda #7
	sta io_driveiftyp
!:	rts
	
io_detcurrdrv:
#if	HAS_TED
#if DEBUG
	lda #$32
	sta ted.border
#endif
#endif
	ldx	#0
	stx	io_drivetyp
	lda	io_drvaddr,x
	ldy	io_drvaddr+1,x
nextchk:
	sta	mraddr
	sty	mraddr+1
	jsr	readmem
#if	HAS_TED
#if DEBUG
	lda #$44
	sta ted.border
#endif
#endif
	bcs	notexist
sameaddr:
	ldy	io_drvaddr+3,x
	lda	io_drvaddr+2,x
	cmp	mrdta
	bne	nomatch
	cpy	#0
	beq	nexttyp
	bne	foundtyp
	
nomatch:
	cpy	#0
	beq	notfound
	
nexttyp:
	inx
	inx
	inx
	inx
	cpx	#io_drvaddre-io_drvaddr
	beq	notfound
	
	lda	io_drvaddr,x
	ldy	io_drvaddr+1,x
	cmp	mraddr
	beq	donext
	bne	nextchk
notfound:
#if	HAS_TED
#if DEBUG
	lda #$77
	sta ted.border
#endif
#endif
	rts
donext:
	cpy	mraddr+1
	beq	sameaddr
	bne	nextchk
	
notexist:
	ldy	#$ff
foundtyp:
#if	HAS_TED
#if DEBUG
	lda #$55
	sta ted.border
#endif
#endif
	sty	io_drivetyp
	rts
	
// Read drive memory
//=======================================
readmem:
#if DEBUG
	lda #'r'
	jsr chrout
#endif
	stx	xtmp
	lda	#15
	ldx	currdrive
	tay
	jsr	setlfs
#if DEBUG
	lda #'l'
	jsr chrout
#endif
	lda	#6
	ldx	#<mrtxt
	ldy	#>mrtxt
	jsr	setnam
#if DEBUG
	lda #'n'
	jsr chrout
#endif
	jsr	open
#if DEBUG
	lda #'+'
	jsr chrout
#endif
	lda	st
	bne	!+
	ldx	#15
	jsr	chkin
#if DEBUG
	lda #'<'
	jsr chrout
#endif
	lda	st
	bne	!+
	jsr	chrin
	sta	mrdta
#if DEBUG
	lda #'r'
	jsr chrout
#endif
// do not check ST so printers can be detected
//	lda	st
//	bne	+
	clc
	.byte	BIT_ZP
!:	sec
	php
	jsr	clrchans
#if DEBUG
	lda #'c'
	jsr chrout
#endif
	lda	#15
	jsr	close
#if DEBUG
	lda #'-'
	jsr chrout
#endif
	plp
	ldx	xtmp
	rts

detflexsd:
{
#if DEBUG
	lda #'f'
	jsr chrout
#endif
	ldx	#'z'
	ldy	#'i'
	jsr	readstatus
	bcc	!+
	rts
!:
#if DEBUG
	lda #'.'
	jsr chrout
#endif
	lda	statustxt
	cmp	#'3'
	beq	!+
	ldy	#11
	sty	io_drivetyp
	clc
	.byte BIT_ZP
!:	sec
#if DEBUG
	php
	pla
	pha
	and #1
	ora #'0'
	jsr chrout
	plp
#endif
	rts
}

io_detbysts: {
#if DEBUG
	lda #'?'
	jsr chrout
#endif
	ldx	#'u'
	ldy	#'i'
	jsr	readstatus
	bcc	!+
	rts
!:
	lda statustxt
	cmp #'7'
	beq !+
	ldx #'u'		// workaround for YaPE IEC which does not support UI
	ldy #'j'
	jsr	readstatus
	bcc	!+
	rts
!:
#if DEBUG
	lda #'!'
	jsr chrout
#endif
// analyze result status string
	lda	#<io_drivestsstr
	ldx	#>io_drivestsstr
	sta	io_ptr
	stx	io_ptr+1
cmpsts:	ldx	#0
nextpos:
	ldy	#0
cmptxt:	lda	statustxt,x
	beq	nextsts
	cmp	(io_ptr),y
	beq	nextchr
	lda	(io_ptr),y
	beq	stsfnd
	inx
	bne	nextpos
nextchr:
	inx
	iny
	cpy	#7
	bne	cmptxt
stsfnd:	ldy	#7
	lda	(io_ptr),y
	sta	io_drivetyp
	bne	!++
nextsts:
	clc
	lda	#8
	adc	io_ptr
	sta	io_ptr
	bcc	!+
	inc	io_ptr+1
!:	cmp	#<io_drivestsstre
	bne	cmpsts
!:	clc
	rts
}

readstatus:
{
	stx	command1
	sty	command2
	ldx	#statustxte-statustxt-1
	lda	#0
!:	sta	statustxt,x
	dex
	bpl	!-
	ldx	currdrive
	lda	#15
	tay
	jsr	setlfs
	lda	#0
	jsr	setnam
	jsr	open
	lda	st
	beq	!+
	jmp	enderr
!:	ldx	#15
	jsr	chkout
	lda	st
	bne	enderr
	lda	command1: #0
	jsr	chrout
	lda	st
	bne	enderr
	lda	command2: #0
	jsr	chrout
	lda	st
	bne	enderr
	jsr clrchans
	lda #15
	jsr close
	ldx #0
!:	dex
	bne !-
#if HAS_TED
!:	bit cpu_port
	bpl !-
#endif
	ldx	currdrive
	lda	#15
	tay
	jsr	setlfs
	lda	#0
	jsr	setnam
	lda	#0
	sta	statuslen
	jsr	open
	lda	st
	bne	enderr
	ldx	#15
	jsr	chkin
	lda	st
	bne	enderr
!:	jsr	chrin
	ldx	statuslen
	sta	statustxt,x
	inc	statuslen
	bit	st
	bvs	!+
	lda	st
	bne	enderr
	beq	!-
!:	lda #2
	bit st
	bne enderr
	clc
	.byte BIT_ZP
enderr:	sec
	php
	jsr	clrchans
	lda	#15
	jsr	close
// Wondering why was this here??
//	lda #0
//	sta statustxt
	plp
	rts
}

io_detdrvif:
	lda	#0
	sta	io_driveiftyp
	ldx	#0
!:	lda	iftable,x
	cmp	io_drivetyp
	bne	!+
	ldy	iftable+1,x
	jsr	io_detdrvcurrif
	bcc	!++
!:	inx
	inx
	cpx	#iftblend-iftable
	bne	!--
!:	rts

io_detdrvcurrif:
{
	tya
	pha
	dey
	bpl	!+
	// YR became negative -> it was 0 (unknown) - increase serial count to be sure
ser:	inc	io_drivesernum
end:	pla
	sta	io_driveiftyp
	clc
	rts
!:	beq	ser	// YR was 1 (serial)
	dey		// can't be 2 because that one is reserved
	dey
	bne	nottcbm
	// YR was 3 (TCBM)
#if	HAS_TED
	lda	currdrive
	ldy	#$30
	cmp	#8
	beq	!+
	bcc	io_detdrvcurriffail
	cmp	#10
	bcs	io_detdrvcurriffail
	ldy	#0
!:	sty	io_tcbmoffs
	lda	#%01010101
	sta	tcbm_9,y // data
	eor	tcbm_9,y
	bne	io_detdrvcurriffail
	lda	tcbm_9+1,y // status
	and	#%00000010
	bne	io_detdrvcurriffail
	beq	end
#endif
nottcbm:
	dey
	bne	io_notpar
	// YR was 4 (parallel) - do actual parallel probe
#if	HAS_TED
	lda	#$ff
	sta	pio_parport
	sta	mwdta
	lda	#<$1803
	ldy	#>$1803
	sta	mwaddr
	sty	mwaddr+1
	jsr	writemem
	lda	#<$1801
	sta	mwaddr
	lda	#$aa
	sta	mwdta
	jsr	writemem
	jsr	chkfd10
	bcs	io_detdrvcurriffail
	lda	#$55
	sta	mwdta
	jsr	writemem
	jsr	chkfd10
	bcc	ser
	bcs io_detdrvcurriffail
#endif
io_notpar:
	dey		// 5: TCBM standard
	beq end // always detected
	dey		// 6: IEEE-488 - not supported yet
	dey		// 7: expansion port
	bne	io_notexpansionport
	beq end // always detected
io_notexpansionport:
io_detdrvcurriffail:
	pla
	sec
	rts
}

#if	HAS_TED
chkfd10:
	ldy	#0
	lda	pio_parport
	cmp	mwdta
	beq	!+
	dey
	bne	chkfd10+2
	sec
	.byte BIT_ZP
!:	clc
	rts
#endif

// Write drive memory
//=======================================
writemem:
	stx	xtmp
	lda	#15
	ldx	currdrive
	tay
	jsr	setlfs
	lda	#7
	ldx	#<mwtxt
	ldy	#>mwtxt
	jsr	setnam
	jsr	open
	lda	#15
	jsr	close
	ldy	#0
	ldx	#0
!:	dex
	bne	!-
	dey
	bne	!-
	ldx	xtmp
	rts

#if HAS_TED
io_detc16stm: {
#if TEST_C16STM
	lda currdrive
	cmp #9
	bne nodevice
	sta c16stmdevice
	ldy	#13
	sty	io_drivetyp
	clc
	rts
#endif
	ldx #$08
!:	txa
	jsr W8051
#if DEBUG
	php
	lda #'/'
	jsr chrout
	plp
#endif
	bne nodevice
	dex
	bne !-
	beq !+

nodevice:
	sec
	rts
!:
#if DEBUG
	lda #'.'
	jsr chrout
#endif
	lda #$6D			// Get config word, response is a single byte
	jsr sendcommand
#if DEBUG
	pha
	jsr hexbout
	pla
#endif
	and #$0f
	cmp currdrive
	bne nodevice
	sta c16stmdevice
	ldy	#13
	sty	io_drivetyp
	clc
	rts

W8051:
	sta temp
	pha
	lda #$62			// Echo
	jsr sendbyte
	pla
	jsr sendbyte
	jsr sendbyte
	eor #$FF
.label temp=*+1
	cmp #0
	rts

sendcommand:
	jsr sendbyte
sendbyte:
	ldy #0
	sta c16stm.data
!:
	dey
	beq !+
	lda c16stm.status
	bmi !-
!:	lda c16stm.data
	rts
}	
#endif

	#if	prtstatus
io_prtdrivenum:
	ldx	#0
	lda	currdrive
!:	cmp	#10
	bcc	!+
	inx
	sec
	sbc	#10
	bcs	!-
!:	pha
	txa
	ora	#'0'
	jsr	chrout
	pla
	ora	#'0'
	jmp	chrout

io_drivenames:
	       //0123456789abcdef
	.text	"unknown         " //  0
	.text	"1540            " //  1
	.text	"1541            " //  2
	.text	"1541C           " //  3
	.text	"1541-II         " //  4
	.text	"1551            " //  5
	.text	"1570            " //  6
	.text	"1571            " //  7
	.text	"1581            " //  8
	.text	"RF-501C         " //  9
	.text	"SD2IEC          " // 10
	.text	"SD2IEC/FlexSD   " // 11
	.text	"FD-2000         " // 12
#if HAS_TED
	.text	"C16STM          " // 13
	.text	"YaPE-IEC        " // 14
	.text	"TCBM2SD         " // 15
#endif
io_ifnames:
		//0123456789abcdef
	.text	"unsupported     " // 0
	.text	"serial          " // 1
	.text	"                " // 2 // placeholder for serial with multiple drives connected
	.text	"TCBM            " // 3
	.text	"parallel        " // 4
	.text	"TCBM standard   " // 5
	.text	"IEEE-488        " // 6
	.text	"expansion port  " // 7
	#endif

//Structure:
// 0: address low
// 1: address high
// 2: memory value
// 3: 0: if value doesn't match=>not found, try next address
//    other: drive type id
	
io_drvaddr:
	.byte	$00,$c0,$4d,12 //FD-2000
	.byte	$00,$c0,$c0,8  //1581
	.byte	$00,$c0,$4a,8  //1581 (1563)
	.byte	$00,$c0,$2b,8  //1581 (beta)
	.byte	$00,$c0,$38,7  //1571
	.byte	$00,$c0,$84,7  //1571
	.byte	$00,$c0,$42,7  //1571
	.byte	$00,$c0,$be,5  //1551
	.byte	$00,$c0,$51,6  //1570
	.byte	$00,$c0,$ff,9  //RF-501C
	.byte	$00,$c0,$97,0
	.byte	$01,$c0,$e0,4  //1541/II
	.byte	$01,$c0,$46,3  //1541C
	.byte	$01,$c0,$aa,0
	.byte	$fa,$ff,$e7,1  //1540
	.byte	$fa,$ff,$01,0
	.byte	$a3,$ea,$fe,3  //1541C
	.byte	$a3,$ea,$ff,2  //1541
io_drvaddre:

// format: null terminated string up to 6 characters or 7 characters without null termination
// 	byte 8 is drive type number
io_drivestsstr:
	       //0123456
	.text	@"sd2iec\$00"
	.byte	10
	.text	@"yape\$00\$00\$00"
	.byte	14
	.text	@"tcbm2sd"
	.byte	15
//	.text	"tdisk"
//	.byte	0, 0, 10 // SD2IEC test
io_drivestsstre:

// Format: drive type, interface type
// 	must be ordered by preffered one first
iftable:
#if HAS_TED
	.byte 13,7//C16STM, expansion port
	.byte 14,5//YAPE IEC, TCBM standard
	.byte 15,5//SD2TCBM, TCBM standard
#endif
//	.byte 0,3 //unknown,TCBM
	.byte 0,0 //unknown,unknown
	.byte 1,1 //1540,serial
	.byte 2,4 //1541,parallel
	.byte 2,1 //1541,serial
	.byte 3,1 //1541C,serial
	.byte 4,4 //1541/II,parallel
	.byte 4,1 //1541/II,serial
#if HAS_TED
	.byte 5,3 //1551,TCBM
#endif
//	.byte 6,4 //1570,parallel
/*
#if HAS_BURST
	.byte 6,5 //1570,serial burst
#endif
*/
	.byte 6,1 //1570,serial
//	.byte 7,4 //1571,parallel
/*
#if HAS_BURST
	.byte 7,5 //1571,serial burst
#endif
*/
	.byte 7,1 //1571,serial
/*
#if HAS_BURST
	.byte 8,5 //1581,serial burst
#endif
*/
	.byte 8,1 //1581,serial
	.byte 9,1 //RF501C,serial
	.byte 10,1//SD2IEC,serial
	.byte 11,1//SD2IEC/FlexSD,serial
	.byte 12,1//FD-2000,serial
iftblend:

nodefaultdrive:
	.byte	0
c16stmdevice:
	.byte	0
io_drivetyp:
	.byte	0
io_driveiftyp:
	.byte	0
io_drivesernum:
	.byte	0
mrtxt:	.text	"m-r"
mraddr:	.word	0
	.byte	1
mrdta:	.byte	0
mwtxt:	.text	"m-w"
mwaddr:	.word	0
	.byte	1
mwdta:	.byte	0
xtmp:	.byte	0
statuslen:
	.byte	0
statustxt:
	.fill	64, 0
statustxte:
}
}
