	.namespace iolib {

init:
{
.label	ptr	=	2

#if	need_loader
	.print	". loader init"
	jsr	initloader
#endif
	rts

#if	need_loader
initloader:
	lda	drivedetect.io_driveiftyp
	asl
	asl
	asl
	asl
	ora	drivedetect.io_drivetyp
	asl
	tax
	stx	xtmp
	lda	io_loadertab,x
	bne	!+
	jmp	io_noloader
!:	pha
	#if	prtstatus
	jsr	primm
	.text	@"Initializing I/O library\$0d\$00"
	#endif	
	ldx	xtmp
	inx
	lda	io_loadertab,x
	#if	prtstatus
	pha
	jsr	primm
	.text	@" Drive loader: \$00"
	asl
	asl
	asl
	asl
	tay
	ldx	#16
!:	lda	io_drvloadernames,y
	jsr	chrout
	iny
	dex
	bne	!-
	jsr	prtcr
	pla
	#endif
	cmp #8
	bne !+
	jmp	nodrivesidecode
!:	asl
	asl
	tax
	// loader start address
	lda	io_driveloaders,x
	sta	ptr
	lda	io_driveloaders+1,x
	sta	ptr+1
	lda	io_driveloaders+2,x
	// loader end address
	sta	ptr+2
	lda	io_driveloaders+3,x
	sta	ptr+3
	// loader drive side address
	lda	#<$0500
	sta	ptr+4
	lda	#>$0500
	sta	ptr+5
	#if	prtstatus
	jsr	primm
	.text	@"  Downloading drive code... \$00"
	#endif
// Download drive code
	ldx	$ae
	lda	#15
	tay
	jsr	setlfs
	lda	#0
	jsr	setnam
	jsr	open
	lda	st
	beq	!+
	jmp	io_noloader
!:
wrtnxt:	ldx	#15
	jsr	chkout
	lda	st
	beq	!+
	jmp	io_noloader
!:	lda	#'m'
	jsr	sendbyte
	lda	#'-'
	jsr	sendbyte
	lda	#'w'
	jsr	sendbyte
	lda	ptr+4
	jsr	sendbyte
	lda	ptr+5
	jsr	sendbyte
	lda	#16
	jsr	sendbyte
	ldy	#0
!:	lda	(ptr),y
	jsr	sendbyte
	iny
	cpy	#16
	bne	!-
	jsr	clrchans
	tya
	clc
	adc	ptr+4
	sta	ptr+4
	bcc	!+
	inc	ptr+5
!:	tya
	clc
	adc	ptr
	sta	ptr
	bcc	!+
	inc	ptr+1
!:	lda	ptr+1
	cmp	ptr+3
	bcc	wrtnxt
	bne	!+
	lda	ptr
	cmp	ptr+2
	bcc	wrtnxt
!:
	#if	prtstatus
	jsr	primm
	.text	@"done\$0d"
	.text	@"  Starting drive code\$0d\$00" 
	#endif
	ldx	#15
	jsr	chkout
	lda	#'m'
	jsr	sendbyte
	lda	#'-'
	jsr	sendbyte
	lda	#'e'
	jsr	sendbyte
	lda	#<$0500
	jsr	sendbyte
	lda	#>$0500
	jsr	sendbyte
	jsr	clrchans
	lda	#15
	jsr	close
nodrivesidecode:
	#if	prtstatus
	jsr	primm
	.text	@" Computer loader: \$00"
	pla
	pha
	asl
	asl
	asl
	asl
	tay
	ldx	#16
!:	lda	io_cmploadernames,y
	jsr	chrout
	iny
	dex
	bne	!-
	#endif
.if (io_base < $0800) {
	sei
}
	pla
	#if	need_exodecrunch
	pha
	#endif
	asl
	tax
	lda	io_plus4loaders,x
	sta	ptr
	lda	io_plus4loaders+1,x
	sta	ptr+1
// Copy computer loader code
	ldy	#0
!:	lda	(ptr),y
	sta	io_base,y
	iny
	bne	!-
.if (io_base > $8000) {
	sei
	sta	ted.ramen
}
	jsr	io_initloader
.if (io_base > $8000) {
	sta	ted.romen
	cli
}
	#if	need_exodecrunch
	#if	prtstatus
	jsr	primm
	.text	@"\$0d Exomizer decruncher\$00"
	#endif
	pla
	cmp #5
	beq c16stmdecrunch

	ldx	#0
!:	lda	io_decrunchdata,x
	sta	iolib.decrunch,x
	inx
	bne	!-
	ldx	#io_decrunchdatasize-$100-1
!:	lda	io_decrunchdata+$100,x
	sta	iolib.decrunch+$100,x
	dex
	bpl	!-
	bmi decruncend
c16stmdecrunch:
	ldx	#0
!:	lda	io_decrunchdata_c16stm,x
	sta	iolib.decrunch,x
	inx
	bne	!-
	ldx	#io_decrunchdatasize_c16stm-$100-1
!:	lda	io_decrunchdata_c16stm+$100,x
	sta	iolib.decrunch+$100,x
	dex
	bpl	!-
decruncend:
	#endif
	#if	prtstatus
	jsr	primm
	.text	@"\$0dInitialization complete\$0d\$00" 
	#endif
	clc
	rts
io_noloader:
	#if	prtstatus
	jsr	primm
	.text	@"\$0dError: no loader for drive!\$0d\$00" 
	#endif
	sec
	rts
	
sendbyte:
	jsr	chrout
	lda	st
	beq	!+
	pla
	pla
	jmp	io_noloader
!:	rts

xtmp:	.byte	0

// Index: bit 7-5: interface
//        bit 4-1: drive type
//        bit   0: 0: computer, 1: drive
// Value:	byte 0: computer loader
//			byte 1: drive loader
io_loadertab:
	// unsupported interface
	.byte	0, 0 // 0
	.byte	0, 0 // 1
	.byte	0, 0 // 2
	.byte	0, 0 // 3
	.byte	0, 0 // 4
	.byte	0, 0 // 5
	.byte	0, 0 // 6
	.byte	0, 0 // 7
	.byte	0, 0 // 8
	.byte	0, 0 // 9
	.byte	0, 0 // a
	.byte	0, 0 // b
	.byte	0, 0 // c
	.byte	0, 0 // d
	.byte	0, 0 // e
	.byte	0, 0 // f
	// serial with single drive on the bus
	.byte	0, 0 // 0 unknown
	.byte	2, 2 // 1 1540
	.byte	2, 2 // 2 1541
	.byte	2, 2 // 3 1541C
	.byte	2, 2 // 4 1541-II
	.byte	0, 0 // 5 1551
	.byte	2, 2 // 6 1570
	.byte	2, 2 // 7 1571
	.byte	2, 4 // 8 1581
	.byte	2, 2 // 9 RF-501C
	.byte	2, 7 // a SD2IEC
	.byte	2, 7 // b SD2IEC/FlexSD
	.byte   0, 0 // c FD-2000
	.byte	0, 0 // d
	.byte	0, 0 // e
	.byte	0, 0 // f
//	.byte	0, 0 // 0 unknown
//	.byte	1, 1 // 1 1540
//	.byte	1, 1 // 2 1541
//	.byte	1, 1 // 3 1541C
//	.byte	1, 1 // 4 1541-II
//	.byte	0, 0 // 5 1551
//	.byte	1, 1 // 6 1570
//	.byte	1, 1 // 7 1571
//	.byte	1, 3 // 8 1581
//	.byte	1, 1 // 9 RF-501C
//	.byte	2, 7 // a SD2IEC
//	.byte	0, 0 // b
//	.byte	0, 0 // c
//	.byte	0, 0 // d
//	.byte	0, 0 // e
//	.byte	0, 0 // f
	// serial with multiple drives on the bus
	.byte	0, 0 // 0 unknown
	.byte	2, 2 // 1 1540
	.byte	2, 2 // 2 1541
	.byte	2, 2 // 3 1541C
	.byte	2, 2 // 4 1541-II
	.byte	0, 0 // 5 1551
	.byte	2, 2 // 6 1570
	.byte	2, 2 // 7 1571
	.byte	2, 4 // 8 1581
	.byte	2, 2 // 9 RF-501C
	.byte	2, 7 // a SD2IEC
	.byte	2, 7 // b SD2IEC/FlexSD
	.byte	0, 0 // c FD-2000
	.byte	0, 0 // d
	.byte	0, 0 // e
	.byte	0, 0 // f
	// TCBM
	.byte	0, 0 // 0 unknown
	.byte	0, 0 // 1 1540
	.byte	0, 0 // 2 1541
	.byte	0, 0 // 3 1541C
	.byte	0, 0 // 4 1541-II
	.byte	4, 6 // 5 1551
	.byte	0, 0 // 6 1570
	.byte	0, 0 // 7 1571
	.byte	0, 0 // 8 1581
	.byte	0, 0 // 9 RF-501C
	.byte	0, 0 // a SD2IEC
	.byte	0, 0 // b 
	.byte	0, 0 // c
	.byte	0, 0 // d
	.byte	0, 0 // e
	.byte	0, 0 // f
	// parallel
	.byte	0, 0 // 0 unknown
	.byte	0, 0 // 1 1540
	.byte	3, 5 // 2 1541
	.byte	3, 5 // 3 1541C
	.byte	3, 5 // 4 1541-II
	.byte	0, 0 // 5 1551
	.byte	0, 0 // 6 1570
	.byte	0, 0 // 7 1571
	.byte	0, 0 // 8 1581
	.byte	0, 0 // 9 RF-501C
	.byte	0, 0 // a SD2IEC
	.byte	0, 0 // b SD2IEC/FlexSD
	.byte	0, 0 // c FD-2000
	.byte	0, 0 // d
	.byte	0, 0 // e
	.byte	0, 0 // f
	// TCBM standard
	.byte	0, 0 // 0
	.byte	0, 0 // 1
	.byte	0, 0 // 2
	.byte	0, 0 // 3
	.byte	0, 0 // 4
	.byte	0, 0 // 5
	.byte	0, 0 // 6
	.byte	0, 0 // 7
	.byte	0, 0 // 8
	.byte	0, 0 // 9
	.byte	0, 0 // a
	.byte	0, 0 // b
	.byte	0, 0 // c
	.byte	0, 0 // d
	.byte	6, 8 // e
	.byte	6, 8 // f
	// IEEE-488
	.byte	0, 0 // 0
	.byte	0, 0 // 1
	.byte	0, 0 // 2
	.byte	0, 0 // 3
	.byte	0, 0 // 4
	.byte	0, 0 // 5
	.byte	0, 0 // 6
	.byte	0, 0 // 7
	.byte	0, 0 // 8
	.byte	0, 0 // 9
	.byte	0, 0 // a
	.byte	0, 0 // b
	.byte	0, 0 // c
	.byte	0, 0 // d
	.byte	0, 0 // e
	.byte	0, 0 // f
	// C16STM
	.byte	0, 0 // 0
	.byte	0, 0 // 1
	.byte	0, 0 // 2
	.byte	0, 0 // 3
	.byte	0, 0 // 4
	.byte	0, 0 // 5
	.byte	0, 0 // 6
	.byte	0, 0 // 7
	.byte	0, 0 // 8
	.byte	0, 0 // 9
	.byte	0, 0 // a
	.byte	0, 0 // b
	.byte	0, 0 // c
	.byte	5, 8 // d
	.byte	0, 0 // e
	.byte	0, 0 // f

	#if	prtstatus
io_cmploadernames:
		//0123456789abcdef
	.text	"unsupported     " // 0
	.text	"serial 2bit     " // 1
	.text	"serial 1bit     " // 2
	.text	"parallel        " // 3
	.text	"TCBM            " // 4
	.text	"C16STM          " // 5
	.text	"TCBM standard   " // 6
io_drvloadernames:
		//0123456789abcdef
	.text	"unsupported     " // 0
	.text	"1541 serial 2bit" // 1
	.text	"1541 serial 1bit" // 2
	.text	"1581 serial 2bit" // 3
	.text	"1581 serial 1bit" // 4
	.text	"1541 parallel   " // 5
	.text	"1551 TCBM       " // 6
 	.text	"SD2IEC G.I. Joe " // 7
 	.text	"n/a             " // 8
	#endif

// contains the start address of the selected loader with the end address on the next word
io_plus4loaders:
	.word	0
	.word	io_plus4serial2bit
	.word	io_plus4serial1bit
	.word	io_plus4parallel
	.word	io_plus4tcbm
	.word	io_plus4c16stm
	.word	io_plus4tcbm_standard
// contains the start address and end addresses of the selected loader
io_driveloaders:
	.word	0, 0
	.word	io_drv1541ser2, io_drv1541ser1
	.word	io_drv1541ser1, io_drv1581ser2
	.word	io_drv1581ser2, io_drv1581ser1
	.word	io_drv1581ser1, io_drv1541par
	.word	io_drv1541par, io_drvtcbm
	.word	io_drvtcbm, io_drvgijoe
	.word	io_drvgijoe, io_loaderend

io_plus4serial2bit:
//	#import	"loader_plus4_serial2bit.inc"
io_plus4serial1bit:
	.import c64 "../loadercode/serial_1bit.prg"
io_plus4parallel:
	.import c64 "../loadercode/parallel.prg"
io_plus4tcbm:
	.import	c64 "../loadercode/tcbm.prg"
io_plus4tcbm_standard:
	.import c64 "../loadercode/tcbm_standard.prg"
io_plus4c16stm:
	.import c64 "../loadercode/c16stm.prg"
io_drv1541ser2:
//	#import	"loader_drive_2bit1541.inc"
io_drv1541ser1:
	.import c64 "../drivecode/serial_1bit_1541.prg"
io_drv1581ser2:
io_drv1581ser1:
	.import c64 "../drivecode/serial_1bit_1581.prg"
io_drv1541par:
	.import c64 "../drivecode/parallel_1541.prg"
io_drvtcbm:
	.import c64 "../drivecode/tcbm_1551.prg"
io_drvgijoe:
	.import c64 "../drivecode/serial_1bit_gijoe.prg"
io_loaderend:
#endif
#if need_exodecrunch
io_decrunchdata:
	.import c64 "../depacker/decrunch.prg"
.label io_decrunchdatasize=*-io_decrunchdata
io_decrunchdata_c16stm:
	.import c64 "../depacker/decrunch_c16stm.prg"
.label io_decrunchdatasize_c16stm=*-io_decrunchdata_c16stm

#endif
}
}
