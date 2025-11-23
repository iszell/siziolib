	#import	"plus4_basic_header.inc"
	#import	"plus4_kernal_table.inc"
	#import	"ted.inc"

.label loader = $0be8

	jsr	iolib.detect.detect
	lda	iolib.detect.drive_model
	bne	!+
quit:
	jmp $fff6

!:	sei
	jsr	iolib.loader.init
	bcs  quit

	ldx #end - loadcode - 1
!:	lda loadcode,x
	sta loader,x
	dex
	bpl !-
	jmp loader

loadcode:
	sta ted.ramen
	.encoding "petscii_mixed"
	ldx #'i'
	ldy #'i'
	jsr iolib.decrunch
	jmp $100d
end:

	// Custom memory locations
	#define io_skip_default_definitions
	.namespace iolib {
		.label	io_base		= $0700
		.label	io_tcbmoffs	= io_base - 1
		.label  io_bitbuff	= $b7
		.label  io_loadptr  = $9e
		.label  io_loadflag = $9d
	}
//	#define io_prtstatus
// Detect video standard. Not really useful except for printing status message
//	#define io_detect_video
// Detect memory size and expansion type
//	#define io_detect_memory_size
// Detect available sound expansions including SID type and address
//	#define io_detect_sound
	#define io_detect_drive
// Detect VICE xplus4 and optionally halt when detected
//	#define io_detect_emulator
//	#define io_halt_on_vice
// Detect CPU to get rid of 6502to7501 adapters
	#define io_detect_cpu_port
	#define io_halt_on_6502

// Include loader code (drive detection and loader)
	#define need_loader
// Handle preset load address flag
//	#define need_loadflag

// Include exomizer on-the-fly decruncher
	#define need_exodecrunch

	#import "../core/iolib.inc"
