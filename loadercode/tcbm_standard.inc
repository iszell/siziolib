	.print	". plus/4 TCBM standard loader"
	.namespace iolib {
	.namespace plus4tcbm_standard {
	#import "iolib_def.inc"
	#import "ted.inc"

	.pseudopc io_base
{

.label	tia	= tcbm_9
.label	padta	= tia
.label	pbdta	= tia+1
.label	pcdta	= tia+2
.label	padir	= tia+3
.label	pbdir	= tia+4
.label	pcdir	= tia+5

	jmp	startload
init:
hardsync:
sync:	rts

readbyte:
	ldx	io_tcbmoffs
	rts

writebyte:
	ldx	io_tcbmoffs
	rts
	#import	"core.inc"

startload:
	plus4load(readbyte, writebyte, sync, hardsync)
}
}
}
