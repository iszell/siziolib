	#import	"plus4_basic_header.inc"
	#import	"plus4_kernal_table.inc"
	#import	"ted.inc"
	#import	"ay.inc"
	#import	"fm.inc"

	.encoding	"petscii_mixed"

	jsr	primm
	.text	"Hardware detect using IOLib by Siz"
	.byte	14, 13, 0
	
	jsr	iolib.detect.detect
	rts

	#define io_prtstatus
	#define io_detect_video
	#define io_detect_sound
	#define io_detect_drive
	#define io_memory_size
	#define io_detect_emulator
//	#define io_halt_on_vice
	#define io_detect_cpu_port

	#import "../detect/detect.inc"
