	#importonce
	.print	"ACIA detection"
	.encoding "petscii_mixed"

	.namespace iolib {
	.namespace detect {
	.namespace detect_acia {
	.memblock "ACIA detection"
	
	.label addressptr	= 2
#if io_detect_acia
	detect: {

		lda #<acia_base
		ldx #>acia_base
		sta addressptr
		stx addressptr+1
		jsr detect_acia_address
		bcs !+
		inc acia_type
		#if io_prtstatus
			jsr primm
			.text @" ACIA detected\$0d\$00"
		#endif
	!:	rts
	}
#endif

	detect_acia_address: {
		#if io_prtstatus
			bit debug_mode
			bpl !+
			jsr primm
			.text @"?ACIA: \$00"
			lda addressptr+1
			jsr hexbout
			lda addressptr
			jsr hexbout
			lda #':'
			jsr chrout
		!:
		#endif
		ldy #(acia.status - acia_base)
		// reset ACIA
		sta (addressptr),y
		// Configure ACIA for No Parity, No Echo, No TX Interrupt, /RTS Low (Unneeded), No RX Interrupt, /DTR Low (Unneeded)
		lda #$0B
		ldy #(acia.command-acia_base)
		sta (addressptr),y
		// 0 Stop Bits, 8 Data Bits, Internal Baud Rate Generator, 19200 bps (which will actually be 31250 due to 3 MHz xtal)
		lda #$1F
		ldy #(acia.control-acia_base)
		sta (addressptr),y

		ldy #(acia.command-acia_base)
		lda (addressptr),y
		#if io_prtstatus
			bit debug_mode
			bpl !+
			pha
			jsr hexbout
			jsr prtspace
			pla
		!:
		#endif
		cmp #$0B
		bne notfound

		ldy #(acia.control-acia_base)
		lda (addressptr),y
		#if io_prtstatus
			bit debug_mode
			bpl !+
			pha
			jsr hexbout
			pla
		!:
		#endif
		cmp #$1F
		bne notfound

		clc
		.byte BIT_ZP
	notfound:
		sec
		#if io_prtstatus
			bit debug_mode
			bpl !+
			php
			jsr prtcr
			plp
		!:
		#endif
		rts
	}
}
}
}
