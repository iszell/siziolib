	#import "plus4_kernal_table.inc"
	#import "ted.inc"

	* = $d0 virtual
.zp {
	
tmp_zp:
	.byte 0
y_zp:
	.byte 0
x_zp:
	.byte 0
buf_zp:
	.word 0

sidnum_zp:
	.byte 0
cnt1_zp:
	.byte 0
sptr_zp:
	.word 0
	.byte 0
.label scnt_zp = sptr_zp+2
mptr_zp:
	.word 0
	.byte 0
.label mcnt_zp = mptr_zp+2
res_zp:
	.byte 0
}

	#import "plus4_basic_header.inc"
	jmp start

sid_map:
/*
	.fill	32, 0
	.fill	32+16, 0
	.fill	16, 0
*/
.fill ($ff00-$fd00)/$20, 0
.const MAP_LEN	=	*-sid_map

start:
	jsr primm
	.text @"\$0dSID-DETECT2 / TLR\$0d\$00"

	ldx	#MAP_LEN
	lda	#0
s_lp0:
	sta	sid_map-1,x
	dex
	bne	s_lp0

	lda	#$81
	sta	sid_map+$00/2,x // do not check at $fd00 to prevent overwriting user port
	sta	sid_map+$02/2,x // do not check at $fd20 to prevent overwriting speech/digimuz/keyboard
	sta	sid_map+$0c/2,x // do not check at $fdc0 to prevent overwriting ROM bank
	sta	sid_map+$1c/2,x // do not check at $fec0 to prevent overwriting TCBM #9
	sta	sid_map+$1e/2,x // do not check at $fec0 to prevent overwriting TCBM #8

	sei
	jsr	check_sid
//	sta rom_bank_latch // fix potentially overwritten ROM config
	cli
	jsr	primm
	.text 	@"\$0dSID MAP: \$00"

	ldx	#0
//	ldy	#$d4
	ldy	#$fd
s_lp1:
	txa
	and	#$7
	bne	s_skp1
	lda	#13
	jsr	chrout
	lda	#' '
	jsr	chrout
	tya
	jsr hexbout
	lda	#0
	jsr	hexbout
	iny
s_skp1:
	lda	#' '
	jsr	chrout
	lda	sid_map,x
	bpl	s_skp2
	cmp	#$81
	bne	s_skp11
	lda	#'*'
	.byte BIT_ABS
s_skp11:
	lda	#'-'
	jsr	chrout
	jsr	chrout
	jmp	s_skp3
s_skp2:
	jsr	hexbout
s_skp3:
	inx
	cpx	#MAP_LEN
	bne	s_lp1

	lda	num_sids
	beq	s_ex1

	jsr	primm
	.text @"\$0d\$0dSID LIST: \$0d\$00"

	ldx	#0
s_lp2:
	stx	mcnt_zp
	lda	#' '
	jsr	chrout
	lda	sid_list_h,x
	jsr	hexbout
	lda	sid_list_l,x
	jsr hexbout
	lda	#' '
	jsr	chrout
	lda	sid_list_t,x
	bne	s_skp4

	jsr	primm
	.text "8580"
	.byte 0

	jmp	s_skp5
s_skp4:
	cmp	#$01
	bne	s_skp5
	jsr	primm
	.text "6581"
	.byte 0
s_skp5:
	lda	#13
	jsr	chrout
	ldx	mcnt_zp
	inx
	cpx	num_sids
	bne	s_lp2
	
s_ex1:
	rts

check_sid:
	lda	#0
	sta	num_sids
	sta	scnt_zp
	sta	sptr_zp
//	lda	#$d4
	lda	#$fd
	sta	sptr_zp+1
	lda	#$10
	sta	sidnum_zp

cs_lp1:
	ldx	#3
cs_lp11:
	lda	sptr_zp,x
	sta	mptr_zp,x
	dex
	bpl	cs_lp11

	ldy	scnt_zp
	lda	sid_map,y
	bne	cs_next

	jsr	sid_print

	sta	sid_map,y
	bmi	cs_next

// found new unique sid!
	pha
	ora	sidnum_zp
	sta	sid_map,y

	ldx	num_sids
	lda	sptr_zp
	sta	sid_list_l,x
	lda	sptr_zp+1
	sta	sid_list_h,x
	pla
	sta	sid_list_t,x
	inc	num_sids
	
// Scan for and mark mirrors
cs_lp2:
	ldx	#mptr_zp
	jsr	add_sid

	lda	sid_map,y
	bne	cs_skp1
	
	jsr	sid_print
	bmi	cs_skp1
	
	ora	sidnum_zp
	sta	sid_map,y

cs_skp1:
	cpy	#MAP_LEN
	bne	cs_lp2

	lda	sidnum_zp
	clc
	adc	#$10
	sta	sidnum_zp
	
// Next sid
cs_next:
	ldx	#sptr_zp
	jsr	add_sid
	cpy	#MAP_LEN
	bne	cs_lp1
	rts

add_sid:
	clc
	lda	$00,x
	adc	#$20
	sta	$00,x
	bcc	as_ex1
	inc	$01,x
as_ex1:
	inc	$02,x
	ldy	$02,x
	rts

/*
;**************************************************************************
;*
;* NAME  sid_print
;*   
;* DESCRIPTION
;*   Detect a sid by retriggering the sawtooth wave.
;*   We verify twice that three samples count up the correct way.
;*   
;******
*/
sid_print:
	stx	x_zp
	sty	y_zp
// switch to single clock
	lda ted.charsetaddr
	ora #%10
	sta ted.charsetaddr
// make sure we stay away from the screen area
sp_lp1:
	lda	ted.rasterlinehi
	and	#%1
	beq	sp_lp1

.byte $f2
	inc	ted.border
	lda	#$1b
	sta	tmp_zp
	ldx	#2
sp_lp2:
	ldy	#18
	lda	#$7f
	sta	(sptr_zp),y	// reset phase using test bit
	ldy	#14
	lda	#$20
	sta	(sptr_zp),y
	iny
	sta	(sptr_zp),y	// freq=$2020
	ldy	#18
	sta	(sptr_zp),y	// sawtooth
	ldy	tmp_zp		// 3
	lda	(mptr_zp),y	// 5  =8
	sta	buf_zp+0	// 3
	lda	(mptr_zp),y	// 5  =8
	sta	buf_zp+1	// 3
	lda	(mptr_zp),y	// 5  =8
	tay
	lda	#$80
	dey
	cpy	buf_zp+1
	bne	sp_fl1
	dey
	cpy	buf_zp+0
	bne	sp_fl1
	dex
	bne	sp_lp2

	tya
sp_fl1:
	pha

	ldy	#18
	lda	#$00
	sta	(sptr_zp),y	// disable oscillator

// Acc = first sample
	dec	ted.border
// switch back to double clock
	lda ted.charsetaddr
	and #~%10
	sta ted.charsetaddr
	ldx	x_zp
	ldy	y_zp
	pla			// make sure flags are set up
	rts

num_sids:
	.byte 0
sid_list_l:
	.fill 8, 0
sid_list_h:
	.fill 8, 0
sid_list_t:
	.fill 8, 0
