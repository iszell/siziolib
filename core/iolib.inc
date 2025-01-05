// IOLib by Siz of ~TBC~ Absence
// (c) Siz, 2014.06.03
//
// Change log V3:
// 	2014.06.03: first touch
//	2015.04.27: implemented sound and memory size detection
//	2015.04.28: implemented drive type detection
//	2015.04.29: implemented drive interface type detection (except parallel)
//	2015.05.01: implemented parallel drive interface detection
//	2015.05.03: implemented loader selection
//	2015.05.06: added G.I. Joe loader definition for SD2IEC
//	2015.05.11: implemented G.I. Joe loader drive part (used for SD2IEC only!)
//	2015.05.15: serial 1 bit plus/4 loader (G.I. Joe) is done
//	2015.05.20: serial 1 bit 1581 loader is finally done (G.I. Joe loader is missing a SEI for read error condition)
//	2015.05.20: serial 1 bit 1541 loader basically G.I. Joe with the SEI fix
//	2015.05.23: implemented 1541 parallel and 1551 TCBM loader, drive code download fix
//	2015.08.14: bugfix in SID type detection when no SIDCard present
//	2016.04.02: exomizer decruncher during load
//	2016.09.06: modular architecture
//	2017.05.08: first public release (no code changes, added readme)
//	2017.05.10: dual 1551 fix
//	2020.05.30: change copyright message
//	2021.06.17: migrate sources to KickAss
//	2024.01.27: C16STM support
//	2024.08.11: YaPE 1551 IEC and SD2TCBM detection
//	2024.08.21: Detect CPU port type
// Change log V4:
//	2024.11.30: Starting V4 branch

	#import "iolib_def.inc"

	.print	"Siz IOLib startup code"

	.namespace iolib {
	.encoding	"petscii_mixed"

	#import	"../detect/detect.inc"
 	#import	"../core/iolib_init.inc"
}

/*
Protocol:
Send 3 bytes:
- file name length (ignored)
- first character of filename
- second character of filename
Byte read:
- marker byte: $ac
- multibyte return: when the marker byte is returned a second read is necessary to get the proper value:
- - marker ($ac): the marker byte should be stored
- - $ff: EOF, no more bytes to read
- - $f7: read error, file is closed
Load:
- always whole files are read (including load address)
- uncompressed load: first two bytes returned are the load address and loaded content will be stored starting from there
- compressed (exomizer mem -f): first two bytes are ignored and file is decomressed on the fly (no storing and uncompressing). Start address is included in compressed bytestream so normal start address is ignored.
*/