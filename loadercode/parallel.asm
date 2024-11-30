	.namespace iolib {
	.namespace plus4parallel {
	#import "plus4_io_map.inc"

	*= io_base
{

// default value %11001000
//  Bit Dir Expl.
//  7   In  Serial Data In (and PLUS/4: Cas Sense) (0=Low, 1=High)
//  6   In  Serial Clock In                        (0=Low, 1=High)
//  5   -   Not used (no pin-out on 7501/8501)     (N/A)
//  4   In  Cassette Read                          (0=Low, 1=High)
//  3   Out Cassette Motor                         (0=9VDC, 1=Off) (Inverted)
//  2   Out Serial ATN Out (and PLUS/4: User Port) (0=High, 1=Low) (Inverted)
//  1   Out Serial Clock Out, Cassette Write       (0=High, 1=Low) (Inverted)
//  0   Out Serial Data Out                        (0=High, 1=Low) (Inverted)
.label	serialdata = cpu_port

	jmp	startload
	jmp	readbyte
	jmp	writebyte
	jmp	hardsync
	jmp	sync
init:	lda #%11111111
	sta pio_parport
	lda #%00001010	// Casette motor OFF// CLK=LOW
	sta serialdata
!:	bit serialdata	// DATA=LOW?
	bmi !-		// no
	lda #%00001000	// Casette motor OFF// CLK=HIGH// DATA=HIGH
	sta serialdata
hardsync:
sync:	rts

readbyte:
	lda #%11111111	// Set all bits to input
	sta pio_parport
!:	bit serialdata	// DATA=LOW?
	bmi !-		// no
	lda #%00001010	// Casette motor OFF// CLK=LOW
	sta serialdata
!:	bit serialdata	// DATA=HIGH?
	bpl !-		// no
	lda pio_parport	// read data byte
	pha
	lda #%00001000	// Casette motor OFF// CLK=HIGH// DATA=HIGH
	sta serialdata
	pla
	rts
	
writebyte:
!:	bit serialdata	// DATA=LOW?
	bmi !-		// no
	sta pio_parport	// write data byte
	lda #%00001010	// Casette motor OFF// CLK=LOW
	sta serialdata
!:	bit serialdata	// DATA=HIGH?
	bpl !-		// no
	lda #%00001000	// Casette motor OFF// CLK=HIGH// DATA=HIGH
	sta serialdata
	rts
	#import	"core.inc"

startload:
	plus4load(readbyte, writebyte, sync, hardsync)
}
}
}