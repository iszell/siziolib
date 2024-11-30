#importonce
.print("CBM KERNAL jump-table")

.label	st	= $90

.label	vidinit	= $ff81
.label	ioinit	= $ff84
.label	ramtest	= $ff87
.label	restorio= $ff8a
// X/Y: user table address, C=0: copy user table to vectors, C=1: copy vectors to user table
.label	vector	= $ff8d
.label	setmsg	= $ff90
.label	second	= $ff93
.label	tksa	= $ff96
.label	memtop	= $ff99
.label	membot	= $ff9c
.label	scnkey	= $ff9f
.label	settmo	= $ffa2
.label	iecin	= $ffa5
.label	iecout	= $ffa8
.label	untalk	= $ffab
.label	unlisten= $ffae
.label	listen	= $ffb1
.label	talk	= $ffb4
.label	readst	= $ffb7
// A: logical file number, X: device number, Y: secondary address
.label	setlfs	= $ffba
// A: length, X/Y: pointer to filename
.label	setnam	= $ffbd
// must follow setlfs and setnam
.label	open	= $ffc0
// A: logical file number to close
.label	close	= $ffc3
// X: logical file number for input
.label	chkin	= $ffc6
// X: logical file number for output
.label	chkout	= $ffc9
// clear input/output channels and set them back to defaults
.label	clrchans= $ffcc
.label	chrin	= $ffcf
.label	chrout	= $ffd2
// A=0: load, A=1-255: verify, X/Y load address (if secondary address=0, otherwise file first two bytes)
.label	load	= $ffd5
// A: zero page pointer number for start address, X/Y: end address+1
.label	save	= $ffd8
.label	settime	= $ffdb
.label	readtim	= $ffde
.label	stop	= $ffe1
.label	getchar	= $ffe4
.label	closeall= $ffe7
.label	udtime	= $ffea
.label	scrnsize= $ffed
.label	plot	= $fff0
.label	iobase	= $fff3
.label	reset	= $fff6
