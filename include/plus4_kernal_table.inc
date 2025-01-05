#importonce
#import "kernal_table.inc"
.print("Plus/4 ROM functions")

// Print a 2-byte decimal number (AC:low, XR: high byte)
//  in:
//   AC: low byte
//   XR: high byte 
//  out:
//  registers destroyed:
//   unknown
.label	lnprt	= $a45f

// Print a hexadecimal byte
//  in:
//   AC: the byte
//  out:
//  registers destroyed:
//   unknown
.label hexbout	= $fb10

.label	defkey	= $ff49
.label	print	= $ff4c
.label	primm	= $ff4f
.label	monitor	= $ff52
