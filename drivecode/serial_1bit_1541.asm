	.namespace iolib {
	.namespace serial1bit1541 {
	*= $0500
{

.var	drive_serial	= true
.var	ledinverted	= false

	#import	"core.inc"
	#import	"serial_1bit_core.inc"

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
//           00 = #8, 01 = #9, 10 = #10, 11 = #11
//Bit  4   ATN acknowledge OUT
//Bit  3   CLOCK OUT
//Bit  2   CLOCK IN
//Bit  1   DATA OUT
//Bit  0   DATA IN
.label	serialdata = $1800

.label	ledport	= $1c00
.const	ledvalue	= %00001000

.const	dirsect	= 1

.label	blk4buf	= $0700

	driveload(init, dirtrack, dirsect, blk4trk, blk4sec, blk4job, blk4buf, retrycnt, hdrid1, hdrid2, drv0id1, drv0id2, readbyte, writebyte, ledport, ledvalue, serialdata, drive_serial, ledinverted)

init:	driveserial1bitinit(serialdata)
readbyte:
	driveserial1bitread(serialdata, bitbuff)
writebyte:
	driveserial1bitwrite(serialdata, bitbuff)

dirtrack:
	.byte	18

}
}
}
