	#import	"plus4_basic_header.inc"
	#import	"plus4_io_map.inc"
	#import	"ted.inc"
	#import	"plus4_kernal_table.inc"

	.encoding	"petscii_mixed"

	jsr	primm
	.text	"Hardware detect using IOLibV3 by Siz"
	.byte	14, 13, 0

	jsr	iolib.detect
	rts

.namespace iolib {
	#define	prtstatus
	#define need_cpu_detect
	#define need_video_detect
	#define need_memory_detect
	#define need_sound_detect
	#define bypass_vice
}
#import "../core/iolib.inc"
