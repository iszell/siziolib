	#importonce
	.print	"Siz IOLib hardware detection code"
	.encoding "petscii_mixed"

	.namespace iolib {
	.namespace detect {
detected_hardware:
	.memblock "Detected hardware data"
#if io_detect_memory_size
	// 0: Stock
	// 1: Hannes
	// 2: Csory
	// 3: NeoRAM
	memory_expansion_type:
		.byte 0
	// in kiloBytes
	memory_size:
		.word 0
#endif
#if io_detect_sound
	// 0: none
	// 1: C64/C128 standard
	// 2: legacy SIDCard/ReSeed
	// 3: NAE/CAE
	sid_card_type:
		.byte 0
	sid_card_version:
		.byte 0
	sid_1_model:
		.byte 0
	sid_1_address:
		.word 0
/*
	sid_2_model:
		.byte 0
	sid_2_address:
		.word 0
*/
#if HAS_AY
	// 0: none
	// 1: AY-3-891x
	// 2: YM2149
	ay_model:
		.byte 0
	ay_address:
		.word 0
#endif
#if HAS_FM
	// 0: none
	// 1: Commodore Sound Expander
	// 2: SideKick
	// 3: SoundX
	fm_card_type:
		.byte 0
	// 0: none
	// 1: OPL/OPL-II
	fm_model:
		.byte 0
	fm_address:
		.word 0
#endif
#if HAS_DAC
/*
	// 0: none
	// 1: DigiBlaster
	// 2: NAE/CAE
	dac_card_type:
		.byte 0
	dac_card_version:
		.byte 0
	dac_model:
		.byte 0
	dac_address:
		.word 0
*/
#endif
#endif
#if io_detect_drive
	drive_number:
		.byte 0
	drive_model:
		.byte 0
	drive_version:
		.byte 0
	drive_interface_type:
		.byte 0
	drive_serial_drives:
		.byte 0
#endif
#if io_detect_cpu_port
	cpu_port_type:
		.byte 0
#endif
.const detected_hardware_area_size = * - detected_hardware
.print ". Detected hardware data allocates " + detected_hardware_area_size + " bytes"

.memblock "Hardware detection main code"
#if io_prtstatus
	debug_mode:
		.byte 0
#endif
detect: {
#if io_prtstatus
	jsr primm
	.text @"\$08\$0eDetecting hardware:\$0d\$00"
#endif
	jsr init

#if io_detect_video
	jsr detect_video_standard
#endif
#if io_detect_memory_size
	jsr detect_memory_size.detect
#endif
#if io_detect_sound
	jsr detect_sound.detect_sound
#endif
#if io_detect_drive
	jsr detect_drive.detect
#endif
#if io_detect_cpu_port
	jsr detect_cpu_port_type
#endif
#if io_detect_emulator
	jsr detect_emulator
#endif
#if io_prtstatus
	.if(detected_hardware_area_size > 0) {
		bit debug_mode
		bmi !+
		rts
	!:	ldx #0
	!:	lda detected_hardware,x
		jsr hexbout
		jsr prtspace
		jsr prtspace
		inx
		cpx #detected_hardware_area_size
		bne !-
	}
#endif
	rts
}

init: {
	.if( detected_hardware_area_size > 0 ) {
		ldx #detected_hardware_area_size-1
		lda #0
	!:	sta detected_hardware,x
		dex
		bpl !-
	}
	#if io_prtstatus
		ldx #0
	!:	lda $0200,x
		cmp #':'
		beq colon
		inx
		cpx #32
		bne !-
		beq nodebug
	colon:
		inx
		lda $0200,x
		cmp #'d'
		bne nodebug
		jsr primm
		.text @"?Debug output enabled!\$0d\$00"
		lda #-1
		.byte BIT_ABS
	nodebug:
		lda #0
		sta debug_mode
	#endif
	rts
}

#if io_detect_video
	.print ". Video standard detection"
	.memblock "Video standard detection"
	detect_video_standard: {
	#if HAS_TED
			#if io_prtstatus
				jsr primm
				.text @" Video standard: \$00"
			lda ted.ctrl2
			and #%01000000
			lsr
			lsr
			lsr
			tax
			ldy #4
		!:	lda video_standard_name,x
			jsr chrout
			inx
			dey
			bne !-
			jsr prtcr
			#endif
	#endif
	rts
	#if io_prtstatus
		video_standard_name:
			.text "PAL NTSC"
	#endif
	}
#endif

#if io_detect_cpu_port
	.print ". CPU I/O port type detection"
	.memblock "CPU I/O port type detection"
	detect_cpu_port_type: {
		#if HAS_TED
			lda cpu_port
			pha
			#if	io_prtstatus
				jsr	primm
				.text	@" CPU port type: \$00"
			#endif
			lda cpu_port
			and #~%00000001
			sta cpu_port
			bit cpu_port
			bpl not7501
			ora #%00000001
			sta cpu_port
			bit cpu_port
			bmi not7501
			and #%00100000
			bne not7501
			#if io_prtstatus
				jsr	primm
				.text	@"7501/8501\$00"
			#endif
			lda cpu_dir
			ora #%00100000
			sta cpu_dir
			lda cpu_dir
			and #%00100000
			beq not6502plus4
			#if io_prtstatus
				jsr	primm
				.text	@" compatible\$00"
			#endif
		not6502plus4:
			jmp cpuend
		not7501:
			and #%11000000
			bne not6502
			lda cpu_port
			and #~%00000011
			sta cpu_port
			lda cpu_port
			and #%00100000
			beq not6502
			#if io_prtstatus
				jsr	primm
				.text	@"6510/8500\$00"
			#endif
			lda #1
			sta cpu_port_type
			bne incompatbile
		not6502:
			#if io_prtstatus
			jsr	primm
			.text	@"Unsupported\$00"
			#endif
			lda #2
			sta cpu_port_type
		incompatbile:
		cpuend:
			#if io_prtstatus
			jsr prtcr
			#endif
			pla
			sta cpu_port
		#endif
		rts
	}
#endif

#if io_detect_emulator
	.print ". Emulator detection"
	.memblock "Emulator detection"
	detect_emulator: {
		#if HAS_TED
			lda $fdd0
			ldx #0
		!:  ora $fdd0
			bne notvice
			dex
			bne !-
			#if io_prtstatus
				jsr	primm
				.text	@" VICE xplus4 detected\$00"
			#endif
			#if io_halt_on_vice
				#if io_prtstatus
					jsr	primm
					.text	@". Stopping\$00"
				#endif
				lda #$56
				sta ted.border
				.byte $02
			#endif
			#if io_prtstatus
				jmp prtcr
			#endif
		notvice:
		#endif
		rts
	}
#endif

#if io_prtstatus
prtspace: {
	lda #' '
	jmp chrout
}

prtcr: {
	lda #13
	jmp chrout
}
#endif

#if io_detect_memory_size
	#import "detect_memory.inc"
#endif
#if io_detect_sound
	#import "detect_sound.inc"
#endif
#if io_detect_drive
	#import "detect_drive.inc"
#endif
}
}
