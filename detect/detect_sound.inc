	.print	". sound source detection"
	.namespace iolib {

detect_sound:
{
.label	sidptr	=	2

	lda	sidptr
	pha
	lda	sidptr+1
	pha
// Detect SID address
#if	HAS_TED
	sei
	sta ted.ramen
#endif
	lda #0
	sta io_soundtype
#if	HAS_TED
	ldx #<sid_solder
	ldy #>sid_solder
	jsr io_detsidaddr
	cmp #$ff
	beq soldersid
	ldx #<sid_csory
	ldy #>sid_csory
	jsr io_detsidaddr
	cmp #$ff
	bne nosiddet
soldersid:
#else
	ldx #<$d400
	ldy #>$d400
	stx sidptr
	sty sidptr+1
#endif
	inc io_soundtype
// Detect SID type
	jsr detsidtp
// Detect NAE
#if	HAS_TED
	jsr detnae
#endif
	ldx sidptr
	ldy sidptr+1
	stx io_sidbase
	sty io_sidbase+1
nosiddet:
// Detect AY (DIGIMUZ)
#if	HAS_TED
	jsr detay
#endif
// Detect FM
	jsr detfm

// Print result
#if	HAS_TED
	sta ted.romen
	cli
#endif
	#if	prtstatus
	jsr	primm
	.text	" Sound: "
	.byte	0
#if	HAS_TED
	jsr	primm
	.text	"TED "
	.byte	0
#endif
// Print SID
#if	HAS_TED
	sei
	sta	ted.ramen
#endif
	lda	io_soundtype
	and	#%111
#if	HAS_TED
	sta	ted.romen
	cli
#endif
	beq	nosid
	pha
	and	#%100
	bne	!+
#if	HAS_TED
	jsr	primm
	      // NAE 1.2
	.text	"SIDCard"
	.byte	0
	jmp	!+++
!:	lda	nae_control+15
	pha
	lsr
	lsr
	lsr
	lsr
	pha
	tax
	lda	#'N'
	dex
	beq	!+
	lda	#'C'
!:	sta naecaecardtype
	jsr	chrout
	jsr	primm
	.text	"AE "
	.byte	0
	pla
	ora	#'0'
	jsr	chrout
	lda	#'.'
	jsr	chrout
	pla
	and	#%00001111
	ora	#'0'
	jsr	chrout
!:	lda	#'/'
	jsr	chrout
#endif
	pla
	and	#%10
	asl
	tax
	ldy	#4
!:	lda	io_sidtypes,x
	jsr	chrout
	inx
	dey
	bne	!-
	jsr	prtspc
nosid:	
// Print AY
#if	HAS_TED
	sei
	sta	ted.ramen
#endif
	lda	#%1000
	bit	io_soundtype
#if	HAS_TED
	sta	ted.romen
	cli
#endif
	beq	noay
	jsr	primm
	.text	"AY "
	.byte	0
noay:
// Print FM
#if	HAS_TED
	sei
	sta	ted.ramen
#endif
	lda	#%10000
	bit	io_soundtype
#if	HAS_TED
	sta	ted.romen
	cli
#endif
	beq	nofm
	jsr	primm
	.text	"FM"
	.byte	0
nofm:
	jsr	prtcr
	#endif
	pla
	sta	sidptr+1
	pla
	sta	sidptr
	rts
	
// Detect SID address
//=======================================
//  In: XR: SID address low
//      YR: SID address high
// Out: AC: 0: not found $ff: found
//=======================================

io_detsidaddr:
	lda #0
	sta io_sidbase
	sta io_sidbase+1
	
	stx sidptr
	sty sidptr+1
	
	ldy #24
!:	sta (sidptr),y
	dey
	bpl !-
	
	ldy #25
	lda (sidptr),y
	iny
	and (sidptr),y
	cmp #$ff
	bne !+
	rts
// Either PotX or PotY is not $ff - no SID or paddle is not fully left - try alternative detection
// Reset SID (again)
!:	ldy #24
	lda #0
!:	sta (sidptr),y
	dey
	bpl !-
// Read oscillator 3 - should be always 0
	ldx #0
	ldy #27
!:	lda (sidptr),y
	bne !+
	dex
	bne !-
	lda #$ff
	.byte $2c
!:	lda #0
	rts

//=======================================
// Detect NST SIDCard
//=======================================

#if	HAS_TED
detnae:
	lda #%11100000
	sta nae_control+13
	ldx #0
!:	lda nae_control+15
	cmp nae_control+15
	bne !+
	dex
	bne !-
	and #%11100000
	cmp #%11100000
	beq !+		// Legacy SID-Card
	lda nae_control+15
	beq !+		// VICE :)
	lda io_soundtype
	ora #%100
	sta io_soundtype
!:	rts
#endif
	
// Detect SID chip type
// original version from Reflex Math demo
//=======================================
	
detsidtp:
//Set a low frequency for Oscillator
// (Voice 3) #$0200 which is somewhere
// between B-0 and C-1, according to demo
	
	ldy #15
	lda #$02
	sta (sidptr),y
	
//Set Sawtooth and Triangle waveforms for
//voice 3 and start release of ADSR
	
	ldy #18
	lda #%00110000
	sta (sidptr),y
	
//Check if output of voice 3 ever goes
//higher than $80. It's only possible on
//new SID chip (8580) and never on the
//old one (6581)
	
	ldy #0
	ldx #0
	sty ystore
loop2:
	ldy #27
	lda (sidptr),y
	bmi newsid
	dex
	bne loop2
	ldy ystore
	dey
	sty ystore
	bne loop2
	beq oldsid
newsid:
	lda #%10
	ora io_soundtype
	sta io_soundtype
oldsid:
	rts

#if	HAS_TED
setsidc64:
	#if	prtstatus
	jsr	primm
	.text	" Switching "
naecaecardtype:
	.text	"NAE to C64 clock"
	.byte	13, 0
	#endif
	lda #$d1
	sta nae_control+13
	rts
#endif

#if	HAS_TED
detay:
	ldy	#15
	lda	#0
!:	jsr	writeayreg
	dey
	bpl	!-

	ldy	#1
	lda	#%01011010
	jsr	writeayreg
	jsr	readayreg
	and	#%1111
	cmp	#%1010
	bne	!+
	lda	#%1000
	ora	io_soundtype
	sta	io_soundtype
!:	rts

writeayreg:
	sty	ay.address
	sta	ay.write
	rts

readayreg:
	sty	ay.address
	lda	ay.read
	rts
#endif

detfm:	{
	ldy	#4
	lda	#$60
	jsr	writefmreg
	lda	#$80
	jsr	writefmreg
	lda	fm.status
	and	#$e0
	sta	fmstat1
	ldy	#2
	lda	#$ff
	jsr	writefmreg
	ldy	#4
	lda	#$21
	jsr	writefmreg
	.for	(var i=0; i<72; i++) nop
	lda	fm.status
	and	#$e0
	sta	fmstat2
	ldy	#4
	lda	#$60
	jsr	writefmreg
	lda	#$80
	jsr	writefmreg
	

.label	fmstat1	= *+1
	lda	#0
	bne	!+

.label	fmstat2	= *+1
	lda	#0
	cmp	#$c0
	bne	!+

	lda	#%10000
	ora	io_soundtype
	sta	io_soundtype
!:	rts
}

// 6us: 11 NTSC double cycles
// 35us: 63 NTSC double cycles
writefmreg:
	sty	fm.address
	.for	(var i=0; i<6; i++) nop
	sta	fm.data
	.for	(var i=0; i<32; i++) nop
	rts

ystore:	.byte	0

	#if	prtstatus
io_sidtypes:
	.text	"6581"
	.text	"8580"
	#endif
}
}
