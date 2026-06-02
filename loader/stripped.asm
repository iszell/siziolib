//	#import	"plus4_basic_header.inc"
	#import	"plus4_kernal_table.inc"
	#import	"ted.inc"

	* = $1030 "Stripped loader init"
	jsr	iolib.detect.detect
	lda	iolib.detect.drive_model
	bne	!+
quit:
	jmp $fff6

!:	jsr	iolib.loader.init
	bcs  quit
#if !STRIPPEDTEST
	jmp $1000
#else
	jmp $5555
#endif

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
// Detect if ACIA is present
//	#define io_detect_acia
// Detect user port type for parallel drive interface
	#define io_detect_userport
// Detect all drives and select last used drive for loader (#8 if device number is less than 8)
	#define io_detect_drive
// Detect VICE xplus4 and optionally halt when detected
//	#define io_detect_emulator
//	#define io_halt_on_vice
// Detect CPU to get rid of 6510to7501 adapters for serial loaders
	#define io_detect_cpu_port
// Halt CPU when 6510 and serial drive detected 
	#define io_halt_on_6510

// Include loader code (drive detection and loader)
	#define need_loader
// Handle preset load address flag
	#define need_loadflag

// Include exomizer on-the-fly decruncher
//	#define need_exodecrunch

	#import "../core/iolib.inc"
