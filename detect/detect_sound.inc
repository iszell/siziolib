	.memblock "Sound detection"
	.print	". Sound source detection"

#if HAS_AY
	#import	"ay.inc"
#endif
#if HAS_FM
	#import	"fm.inc"
#endif

	.namespace iolib {
	.namespace detect {
	.namespace detect_sound {

detect_sound:
{
.label	sidptr	=	2

	#if io_prtstatus
		jsr primm
		.text @" Detected sound sources:\$0d\$00"
		#if HAS_TED
			jsr primm
			.text @"  TED\$0d\$00"
		#endif
	#endif
	jsr detect_sid
	#if HAS_AY
		jsr detect_ay
	#endif
	#if HAS_FM
		jsr detect_fm
	#endif
	#if HAS_DAC
		jsr detect_dac
	#endif
	rts

detect_sid: {
	#if HAS_TED
		lda #<sid_solder
		ldx #>sid_solder
		sta sidptr
		stx sidptr+1
		jsr detect_sid_address
		bcc foundsid
	#if io_prtstatus
		bit debug_mode
		bpl !+
		jsr prtcr
	!:
	#endif
		lda #<sid_csory
		ldx #>sid_csory
		sta sidptr
		stx sidptr+1
		jsr detect_sid_address
	#endif
	#if !HAS_TED
		lda #<sid_base
		ldx #>sid_base
		sta sidptr
		stx sidptr+1
		jsr detect_sid_address
	#endif
	
	bcc foundsid
	#if io_prtstatus
		bit debug_mode
		bpl !+
		jsr prtcr
	!:	sec
	#endif
	rts

foundsid:
	lda sidptr
	ldx sidptr+1
	sta sid_1_address
	stx sid_1_address+1
	jsr detect_sid_model
	ldx #1
	bcs !+
	inx
!:	stx sid_1_model
	#if !HAS_TED
		lda #1
		sta sid_card_type
	#endif
	#if HAS_TED
		lda #2
		sta sid_card_type
		jsr detect_nae
		beq nonae
		inc sid_card_type
		and #$7f
		sta sid_card_version
	nonae:
	#endif
	#if io_prtstatus
		bit debug_mode
		bpl !+
		jsr prtcr
	!:
		jsr primm
		.text @"  SID \$00"
		lda sidptr
		ldx sidptr+1
		jsr printaddr
		lda sid_1_model
		asl
		asl
		tax
		ldy #4
	!:	lda sid_models-4,x
		jsr chrout
		inx
		dey
		bne !-
		#if HAS_TED
			lda sid_card_type
			cmp #3
			bne noprintnae
			lda #','
			jsr chrout
			jsr prtspace
		printnae:
			ldx #'N'
			lda sid_card_version
			and #$70
			cmp #$10
			beq !+
			ldx #'C'
		!:	txa
			jsr chrout
			jsr primm
			.text @"AE v\$00"
			lda sid_card_version
			lsr
			lsr
			lsr
			lsr
			ora #'0'
			jsr chrout
			lda #'.'
			jsr chrout
			lda sid_card_version
			and #$0f
			ora #'0'
			jsr chrout
		noprintnae:
			jsr prtcr
		#endif
	#endif
	rts

sid_models:
	.text	"6581"
	.text	"8580"
}

#if HAS_AY
detect_ay: {
.const AY_TEST_DATA = %01011010
	ldy	#15
	lda	#0
!:	jsr	writeayreg
	dey
	bpl	!-

	ldy	#1
	lda	#AY_TEST_DATA
	jsr	writeayreg
	jsr	readayreg
	#if io_prtstatus
		bit debug_mode
		bpl !+
		pha
		jsr primm
		.text @"?AY: \$00"
		pla
		pha
		jsr hexbout
		jsr prtcr
		pla
	!:
	#endif
	ldx #2
	cmp #AY_TEST_DATA
	beq ym2149
	cmp	#AY_TEST_DATA & $0f
	bne	noay
	dex
ym2149:
	stx ay_model
	lda #<ay_base
	ldx #>ay_base
	sta ay_address
	stx ay_address+1
	#if io_prtstatus
		jsr primm
		.text @"  AY  \$00"
		lda ay_address
		ldx ay_address+1
		jsr printaddr
		
		ldx ay_model
		dex
		bne ym
		jsr primm
		.text @"AY-3-891x\$00"
		jmp !+
	ym:	jsr primm
		.text @"YM2149\$00"
	!:	jsr prtcr
	#endif
	clc
	.byte BIT_ZP
noay:
	sec
	rts

writeayreg:
	sty	ay.address
	sta	ay.write
	rts

readayreg:
	sty	ay.address
	lda	ay.read
	rts
}
#endif

#if HAS_FM
detect_fm: {
.label addressptr	= sidptr
.label dataptr		= addressptr+2
.label statusptr	= dataptr+2

	sei
	#if HAS_TED
		ldx #<(soundx.address)
		lda #>(soundx.address)
		stx addressptr
		sta addressptr+1
		ldx #<(soundx.data)
		stx dataptr
		sta dataptr+1
		ldx #<(soundx.status)
		stx statusptr
		sta statusptr+1
		jsr detect_opl
		bcs !+
		lda #3
		sta fm_card_type
		bne found
	#endif
!:	ldx #<(fm.address)
	lda #>(fm.address)
	stx addressptr
	sta addressptr+1
	ldx #<(fm.data)
	stx dataptr
	sta dataptr+1
	ldx #<(fm.status)
	stx statusptr
	sta statusptr+1
	jsr detect_opl
	bcs notfound
	#if HAS_TED
		lda #2
	#else
		lda #1
	#endif
	sta fm_card_type

found:
	lda #1
	sta fm_model
	lda addressptr
	ldx addressptr+1
	sta fm_address
	stx fm_address+1
	#if io_prtstatus
		jsr primm
		.text @"  FM  \$00"
		lda fm_address
		ldx fm_address+1
		jsr printaddr
		jsr primm
		.text @"OPL/OPL-II, \$00"
		ldx fm_card_type
		dex
		bne !+
		jsr primm
		.text @"Sound Expander\$00"
		jmp foundend
	!:	dex
		bne !+
		jsr primm
		.text @"SideKick\$00"
		jmp foundend
	!:	jsr primm
		.text @"SoundX\$00"
foundend:
		jsr prtcr
	#endif
	clc
	.byte BIT_ZP
notfound:
	sec
	cli
	rts

	detect_opl: {
		#if io_prtstatus
			bit debug_mode
			bpl !+
			jsr primm
			.text @"?FM: \$00"
			lda addressptr+1
			jsr hexbout
			lda addressptr
			jsr hexbout
			lda #':'
			jsr chrout
		!:
		#endif

		ldy #0
		ldx	#4
		lda	#$60
		jsr	writefmreg
		lda	#$80
		jsr	writefmreg
		lda	(statusptr),y
		and	#$e0
		sta	fmstat1
		ldx	#2
		lda	#$ff
		jsr	writefmreg
		ldx	#4
		lda	#$21
		jsr	writefmreg
		.for	(var i=0; i<72; i++) nop
		lda	(statusptr),y
		and	#$e0
		sta	fmstat2
		ldx	#4
		lda	#$60
		jsr	writefmreg
		lda	#$80
		jsr	writefmreg

		lda	fmstat1: #0
		bne	!+

		lda	fmstat2: #0
		cmp	#$c0
		beq	found
	!:
		sec
		.byte BIT_ZP

	found:
		clc

		#if io_prtstatus
			php
			bit debug_mode
			bpl !+
			lda detect_opl.fmstat1
			jsr hexbout
			jsr prtspace
			lda detect_opl.fmstat2
			jsr hexbout
			jsr prtcr
		!:	plp
		#endif
		rts
	}

	//  3.3us after address  write: 6 NTSC double cycles, 4 C64 cycles
	// 35.0us after register write: 42 NTSC double cycles, 24 C64 cycles
	// see: https://c64.xentax.com/index.php/15-testing-ym3812-register-write-timing
	writefmreg: {
		pha
		txa
		sta	(addressptr),y
		.for	(var i=0; i<3; i++) nop
		pla
		sta	(dataptr),y
		.for	(var i=0; i<21; i++) nop
		rts
	}
}
#endif

#if HAS_DAC
detect_dac: {
/*
	#if !HAS_TED
*/
		sec
		rts
/*
	#endif
	#if HAS_TED
		lda sid_card_type
		cmp #3
		bne nonae
		lda #2
		bne found
	nonae:
		lda sid_solder+$1f
		and sid_solder+$1f
		tax
		lda sid_csory+$1f
		and sid_csory+$1f
		stx dac1
		sta dac2
		lda dac1: #0
		cmp dac2: #0
		bne notfound
		cmp #0
		beq notfound
		lda #1
		bne found
	notfound:
		sec
		.byte BIT_ZP
	found:
		clc
		php
		pha
	#if io_prtstatus
		bit debug_mode
		bpl !+
		jsr primm
		.text @"?DAC: \$00"
		lda dac1
		jsr hexbout
		jsr prtspace
		lda dac2
		jsr hexbout
		jsr prtcr
	!:
	#endif
		pla
		plp
		bcc !+
		rts
	!:	sta dac_model
		clc
		lda sid_1_address
		adc #$1e
		sta dac_address
		lda sid_1_address+1
		adc #0
		sta dac_address+1
	#if io_prtstatus
		jsr primm
		.text @"  DAC \$00"
		lda dac_address
		ldx dac_address+1
		jsr printaddr

		lda dac_model
		cmp #2
		bne !+
		jsr detect_sid.printnae
		jmp end
	!:	jsr primm
		.text @"DigiBlaster\$0d\$00"
	#endif
end:
	clc
	#endif
	rts
*/
}
#endif

// Detect SID address
//=======================================
//  In: XR: SID address low
//      YR: SID address high
// Out: AC: 0: not found $ff: found
//=======================================

detect_sid_address: {
	ldy #24
!:	sta (sidptr),y
	dey
	bpl !-
	
	ldy #25
	lda (sidptr),y
	iny
	and (sidptr),y
	cmp #$ff
	beq sidfound
// Either PotX or PotY is not $ff - no SID or paddle is not fully left - try alternative detection
// Reset SID (again)
	ldy #24
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
sidfound:
	clc
	.byte BIT_ZP
!:	sec
	#if io_prtstatus
		php
		bit debug_mode
		bpl !+
		pha
		jsr primm
		.text @"?SID: \$00"
		pla
		jsr hexbout
		jsr prtspace
		tya
		jsr hexbout
	!:
		plp
	#endif
	rts
}

// Detect SID chip type
// original version from Reflex Math demo
//=======================================
detect_sid_model: {

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
	ldy ystore: #0
	dey
	sty ystore
	bne loop2
	beq oldsid
newsid:
	clc
	.byte BIT_ZP
oldsid:
	sec
	#if io_prtstatus
		php
		bit debug_mode
		bpl !+
		pha
		jsr prtspace
		pla
		jsr hexbout
	!:
		plp
	#endif
	rts
}

//=======================================
// Detect NST SIDCard
//=======================================
#if	HAS_TED
detect_nae: {
	lda #%11100000
	sta nae_control+13
	ldx #0
!:	lda nae_control+15
	cmp nae_control+15
	bne notfound
	dex
	bne !-
	and #%11100000
	cmp #%11100000
	beq notfound	// Legacy SID-Card
	lda nae_control+15
//	beq !+		// VICE :)
	clc
	.byte BIT_ZP
notfound:
	sec
	#if io_prtstatus
		php
		bit debug_mode
		bpl !+
		pha
		jsr prtspace
		pla
		pha
		jsr hexbout
		pla
	!:	plp
	#endif
	bcc !+
	lda #0
!:	rts
}
#endif

#if io_prtstatus
	printaddr: {
		pha
//		lda #'@'
//		jsr chrout
		lda #'$'
		jsr chrout
		txa
		jsr hexbout
		pla
		jsr hexbout
		lda #':'
		jsr chrout
		jmp prtspace
	}
#endif

}
}
}
}