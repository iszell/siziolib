	.print	". memory size detection"
	.namespace iolib {

detect_memsize:
{
#if !HAS_TED
	rts
#else
.label	testaddr	=	$fcff
	sei
	ldy	#$ff
// Reset Csory expansion control
	sty	ramexpctrl_csory
// Reset Hannes expansion control
	sty	ramexpctrl_hannes
	iny
	sty	io_ramsize
	sty	ted.ramen

// Save and Reset detection address contents
	lda	testaddr
	pha
	sty	testaddr
	lda	testaddr & $7fff
	pha
	sty	testaddr & $7fff
	lda	testaddr & $3fff
	pha
	sty	testaddr & $3fff

// Set test byte at 64k RAM end
	lda	#$55
	sta	testaddr
// Detect 16k
	cmp	testaddr & $3fff
	beq	!+
	iny
// Detect 32k
	cmp	testaddr & $7fff
	beq	!+
// We have at least 64k	
	iny
!:	sty	io_ramsize
	cpy	#2
	bne	!+
	jsr	io_detmem_hannes
	jsr	io_detmem_csory
!:	jsr	io_detmem_neoram
	sty	io_detmemtemp
// Restore detection address contents
	pla
	sta	testaddr & $3fff
	pla
	sta	testaddr & $7fff
	pla
	sta	testaddr
	sta	ted.romen
	cli

	#if	prtstatus
	jsr	primm
	.text	" RAM: "
	.byte	0
	sei
	sta	ted.ramen
	lda	io_ramsize
	sta	ted.romen
	pha
	cli
	and	#%00011111
	asl
	tay
	ldx	io_ramsizes,y
	lda	io_ramsizes+1,y
	jsr	lnprt
	lda	#'k'
	jsr	chrout
	pla
	pha
	bpl	!+
	jsr	primm
	.text	" Hannes"
	.byte	0
!:	pla
	pha
	and	#%01000000
	beq	!+
	jsr	primm
	.text	" Csory"
	.byte	0
!:	pla
	and	#%00100000
	beq	!+
	jsr	prtspc
	lda	io_detmemtemp
	asl
	tay
	ldx	io_ramsizes,y
	lda	io_ramsizes+1,y
	jsr	lnprt
	jsr	primm
	.text	"k NeoRAM"
	.byte	0
!:	jsr	prtcr
	#endif
	rts
	
// Detect Hannes expansion
// -----------------------
// Method: 
// 1) set a test pattern in highest possible memory bank 
// 2) test from lowest bank, increasing used address bits one by one
// 3) we are at highest bank present when we found our test pattern
// Attention! Address bits are inverted (control register defaults to $ff)  
 
io_detmem_hannes:
{
	lda	#0
// Reset address contents in all Hannes memory banks
// 4M
	ldx	#%11000000
	stx	ramexpctrl_hannes
	sta	testaddr
// 2M
	ldx	#%11100000 // this one turns on Csory expansion!
	stx	ramexpctrl_hannes
	sta	testaddr
// 1M
	ldx	#%11110000
	stx	ramexpctrl_hannes
	sta	testaddr
// 512k
	ldx	#%11111000
	stx	ramexpctrl_hannes
	sta	testaddr
// 256k
	ldx	#%11111100
	stx	ramexpctrl_hannes
	sta	testaddr
// 128k
	ldx	#%11111110
	stx	ramexpctrl_hannes
	sta	testaddr
// 64k
	ldx	#%11111111
	stx	ramexpctrl_hannes
	sta	testaddr

// Set test byte in highest bank
	ldx	#%11000000
	stx	ramexpctrl_hannes
	lda	#$aa
	sta	testaddr
// Start search from first (default) bank
	ldx	#%11111111
	stx	ramexpctrl_hannes
	
	ldx	#6	// Number of available memory configurations (address bits)
!:	cmp	testaddr
	beq	!+
	asl	ramexpctrl_hannes
	iny
	dex
	bne	!-
!:	ldx	#%11111111
	stx	ramexpctrl_hannes
	cpy	#3
	bcc	!+
	tya
	ora	#%10000000
	sta	io_ramsize
!:	rts	
}

// Detect Csory expansion
// -----------------------
// Method: 
// 1) 
io_detmem_csory: {
	// Set Csory expansion to default config: $c000-$ffff= 16k bank 3
	ldx	#%11000011
	stx	ramexpctrl_csory
	// Turn on Csory expansion on Hannes
	lda	#%11011111
	sta	ramexpctrl_hannes

	ldx	#%11000011
	stx	ramexpctrl_csory
	lda	#$55
	sta	testaddr

	ldx	#%11001111
	stx	ramexpctrl_csory
	sta	testaddr

	ldx	#%11111111
	stx	ramexpctrl_csory
	sta	testaddr

	ldx	#%11000011
	stx	ramexpctrl_csory
	lda	#$aa
	sta	testaddr

	ldy	#2
	ldx	#%11001111
	stx	ramexpctrl_csory
	cmp	testaddr
	beq	!+
	iny

	ldx	#%11111111
	stx	ramexpctrl_csory
	cmp	testaddr
	beq	!+
	iny

!:	sty	io_detmemtemp
	cpy	#3
	bcc	!+
	lda	#%01000000
	ora	io_ramsize
	sta	io_ramsize
	and	#%00111111
	cmp	io_detmemtemp
	bcs	!+
	lda	io_ramsize
	and	#%11000000
	ora	io_detmemtemp
	sta	io_ramsize
	// Turn off Csory and reset Hannes expansion
!:	lda	#%11111111
	sta	ramexpctrl_hannes
	rts
}

io_detmem_neoram: {
	ldx	#0
	stx	neoram.window
	stx	neoram.block
	stx	neoram.page
	lda	#$55
	sta	neoram.ram

	ldx	#32
	sta	neoram.ram
	ldx	#64
	stx	neoram.window
	sta	neoram.ram
	ldx	#128
	stx	neoram.window
	sta	neoram.ram

	ldx	#0
	ldy	#0
	stx	neoram.window
	cmp	neoram.ram
	bne	!+
	lda	#$aa
	ldy	#5
	sta	neoram.ram
	ldx	#32
	sta	neoram.window
	cmp	neoram.ram
	beq	!+
	iny
	ldx	#64
	sta	neoram.window
	cmp	neoram.ram
	beq	!+
	iny
	ldx	#128
	sta	neoram.window
	cmp	neoram.ram
	beq	!+
	iny
!:	tya
	beq	!+
	lda	#%00100000
	ora	io_ramsize
	sta	io_ramsize
!:	rts
}

io_detmemtemp:
	.byte	0
	#if	prtstatus
io_ramsizes:
	.word 16, 32, 64, 128, 256, 512, 1024, 2048, 4096
	#endif
#endif
}
}
