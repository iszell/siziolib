	.namespace iolib {
	.namespace serial1bit1581 {
	*= $0500
{

.var	drive_serial	= true
.var	ledinverted	= false

	#import	"core.inc"
	#import	"serial_1bit_core.inc"

.label	blk4job	= $06
.label	blk4trk	= $13
.label	blk4sec	= $14
.label	drv0id1	= $1d
.label	drv0id2	= $1e
.label	bitbuff	= $5e
.label	hdrid1	= $1d
.label	hdrid2	= $1e
.label	retrycnt	= $5f

// serial bus bits are inverted!
// default value %01010000
// Bit 7 ATN IN
// Bit 6 0 = Write protect active
// Bit 5 Data Direction of the Bus Driver (FSM)
//         0 = Input, 1 = Output
// Bit 4 automatic ATN-Response
// Bit 3 CLOCK OUT
// Bit 2 CLOCK IN
// Bit 1 DATA OUT
// Bit 0 DATA IN

.label	serialdata = $4001

.label	ledport	= $79
.const	ledvalue	= %01000000

.label	blk4buf	= $0700

.const	dirsect	= 3
.label	dirtrack	= $022b

	driveload(init, dirtrack, dirsect, blk4trk, blk4sec, blk4job, blk4buf, retrycnt, hdrid1, hdrid2, drv0id1, drv0id2, readbyte, writebyte, ledport, ledvalue, serialdata, drive_serial, ledinverted)

init:	driveserial1bitinit(serialdata)
readbyte:
	driveserial1bitread(serialdata, bitbuff)
writebyte:
	driveserial1bitwrite(serialdata, bitbuff)

}
}
}
