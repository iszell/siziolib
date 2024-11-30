#importonce
#import "plus4_io_map.inc"
#define HAS_TED

// TED registers 
.print("TED registers")
.namespace ted {

.label	timer1lo	= ted_base + $00
.label	timer1hi	= ted_base + $01
.label	timer2lo	= ted_base + $02
.label	timer2hi	= ted_base + $03
.label	timer3lo	= ted_base + $04
.label	timer3hi	= ted_base + $05

//  Bit7    TEST Internal TED Test    (Should be always 0)
//  Bit6    ECM  Enhanced color mode  (0=Normal, 1=Extended)
//  Bit5    BMM  Bitmap mode          (0=Text, 1=Bitmap)
//  Bit4    DEN  Display Enable       (0=Blank/Border Color, 1=Display Enable)
//  Bit3    RSEL 24/25 rows screen    (0=24 Rows, 1=25 Rows)
//  Bit2-0  Y2-0 Vertical smooth-scrolling
.label	ctrl1	= ted_base + $06

//  Bit7         Reverse Mode  (0=Reverse/128 Chars, 1=No Reverse/256 Chars)
//               If 0, higmost char-bit tells if char should appear in inverse.
//  Bit6    NTSC Select Video Format  (0=PAL, 1=NTSC) (Don't change!)
//  Bit5    RES  TED stop. If set, the TED stops it's counters and
//               screen-generating, only single clock and refresh
//               cycles remain.  PROJECT 64: Flashing (0=Yes, 1=No)
//  Bit4    MCM  Multicolor mode      (0=Normal, 1=Multicolor)
//  Bit3    CSEL 40/38 columns screen (0=38 Columns, 1=40 Columns)
//  Bit2-0  X2-0 Horizontal smooth-scrolling
.label	ctrl2	= ted_base + $07
.label	keyblatch	= ted_base + $08

//  Bit7  Set on Any-Enabled-IRQ Condition  (0=None, 1=IRQ)
//  Bit6  Timer 3 Interrupt Request         (0=None, 1=IRQ / Acknowledge)
//  Bit5  Unused
//  Bit4  Timer 2 Interrupt Request         (0=None, 1=IRQ / Acknowledge)
//  Bit3  Timer 1 Interrupt Request         (0=None, 1=IRQ / Acknowledge)
//  Bit2  Lightpen Interrupt Request        (Not implemented?)
//  Bit1  Raster Interrupt Request          (0=None, 1=IRQ / Acknowledge)
//  Bit0  Unused
.label	irqsource	= ted_base + $09

//  Bit7  Unused
//  Bit6  Timer 3 Interrupt        (0=Disable, 1=Enable)
//  Bit5  Unused
//  Bit4  Timer 2 Interrupt        (0=Disable, 1=Enable)
//  Bit3  Timer 1 Interrupt        (0=Disable, 1=Enable)
//  Bit2  Lightpen Interrupt       (Not implemented?)
//  Bit1  Raster Interrupt         (0=Disable, 1=Enable)
//  Bit0  High bit of Raster FF0Bh (see there)
.label	irqmask	= ted_base + $0a
.label	rasterirqline	= ted_base + $0b
.label	cursorhi	= ted_base + $0c
.label	cursorlo	= ted_base + $0d
.label	voice1lo	= ted_base + $0e
.label	voice2lo	= ted_base + $0f
.label	voice2hi	= ted_base + $10

//  Bit7    D/A mode.
//  Bit6    Sound #2 noise on/off. If You set both, the square will sound.
//  Bit5    Sound #2 squarewave on/off.
//  Bit4    Sound #1 squarewave on/off.
//  Bit0-3  Volume. Maximum value is 8.
.label	soundctrl	= ted_base + $11

//  Bit7-6  Unused
//  Bit5-3  Bitmap Base Address      (in 8K steps) (ROM/RAM see FF12h.Bit2)
//  Bit2    Character/Bitmap ROM/RAM (0=RAM, 1=ROM) (Address see FF13h.Bit7-2)
//  Bit1-0  Sound 2 Frequency MSB    (see Sound chapter)
.label	bitmapctrl	= ted_base + $12

//  Bit7-2  Character Base Address (in 1K steps) (ROM/RAM see FF12h.Bit2)
//  Bit1    Force single clock mode (0=double, 1=single)
//  Bit0    Memory State READ ONLY (0=RAM, 1=ROM) (change via FF3Eh/FF3Fh)
.label	charsetaddr	= ted_base + $13

//  Bit7-3  Video-ram Base Address (in 2K steps) (ROM/RAM see FF3Eh/FF3Fh)
//  Bit2-0  Unused
.label	screenaddr	= ted_base + $14
.label	background	= ted_base + $15
.label	color1	= ted_base + $16
.label	color2	= ted_base + $17
.label	color3	= ted_base + $18
.label	border	= ted_base + $19

// Bit7-3   Unused
// Bit1-0   Current character position MSB
.label	currcharhi	= ted_base + $1a
// Bit7-0   Current character position LSB
.label	currcharlo	= ted_base + $1b

// Bit7-1   Unused
// Bit0     MSB of register $1d
.label	rasterlinehi= ted_base + $1c
.label	rasterline	= ted_base + $1d
.label	rastercolumn= ted_base + $1e

//  Bit7    Unused
//  Bit6-3  Flashing counter. It's value increases with every
//             frame, and TED fits it's flashing feature to this
//             register's reaching to 15.
//  Bit2-0  Actual vertical scanning-line in a character-row (R/W!)
.label	chrscanline	= ted_base + $1f

.label	romen	= ted_base + $3e
.label	ramen	= ted_base + $3f
}