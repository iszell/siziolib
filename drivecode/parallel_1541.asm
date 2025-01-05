	.namespace iolib {
	.namespace parallel1541 {
	*= $0500
{

.var	drive_serial	= false
.var	ledinverted	= false

	#import	"core.inc"

.label	blk4job	= $04
.label	blk4trk	= $0e
.label	blk4sec	= $0f
.label	drv0id1	= $12
.label	drv0id2	= $13
.label	bitbuff	= $14
.label	hdrid1	= $16
.label	hdrid2	= $17
.label	retrycnt	= $8b

// serial bus bits are inverted!
// default value %00000000
//Bit  7   ATN IN
//Bits 6-5 Device address preset switches:
//	  00 = #8, 01 = #9, 10 = #10, 11 = #11
//Bit  4   ATN acknowledge OUT
//Bit  3   CLOCK OUT
//Bit  2   CLOCK IN
//Bit  1   DATA OUT
//Bit  0   DATA IN
.label	serialport = $1800
.label	parport	= $1801
.label	pardir	= $1803

.label	ledport	= $1c00
.const	ledvalue	= %00001000

.const	dirsect	= 1

.label	blk4buf	= $0700

	driveload(init, dirtrack, dirsect, blk4trk, blk4sec, blk4job, blk4buf, retrycnt, hdrid1, hdrid2, drv0id1, drv0id2, readbyte, writebyte, ledport, ledvalue, 0, drive_serial, ledinverted)

init:	sei
	cld
	lda #%11111111
	sta pardir
	lda #%00000100	//CLK=0?
!:	bit serialport
	beq !-		//yes
	ldx #%00000010
	stx serialport
!:	bit serialport
	bne !-		//no
	rts

writebyte:
	lda #%00000100	//CLK=0?
!:	bit serialport
	beq !-		//yes
	stx parport	//put byte
	lda #%00000000	//CLK=0,DATA=0
	sta serialport
	lda #%00000100	//CLK=1?
!:	bit serialport
	bne !-		//no
	lda #%00000010	//CLK=0,DATA=1
	sta serialport
	rts

readbyte:
	inc pardir
	lda #%00000100	//CLK=0?
!:	bit serialport
	beq !-		//yes
	lda parport	//get byte
	pha
	lda #%00000000	//CLK=0,DATA=0
	sta serialport
	lda #%00000100	//CLK=0?
!:	bit serialport
	bne !-		//no
	lda #%00000010
	sta serialport	//CLK=0,DATA=1
	dec pardir
	pla
	rts

dirtrack:
	.byte	18

}
}
}
