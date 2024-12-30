	.namespace iolib {
	.namespace loader {
	
	.memblock "Loader initialization"

	.enum { CL_SERIAL_1_BIT, CL_PARALLEL, CL_TCBM, CL_C16STM, CL_TCBM_STANDARD }
	.enum { DL_SERIAL_1_BIT_1541, DL_SERIAL_1_BIT_1581, DL_PARALLEL_1541, DL_TCBM_1551, DL_GIJOE, DL_NA }

init:
{
.label	ptr	=	2

#if	need_loader
	.print	". loader init"
	ldx #0
!:	lda	io_loadertab,x
	beq noloader
	cmp iolib.detect.drive_model
	bne notfound
	lda io_loadertab+1,x
	cmp iolib.detect.drive_interface_type
	beq !+
notfound:
	inx
	inx
	inx
	inx
	bne	!-
noloader:
	jmp	io_noloader
!:	stx xtmp
	#if	io_prtstatus
		jsr	primm
		.text	@"Initializing I/O library\$0d\$00"
		ldx	xtmp
	#endif	
	lda	io_loadertab+3,x
	#if	io_prtstatus
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
		jsr	iolib.detect.prtcr
		pla
	#endif
	cmp #DL_NA
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
	#if	io_prtstatus
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
	lda	st
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
	#if	io_prtstatus
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
	#if	io_prtstatus
		jsr	primm
		.text	@" Computer loader: \$00"
		ldx xtmp
		lda io_loadertab+2,x
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
	ldx xtmp
	lda io_loadertab+1,x
	cmp #TCBM
	bne nottcbm
	ldx #$30
	lda iolib.detect.drive_number
	cmp #8
	beq !+
	ldx #$00
!:	stx io_tcbmoffs
nottcbm:
	ldx xtmp
	lda io_loadertab+2,x
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
		#if	io_prtstatus
			jsr	primm
			.text	@"\$0d Exomizer decruncher\$00"
		#endif
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
	#endif
	#if	io_prtstatus
		jsr	primm
		.text	@"\$0dInitialization complete\$0d\$00" 
	#endif
	clc
	rts
io_noloader:
	#if	io_prtstatus
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

	.memblock "Loader definitions"
io_loadertab:
	.byte	C1540,		SERIAL,			CL_SERIAL_1_BIT,	DL_SERIAL_1_BIT_1541
	.byte	C1541,		SERIAL,			CL_SERIAL_1_BIT,	DL_SERIAL_1_BIT_1541
	.byte	C1541,		PARALLEL,		CL_PARALLEL,		DL_PARALLEL_1541
	.byte	C1541C,		SERIAL,			CL_SERIAL_1_BIT,	DL_SERIAL_1_BIT_1541
	.byte	C1541C,		PARALLEL,		CL_PARALLEL,		DL_PARALLEL_1541
	.byte	C1541II,	SERIAL,			CL_SERIAL_1_BIT,	DL_SERIAL_1_BIT_1541
	.byte	C1541II,	PARALLEL,		CL_PARALLEL,		DL_PARALLEL_1541
	.byte	C1551,		TCBM,			CL_TCBM,			DL_TCBM_1551
	.byte	C1570,		SERIAL,			CL_SERIAL_1_BIT,	DL_SERIAL_1_BIT_1541
	.byte	C1571,		SERIAL,			CL_SERIAL_1_BIT,	DL_SERIAL_1_BIT_1541
	.byte	C1581,		SERIAL,			CL_SERIAL_1_BIT,	DL_SERIAL_1_BIT_1581
	.byte	RF501C,		SERIAL,			CL_SERIAL_1_BIT,	DL_SERIAL_1_BIT_1541
	.byte	SD2IEC,		SERIAL,			CL_SERIAL_1_BIT,	DL_GIJOE
	.byte	FLEXSD,		SERIAL,			CL_SERIAL_1_BIT,	DL_GIJOE
	.byte	C16STM,		EXPANSIONPORT,	CL_C16STM,			DL_NA
	.byte	YAPEIEC,	TCBM,			CL_TCBM_STANDARD,	DL_NA
	.byte	TCBM2SD,	TCBM,			CL_TCBM_STANDARD,	DL_NA
	.byte	0

	#if	io_prtstatus
io_cmploadernames:
		//   0123456789abcdef
	.text	"serial 1bit     "
	.text	"parallel        "
	.text	"TCBM            "
	.text	"C16STM          "
	.text	"TCBM standard   "
io_drvloadernames:
		//   0123456789abcdef
	.text	"1541 serial 1bit"
	.text	"1581 serial 1bit"
	.text	"1541 parallel   "
	.text	"1551 TCBM       "
 	.text	"SD2IEC G.I. Joe "
 	.text	"n/a             "
	#endif

// contains the start address of the selected loader with the end address on the next word
io_plus4loaders:
	.word	io_plus4serial1bit
	.word	io_plus4parallel
	.word	io_plus4tcbm
	.word	io_plus4c16stm
	.word	io_plus4tcbm_standard
// contains the start address and end addresses of the selected loader
io_driveloaders:
	.word	io_drv1541ser1, io_drv1581ser1
	.word	io_drv1581ser1, io_drv1541par
	.word	io_drv1541par, io_drvtcbm
	.word	io_drvtcbm, io_drvgijoe
	.word	io_drvgijoe, io_loaderend

	.memblock "plus/4 side loader codes"
io_plus4serial1bit:
	#import "../loadercode/serial_1bit.inc"
io_plus4parallel:
	#import "../loadercode/parallel.inc"
io_plus4tcbm:
	#import	"../loadercode/tcbm.inc"
io_plus4c16stm:
	#import "../loadercode/c16stm.inc"
io_plus4tcbm_standard:
	#import "../loadercode/tcbm_standard.inc"

	.memblock "Drive side loader codes"
io_drv1541ser1:
	.import c64 "../drivecode/serial_1bit_1541.prg"
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
	.memblock "Exompizer decruncher"
	io_decrunchdata:
		.import c64 "../depacker/decrunch.prg"
	.label io_decrunchdatasize=*-io_decrunchdata
#endif
}
}
}