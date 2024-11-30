	.namespace iolib {
	.namespace tcbm1551 {
	*=$0500
{

.var	drive_serial	= false
.var	ledinverted	= true

	#import	"core.inc"

.label	cpuport  = $01

.label	blk4job  = $06
.label	blk4trk  = $10
.label	blk4sec  = $11
.label	drv0id1  = $14
.label	drv0id2  = $15
.label	retrycnt = $16
.label	hdrid1   = $18
.label	hdrid2   = $19

.label	blk4buf  = $0700

.label	tia      = $4000
.label	padta    = tia
.label	pbdta    = tia+1
.label	pcdta    = tia+2
.label	padir    = tia+3
.label	pbdir    = tia+4
.label	pcdir    = tia+5

.label	ledport	= cpuport
.const	ledvalue = %00001000

.const	dirsect	= 1

/*
Memory map of the 1551:

Location	Purpose
$0000-0001	8 bit port - 6510T

	$00 : cpu data direction register 
	$01 : cpu port register

		bit 0: drive head stepper 0 (STP)
		bit 1: drive head stepper 1 (STP)
		bit 2: drive motor on/off (MTR)
		bit 3: drive LED (ACT)
		bit 4: write protect sensor (WPS)
		bit 5: density select #0 (DS0)
		bit 6: density select #1 (DS1)
		bit 7: byte latched (1 - yes, 0 - no)

$0002-00ff	Zero page work area, *job queue, variables
$0100-01ff	GCR overflow area and stack
$0200-02ff	Command buffer, parser, tables, variables
$0300-06ff	4 data buffers, 0-3
$0700-07ff	BAM resident in memory
$4000-4007	6523a I/O to computer controller
$c000-ffe5	16KB ROM, DOS and controller routines
$ffe6-ffff	JMP table, user command vectors
*job queue and HDRS offset by 2 bytes - starts at $0002


TIA 6523 - Triport Interface Adapter

The TIA6523 is a tri-port adapter. It has 28 pins,
three data direction and three port registers. It's
mapped in at $4000-$4007 in the 1551. Some drives actually
contain a 6525 with a 6523 pinout.

Address		6523 TIA ($4000-$4007)
-------------------------------------------------------------------------------
    0		Port A Data (data sent to the computer)
    1		Port B Data (connected to the shift register of the R/W head)
    2		Port C Data
			bit 7: DAV (DAta Valid)
				handshake from bit #6 from $FEF2/FEC2 of plus/4
			bit 6: SYNC
				0 -> SYNC found
				1 -> no SYNC
			bit 5: MODE (jumper)
				drive number (0 -> 8, 1 -> 9)
			bit 4: MODE (R/W head)
				0 -> head in write mode
				1 -> head in read mode
			bit 3: ACK (ACKnowledge)
				handshake to bit #7 of $FEF2/$FEC2 
			bit 2: DEV
				1 -> drive mapped at $FEF0 on plus/4
				0 -> drive mapped at $FEC0 on plus/4
			bit 1: STATUS1 - mapped at $FEF1/FEC1 on plus/4
			bit 0: STATUS0 - mapped at $FEF1/FEC1 on plus/4
					  01 Timeout during reading
					  10 Timeout during writing
					  11 End of data
   Data direction registers (	1 -> bit out, 0 -> bit in)

   3		Port A Direction
   4		Port B Direction 
   5		Port C Direction (set to $1F by the drive ROM)
   6		N/A
   7		N/A
*/

	driveload(init, dirtrack, dirsect, blk4trk, blk4sec, blk4job, blk4buf, retrycnt, hdrid1, hdrid2, drv0id1, drv0id2, readbyte, writebyte, ledport, ledvalue, 0, drive_serial, ledinverted)

init:
	sei
	cld
    lda #%11111111	//dir=output
    sta padir
	lda pcdta
	and #%11110100	//ACK=0
	sta pcdta
!:	bit pcdta	//DAV=1
	bmi !-		//yes
	rts
writebyte:
!:	bit pcdta	//DAV=1?
	bpl !-		//no
	stx padta	//put byte
	lda pcdta
	ora #%00001000
	sta pcdta	//ACK=1
!:	bit pcdta	//DAV=1?
	bmi !-		//yes
	lda pcdta
	and #%11110111
	sta pcdta	//ACK=0
	rts
readbyte:
	inc padir
!:	bit pcdta	//DAV=1?
	bpl !-		//no
	lda padta	//get byte
	pha
	lda pcdta
	ora #%00001000
	sta pcdta	//ACK=1
!:	bit pcdta	//DAV=0?
	bmi !-		//yes
	lda pcdta
	and #%11110111
	sta pcdta	//ACK=0
	dec padir
	pla
	rts

dirtrack:
	.byte	18

}
}
}
