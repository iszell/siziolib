//	#import	"plus4_basic_header.inc"
	#import	"plus4_kernal_table.inc"
	#import	"ted.inc"

	* = $1030
	jsr	iolib.detect
	lda	iolib.drivedetect.io_drivetyp
	bne	!+
quit:
	jmp $fff6
!:	sei
	jsr	iolib.init
	bcs  quit
	jmp $1000
//	jmp $5555

.namespace iolib {
	.label	io_tcbmoffs	= $06ff
	.label	io_base		= $0700
	.label  io_bitbuff	= $b7
	.label  io_loadptr  = $9e
	.label  io_loadflag = $9d
}
#undef prtstatus
#define need_loader
#define need_loadflag
#define skip_default_definitions
#define bypass_vice
#import "../core/iolib.inc"
