	#import "plus4_kernal_table.inc"

	* = $02 virtual
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
	.fill	32, 0
	.fill	32+16, 0
	.fill	16, 0
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

	ldx	#48
	lda	#$81
s_lp00:
	sta	sid_map+32-1,x
	dex
	bne	s_lp00

/*
	sei
	jsr	check_sid
	cli

	lda	#<map_msg
	ldy	#>map_msg
	jsr	$ab1e
	ldx	#0
	ldy	#$d4
s_lp1:
	txa
	and	#$7
	bne	s_skp1
	lda	#13
	jsr	$ffd2
	lda	#$20
	jsr	$ffd2
	tya
	jsr	print_hex
	lda	#0
	jsr	print_hex
	iny
s_skp1:
	lda	#$20
	jsr	$ffd2
	lda	sid_map,x
	bpl	s_skp2
	cmp	#$81
	bne	s_skp11
	lda	#"*"
	dc.b	$2c
s_skp11:
	lda	#"-"
	jsr	$ffd2
	jsr	$ffd2
	jmp	s_skp3
s_skp2:
	jsr	print_hex
s_skp3:
	inx
	cpx	#MAP_LEN
	bne	s_lp1

	lda	num_sids
	beq	s_ex1

	lda	#<list_msg
	ldy	#>list_msg
	jsr	$ab1e
	ldx	#0
s_lp2:
	stx	mcnt_zp
	lda	#$20
	jsr	$ffd2
	lda	sid_list_h,x
	jsr	print_hex
	lda	sid_list_l,x
	jsr	print_hex
	lda	#$20
	jsr	$ffd2
	lda	sid_list_t,x
	bne	s_skp4
	lda	#<newsid_msg
	ldy	#>newsid_msg
	jsr	$ab1e
	jmp	s_skp5
s_skp4:
	cmp	#$01
	bne	s_skp5
	lda	#<oldsid_msg
	ldy	#>oldsid_msg
	jsr	$ab1e
s_skp5:
	lda	#13
	jsr	$ffd2
	ldx	mcnt_zp
	inx
	cpx	num_sids
	bne	s_lp2
	
s_ex1:
	jmp	$a474

map_msg:
	dc.b	13,"SID MAP: ",0
list_msg:
	dc.b	13,13,"SID LIST: ",13,0
oldsid_msg:
	dc.b	"6581",0
newsid_msg:
	dc.b	"8580",0

;**************************************************************************
;*
;* NAME  print_hex
;*
;* DESCRIPTION
;*   output hex byte
;*
;******
print_hex:
	pha
	lsr
	lsr
	lsr
	lsr
	jsr	ph_skp1
	pla
	and	#$0f
ph_skp1:
	cmp	#$0a
	bcc	ph_skp2
; C = 1
	adc	#"A"-$0a-"0"-1
ph_skp2:
; C = 0
	adc	#"0"
	jmp	$ffd2
;	rts

	
;**************************************************************************
;*
;* NAME  check_sid
;*   
;* DESCRIPTION
;*   Check for SID model.
;*   
;******
check_sid:
	lda	#$00
	sta	num_sids
	sta	scnt_zp
	sta	sptr_zp
	lda	#$d4
	sta	sptr_zp+1
	lda	#$10
	sta	sidnum_zp

cs_lp1:
	ldx	#3
cs_lp11:
	lda	sptr_zp-1,x
	sta	mptr_zp-1,x
	dex
	bne	cs_lp11

	ldy	scnt_zp
	lda	sid_map,y
	bne	cs_next

	jsr	sid_print

	sta	sid_map,y
	bmi	cs_next
;******
;* found new unique sid!
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
	
;******
;* Scan for and mark mirrors
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
	
;******
;* Next sid
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

;**************************************************************************
;*
;* NAME  sid_print
;*   
;* DESCRIPTION
;*   Detect a sid by retriggering the sawtooth wave.
;*   We verify twice that three samples count up the correct way.
;*   
;******
sid_print:
	stx	x_zp
	sty	y_zp
; make sure we stay away from the screen area
sp_lp1:
	lda	$d012
	cmp	#45
	bcs	sp_lp1
;------
	inc	$d020
	lda	#$1b
	sta	tmp_zp
	ldx	#2
sp_lp2:
	ldy	#$12
	lda	#$7f
	sta	(sptr_zp),y	; reset phase using test bit
	ldy	#$0e
	lda	#$20
	sta	(sptr_zp),y
	iny
	sta	(sptr_zp),y	; freq=$2020
	ldy	#$12
	sta	(sptr_zp),y	; sawtooth
	ldy	tmp_zp		; 3
	lda	(mptr_zp),y	; 5  =8
	sta	buf_zp+0	; 3
	lda	(mptr_zp),y	; 5  =8
	sta	buf_zp+1	; 3
	lda	(mptr_zp),y	; 5  =8
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

	ldy	#$12
	lda	#$00
	sta	(sptr_zp),y	; disable oscillator

; Acc = first sample
	dec	$d020
	ldx	x_zp
	ldy	y_zp
	pla			; make sure flags are set up
	rts
	echo	.

*/

/*
num_sids:
	ds.b	1
sid_list_l:
	ds.b	8
sid_list_h:
	ds.b	8
sid_list_t:
	ds.b	8
	
; eof
*/